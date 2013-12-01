//
//  CrypticStream.h
//  BinaryCoder
//
//  Created by hailong on 13-4-5.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#ifndef __BinaryCoder__CrypticStream__
#define __BinaryCoder__CrypticStream__

#include "Stream.h"
extern "C" {
#include "des.h"
}

namespace binary_coder {

    class InputStreamDES: public InputStream
    {
    public:
        InputStreamDES(InputStream* s, size_t encrypted_bytes=-1, bool retain = false);
        ~InputStreamDES();
        
        virtual int Seek(long offset, int origin);
        virtual size_t Read(void* ptr, size_t size, size_t count);
        virtual size_t Tell() const;
        virtual int Eof() const;
        virtual error_t Error() const { return err_; }
        
        /* Set DES key.
          key          64 bits key (only 56 bits used)
         */
        void setDESKey(const uint8_t key[8]);
    private:
        DES_KS ks_;
    protected:
        InputStream* stream_;
        size_t start_;
        size_t length_;
        error_t err_;
        bool retain_;
        
        struct {
            size_t dirty: 3;
            size_t index: sizeof(size_t)-3;
        } block_info_;
        uint8_t block_[8];
        size_t position_;
        
        void _SetError(error_t err) {
            if (err_ == NoError) {
                err_ = err;
            }
        }
    };
    
    class OutputStreamDES: public OutputStream
    {
    public:
        OutputStreamDES(OutputStream* s, size_t preferred_buffer_size = 4096, bool retain = false);
        virtual ~OutputStreamDES();
        
        virtual size_t Write(const void* ptr, size_t size, size_t count);
        // Write buffers to output stream.
        // If length of data in buffer is not the multiple of 8, the remaining bytes will not be flushed.
        virtual void Flush();
        virtual void Seal();
        virtual error_t Error() const { return err_; }
        
        /* Set DES key.
         key          64 bits key (only 56 bits used)
         */
        void setDESKey(const uint8_t key[8]);
    private:
        DES_KS ks_;
    private:
        OutputStream* stream_;
        error_t err_;
        bool retain_;
        bool is_sealed_;
        
        uint8_t* buffer_;
        size_t buffer_size_;
        size_t bytes_in_buffer_;
        
        void _SetError(error_t err) {
            if (err_ == NoError) {
                err_ = err;
            }
        }
        void _Flush(bool bSeal);
    };

} /* binary_coder */
#endif /* defined(__BinaryCoder__CrypticStream__) */
