#ifndef BINARYCODER_CONSTANTS_H_
#define BINARYCODER_CONSTANTS_H_

#include "STDHeaders.h"

namespace binary_coder {
    /** The default size, in bytes, for the internal buffer. */
#define BUFFER_SIZE 4096
    /** The default size, in bytes, for the reading strings. */
#define STR_BUFFER_SIZE 1024
    /** Bit mask applied to bytes when converting to unsigned integers. */
#define BYTE_MASK 255
    /** Number of bits to shift when aligning a value to the second byte. */
#define TO_BYTE1 8
    /** Number of bits to shift when aligning a value to the third byte. */
#define TO_BYTE2 16
    /** Number of bits to shift when aligning a value to the fourth byte. */
#define TO_BYTE3 24
    /** Number of bits in an int. */
#define BITS_PER_INT 32
    /** Number of bits in a byte. */
#define BITS_PER_BYTE 8
    /** Right shift to convert number of bits to number of bytes. */
#define BITS_TO_BYTES 3
    /** Left shift to convert number of bytes to number of bits. */
#define BYTES_TO_BITS 3
    /** Offset to add to number of bits when calculating number of bytes. */
#define ROUND_TO_BYTES 7
    
#define LOWEST3 7
    
#define BIT1 0x01
#define BIT2 0x02
#define BIT3 0x04
#define BIT4 0x08
#define BIT5 0x10
#define BIT6 0x20
#define BIT7 0x40
#define BIT8 0x80
    
    
    enum error_t {
        NoError = 0,

        /// common errors
        BadArguments,

        /// error code for Decoder
        ArrayIndexOutOfBounds,
        ReachedEndOfFile,

//        /// error code for decoding movie
//        BadDataFormat,
//        IOError,
//        UnknownTag,

        /// IO errors.
        InvalidFile,
        FailedToOpen,
        FailedToRead,
        FailedToWrite,
        StreamIsClosed,
        InvalidData,
    };
} /* binary_coder */

#endif
