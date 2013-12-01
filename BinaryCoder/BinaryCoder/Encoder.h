//
//  Encoder.h
//  BinaryCoder
//
//  Created by hailong on 13-2-9.
//  Copyright (c) 2013年 hailong. All rights reserved.
//

#ifndef BINARYCODER_ENCODER_H_
#define BINARYCODER_ENCODER_H_

#include "STDHeaders.h"
#include "Constants.h"

namespace binary_coder {
    class OutputStream;
    class Encoder
    {
    public:
        /**
         * Create a new SWFEncoder for the underlying InputStream with the
         * specified buffer size.
         *
         * @param streamOut the stream from which data will be written.
         */
        Encoder(OutputStream* streamOut);
        ~Encoder();
        
        error_t GetLastError() const {
            return error_;
        }
        
        void ResetError() {
            error_ = NoError;
        }
        
        /**
         * Remember the current position.
         * @return the current position.
         */
        long Mark();
        
        /**
         * Discard the last saved position.
         */
        void Unmark();
        
        /**
         * Compare the number of bytes read with the expected number and throw an
         * exception if there is a difference.
         *
         * @param expected the expected number of bytes read.
         * @return the differece between real and expected length.
         */
        long Check(long expected);
        
        /**
         * Changes the location to the next byte boundary.
         */
        void AlignToByte();
        
        /**
         * Write the data currently stored in the buffer to the underlying stream.
         */
        void Flush();

        /**
         * Write a value to bit field.
         *
         * @param value
         *            the value.
         * @param numberOfBits
         *            the (least significant) number of bits that will be written.
         * stream.
         */
        void WriteBits(int value, int numberOfBits);

        /**
         * Write a byte.
         *
         * @param value
         *            the value to be written - only the least significant byte will
         *            be written.
         */
        void WriteByte(int value);
        
        /**
         * Write an array of bytes.
         *
         * @param bytes
         *            the array to be written.
         *
         * @return the number of bytes written.
         */
        size_t WriteBytes(const uint8_t* bytes, size_t numOfBytes);
        
        /**
         * Write a 16-bit integer.
         *
         * @param value
         *            an integer containing the value to be written.
         */
        void WriteShort(int value);
        
        /**
         * Write a 32-bit integer.
         *
         * @param value
         *            an integer containing the value to be written.
         */
        void WriteInt(int value);
        
        /**
         * Write a string using the default character set defined in the encoder.
         *
         * @param str
         *            the string.
         */
        void WriteString(const std::string str);

    private:
        bool _SetError(error_t err);
        
    private:
        /** The underlying output stream. */
        OutputStream* stream_;
        /** buffer size */
        size_t buffer_size_;
        /** pointer to buffer. */
        uint8_t* buffer_;
        /** The position of the buffer relative to the start of the stream. */
        long position_;
        /** The index in bytes to the current location in the buffer. */
        long index_;
        /** The offset in bits to the location in the current byte. */
        int offset_;
        /** Stack for storing file locations. */
        std::list<long> locations_;
        
        error_t error_;
    };
}  /* binary_coder */

#endif /* defined(BINARYCODER_ENCODER_H_) */
