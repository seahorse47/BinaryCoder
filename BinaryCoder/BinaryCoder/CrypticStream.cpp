//
//  CrypticStream.cpp
//  BinaryCoder
//
//  Created by hailong on 13-4-5.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#include "CrypticStream.h"

namespace binary_coder {
    
    InputStreamDES::InputStreamDES(InputStream* s, size_t encrypted_bytes, bool retain/* = false*/)
    {
        stream_ = s;
        start_ = s->Tell();
        s->Seek(0, SEEK_END);
        size_t end = s->Tell();
        s->Seek(start_, SEEK_SET);
        size_t length = end-start_;
        if (encrypted_bytes < length) {
            length_ = encrypted_bytes;
        } else {
            length_ = length;
        }
        position_ = 0;
        block_info_.index = 0;
        block_info_.dirty = 1;
        retain_ = retain;
        err_ = NoError;
    }
    
    InputStreamDES::~InputStreamDES()
    {
        if (retain_ && stream_ != NULL) {
            delete stream_;
            stream_ = NULL;
        }
    }
    
    void InputStreamDES::setDESKey(const uint8_t key[8])
    {
        deskey(ks_, (unsigned char*)key, 1);
    }
    
    int InputStreamDES::Seek(long offset, int origin)
    {
        if (err_ != NoError) {
            return -1;
        }
        size_t begin = 0;
        if (origin == SEEK_CUR) {
            begin = position_;
        } else if (origin == SEEK_END) {
            begin = length_;
        }
        position_ = begin + offset;
        return 0;
    }
    
    size_t InputStreamDES::Read(void* ptr, size_t size, size_t count)
    {
        if (err_ != NoError) {
            return 0;
        }
        
        uint8_t* start = (uint8_t*)ptr;
        uint8_t* end = start + size*count;
        uint8_t* p = start;
        while (p < end) {
            if (block_info_.dirty || block_info_.index != (position_>>3)) {
                block_info_.index = (position_>>3);
                size_t pos = start_ + (block_info_.index << 3);
                if (stream_->Tell() != pos) {
                    stream_->Seek(pos, SEEK_SET);
                }
                size_t c = stream_->Read(block_, 8, 1);
                if (c < 8) {
                    _SetError(InvalidData);
                    break;
                }
                
                des(ks_, block_);
                
                block_info_.dirty = 0;
            }
            
            int n = p+8>end? (int)(p+8-end): 8;
            for (int i=0; i<n; i++) {
                p[i] = block_[i];
            }
            position_ += n;
            p += n;
        }
    
        return p - start;
    }
    
    size_t InputStreamDES::Tell() const
    {
        return position_;
    }
    
    int InputStreamDES::Eof() const
    {
        if (position_ >= length_) {
            return 1;
        } else {
            return 0;
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    OutputStreamDES::OutputStreamDES(OutputStream* s, size_t preferred_buffer_size, bool retain/* = false*/)
    {
        stream_ = s;
        retain_ = retain;
        err_ = NoError;
        is_sealed_ = false;
        
        if (preferred_buffer_size >= 64) {
            buffer_size_ = (preferred_buffer_size + 7) & ~0x07;
        } else {
            buffer_size_ = 4096;
        }
        buffer_ = (uint8_t*)malloc(buffer_size_);
        bytes_in_buffer_ = 0;
    }
    
    OutputStreamDES::~OutputStreamDES()
    {
        if (!is_sealed_) {
            Seal();
        }
        
        free(buffer_);
        buffer_ = NULL;
        
        if (retain_ && stream_ != NULL) {
            delete stream_;
            stream_ = NULL;
        }
    }
    
    void OutputStreamDES::setDESKey(const uint8_t key[8])
    {
        deskey(ks_, (unsigned char*)key, 0);
    }
    
    size_t OutputStreamDES::Write(const void* ptr, size_t size, size_t count)
    {
        if (err_ != NoError) {
            return 0;
        }
        if (is_sealed_) {
            _SetError(StreamIsClosed);
            return 0;
        }
        
        size_t total = size*count;
        size_t left = total;
        uint8_t* src = (uint8_t*)ptr;
        do {
            if (bytes_in_buffer_ >= buffer_size_) {
                _Flush(false);
            }
            if (err_ != NoError) {
                break;
            }
            assert(bytes_in_buffer_ < buffer_size_);
            if (left == 0) {
                break;
            }
            size_t n = left+bytes_in_buffer_ > buffer_size_? buffer_size_-bytes_in_buffer_: left;
//            assert(n<=left);
            memcpy(buffer_+bytes_in_buffer_, src, n);
            src += n;
            left -= n;
            bytes_in_buffer_ += n;
        } while (true);
        return total - left;
    }
    
    void OutputStreamDES::Flush()
    {
        _Flush(false);
    }
    
    void OutputStreamDES::Seal()
    {
        _Flush(true);
        is_sealed_ = true;
    }
    
    void OutputStreamDES::_Flush(bool bSeal)
    {
        if (bytes_in_buffer_ > 0) {
            size_t l = bytes_in_buffer_ & (~0x07);
            uint8_t* ptr = buffer_;
            if (l > 0) {
                uint8_t* end = buffer_ + l;
                while (ptr < end) {
                    des(ks_, ptr);
                    ptr += 8;
                }
            }
            
            size_t remains = bytes_in_buffer_ - l;
            assert(remains >= 0 && remains < 8);
            if (bSeal && remains > 0) {
                des(ks_, ptr);
                l += 8;
                remains = 0;
            }
            
            if (l > 0) {
                stream_->Write(buffer_, l, 1);
                stream_->Flush();
                
                if (remains > 0) {
                    for (int i=0; i<remains; i++) {
                        buffer_[i] = buffer_[l+i];
                    }
                }
                bytes_in_buffer_ = remains;
            }
        }
    }
} /* binary_coder */
