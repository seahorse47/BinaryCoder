#ifndef BINARYCODER_DECODER_H_
#define BINARYCODER_DECODER_H_

#include "STDHeaders.h"
#include "Constants.h"

namespace binary_coder {

class InputStream;

class Decoder
{
public:
    Decoder(InputStream* input);
    ~Decoder();

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
     * Reposition the decoder to the point recorded by the last call to the Mark() method.
     */
    void Reset();

    /**
     * Compare the number of bytes read since the last saved position (real
     * length) and the expected length.
     * @param expected the expected number of bytes read.
     * @return the differece between real and expected length.
     */
    long Check(long expected);

    /**
     * Skips over and discards n bytes of data.
     * @param bytesCount the number of bytes to skip.
     */
    void Skip(size_t bytesCount);

    /**
     * Skips over and discards n bits of data.
     * @param bitsCount the number of bits to skip.
     */
    void SkipBits(size_t bitsCount);

    /**
     * Changes the location to the next byte boundary.
     */
    void AlignToByte();

    /**
     * Get the number of bytes read from the last saved position.
     * @return the number of bytes read since the Mark() method was last called.
     */
    long BytesRead() const;

    /**
     * Read a bit field and return the signed value.
     * @param numberOfBits
     *            the number of bits to read.
     * @return the value read.
     */
    int32_t ReadSignedBits(int numberOfBits);

    /**
     * Read a bit field and return the unsigned value.
     * @param numberOfBits
     *            the number of bits to read.
     * @return the value read.
     */
    uint32_t ReadUnsignedBits(int numberOfBits);

    /**
     * Read-ahead a bit field and return the signed value.
     * @param numberOfBits
     *            the number of bits to read.
     * @return the value read.
     */
    int32_t ScanSignedBits(int numberOfBits);

    /**
     * Read-ahead a bit field and return the unsigned value.
     * @param numberOfBits
     *            the number of bits to read.
     * @return the value read.
     */
    uint32_t ScanUnsignedBits(int numberOfBits);

    uint8_t ScanUnsignedByte();
    uint8_t ReadUnsignedByte();
    int8_t ScanSignedByte();
    int8_t ReadSignedByte();

    uint16_t ScanUnsignedShort();
    uint16_t ReadUnsignedShort();
    int16_t ScanSignedShort();
    int16_t ReadSignedShort();

    uint32_t ScanUnsignedInt();
    uint32_t ReadUnsignedInt();
    int32_t ScanSignedInt();
    int32_t ReadSignedInt();

    /**
     * Reads an array of bytes.
     * @param bytes
     *            the array that will contain the bytes read.
     * @param wanted
     *            wanted length of bytes to read.
     * @return the length of bytes read.
     */
    size_t ReadBytes(uint8_t* bytes, size_t wanted);

    /**
     * Read a string.
     * @param out
     *            The output string. The contents read from the decoder will
     *        be appended to the output string.
     * @param length
     *            the number of bytes to read.
     * @return the output string.
     */
    string& ReadString(string& out, int length);

    /**
     * Read a null-terminated string.
     * @param out
     *            The output string. The contents read from the decoder will
     *        be appended to the output string.
     * @return the output string.
     */
    string& ReadString(string& out);

private:
    bool _SetError(error_t err);
    void _DiscardBuffer();
    void _Fill();
    /**
     * Read a bit field. The first bit is stored in the the most significant
     * bit of uint32_t. For example, if you read 10 bits of value 1011011111, 
     * the return value is 10110111 11xxxxxx xxxxxxxx xxxxxxxx, where x is 
     * undefined.
     * @param numberOfBits
     *            the number of bits to read.
     * @param updatePointer
     *            indicates whether to advance the internal pointer.
     * @return the bits read.
     */
    uint32_t _ScanBits(int numberOfBits, bool updatePointer);
    
    uint8_t _ScanByte(bool updatePointer);
    uint16_t _ScanShort(bool updatePointer);
    uint32_t _ScanInt(bool updatePointer);
private:
    InputStream* input_;
    /** buffer size */
    size_t buffer_size_;
    /** pointer to buffer. */
    uint8_t* buffer_;
    /** The position of the buffer relative to the start of the file. */
    long position_;
    /** The position from the start of the buffer. */
    long index_;
    /** The number of bytes available in the current buffer. */
    size_t buffered_bytes_;
    /** The offset in bits in the current buffer location. */
    int offset_;
    /** Stack for storing file locations. */
    std::list<long> locations_;

    error_t error_;
};

} /* binary_coder */

#endif
