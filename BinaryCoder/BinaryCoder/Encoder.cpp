//
//  Encoder.cpp
//  BinaryCoder
//
//  Created by hailong on 13-2-9.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#include "Encoder.h"
#include "Stream.h"

namespace binary_coder {
    Encoder::Encoder(OutputStream* streamOut) {
        stream_ = streamOut;
        buffer_size_ = BUFFER_SIZE;
        buffer_ = new uint8_t[buffer_size_];
        position_ = 0;
        index_ = 0;
        offset_ = 0;
        
        // error_ = NoError;
        ResetError();
    }
    
    Encoder::~Encoder()
    {
        delete[] buffer_;
    }
    
    bool Encoder::_SetError(error_t err)
    {
        if (error_ == NoError) {
            error_ = err;
            return true;
        } else {
            return false;
        }
    }
    
    long Encoder::Mark() {
        long current = position_ + index_;
        locations_.push_back(current);
        return current;
    }
    
    void Encoder::Unmark() {
        locations_.pop_back();
    }
    
    long Encoder::Check(long expected) {
        long last;
        if (locations_.empty()) {
            last = 0;
        } else {
            last = locations_.back();
        }
        
        long current = position_ + index_;
        long real = current - last;
        return real - expected;
    }
    
    void Encoder::AlignToByte() {
        if (offset_ > 0) {
            index_ += 1;
            offset_ = 0;
        }
    }
    
    void Encoder::Flush() {
        int diff = offset_ == 0? 0: 1;
        
        stream_->Write(buffer_, 1, index_+diff);
        stream_->Flush();
        
        if (diff != 0) {
            buffer_[0] = buffer_[index_];
        }
        
//        for (int i = diff; i < buffer_size_; i++) {
//            buffer_[i] = 0;
//        }
        memset(buffer_, 0, sizeof(uint8_t)*buffer_size_);
        
        position_ += index_;
        index_ = 0;
    }
    
    void Encoder::WriteBits(int value, int numberOfBits) {
        long ptr = (index_ << BYTES_TO_BITS) + offset_ + numberOfBits;
        
        if (ptr >= (buffer_size_ << BYTES_TO_BITS)) {
            Flush();
        }
        
        int val = ((value << (BITS_PER_INT - numberOfBits)) >> /*>>>*/ offset_) | (buffer_[index_] << TO_BYTE3);
        int base = BITS_PER_INT - (((offset_ + numberOfBits + ROUND_TO_BYTES) >> /*>>>*/ BITS_TO_BYTES) << BYTES_TO_BITS);
        base = base < 0 ? 0 : base;
        
        long pointer = (index_ << BYTES_TO_BITS) + offset_;
        
        for (int i = 24; i >= base; i -= BITS_PER_BYTE) {
            buffer_[index_++] = (uint8_t) (val >> /*>>>*/ i);
        }
        
        if (offset_ + numberOfBits > BITS_PER_INT) {
            buffer_[index_] = (uint8_t) (value << (BITS_PER_BYTE - (offset_ + numberOfBits - BITS_PER_INT)));
        }
        
        pointer += numberOfBits;
        index_ = pointer >> /*>>>*/ BITS_TO_BYTES;
        offset_ = pointer & LOWEST3;
    }
    
    void Encoder::WriteByte(int value) {
        if (index_ == buffer_size_) {
            Flush();
        }
        buffer_[index_++] = (uint8_t) value;
    }
    
    size_t Encoder::WriteBytes(const uint8_t* bytes, size_t numOfBytes) {
        if (index_ + numOfBytes < buffer_size_) {
            memcpy(buffer_+index_, bytes, numOfBytes);
            index_ += numOfBytes;
        } else {
            Flush();
            stream_->Write(bytes, 1, numOfBytes);
            position_ += numOfBytes;
        }
        return numOfBytes;
    }
    
    void Encoder::WriteShort(int value) {
        if (index_ + 2 > buffer_size_) {
            Flush();
        }
        buffer_[index_++] = (uint8_t) value;
        buffer_[index_++] = (uint8_t) (value >> /*>>>*/ TO_BYTE1);
    }
    
    void Encoder::WriteInt(int value) {
        if (index_ + 4 > buffer_size_) {
            Flush();
        }
        buffer_[index_++] = (uint8_t) value;
        buffer_[index_++] = (uint8_t) (value >> /*>>>*/ TO_BYTE1);
        buffer_[index_++] = (uint8_t) (value >> /*>>>*/ TO_BYTE2);
        buffer_[index_++] = (uint8_t) (value >> /*>>>*/ TO_BYTE3);
    }
    
    void Encoder::WriteString(const std::string str) {
        WriteBytes((const uint8_t*)str.c_str(), str.size());
        WriteByte(0);
    }
} /* binary_coder */
