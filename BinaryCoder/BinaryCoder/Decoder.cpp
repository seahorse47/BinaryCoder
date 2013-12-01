#include "Decoder.h"
#include "Stream.h"

namespace binary_coder {

Decoder::Decoder(InputStream* input)
{
    input_ = input;

    buffer_size_ = BUFFER_SIZE;
    buffer_ = new uint8_t[buffer_size_];
    buffered_bytes_ = 0;
    position_ = 0;
    index_ = 0;
    offset_ = 0;

    //error_ = NoError;
    ResetError();
}

Decoder::~Decoder()
{
    delete[] buffer_;
}

bool Decoder::_SetError(error_t err)
{
    if (error_ == NoError) {
        error_ = err;
        return true;
    } else {
        return false;
    }
}

void Decoder::_Fill()
{
    long diff = buffered_bytes_ - index_;
    if (diff < 0) {
        diff = 0;
    }
    position_ += index_;

    if ((size_t)index_ < buffered_bytes_) {
        for (int i = 0; i < diff; i++) {
            buffer_[i] = buffer_[index_++];
        }
    }

    long bytesRead = 0;
    long bytesToRead = buffer_size_ - diff;

    buffered_bytes_ = diff;
    input_->Seek(position_ + buffered_bytes_, SEEK_SET);

    while (bytesToRead > 0) {
        bytesRead = input_->Read(buffer_ + buffered_bytes_, 1, bytesToRead);
        if (bytesRead == 0) {
            bytesToRead = 0;
        } else {
            buffered_bytes_ += bytesRead;
            bytesToRead -= bytesRead;
        }
    }

    index_ = 0;
}

void Decoder::_DiscardBuffer()
{
    position_ += index_;
    index_ = 0;
    buffered_bytes_ = 0;
}

long Decoder::Mark()
{
    long current = position_ + index_;
    locations_.push_back(current);
    return current;
}

void Decoder::Unmark()
{
    locations_.pop_back();
}

void Decoder::Reset()
{
    long last;
    if (locations_.empty()) {
        last = 0;
    } else {
        last = locations_.back();
    }

    if (last < position_) {
        _DiscardBuffer();
        index_ = last & 0x01ff;
        position_ = last - index_;
    } else {
        index_ = last - position_;
    }
}

long Decoder::Check(long expected)
{
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

void Decoder::Skip(size_t count)
{
    if (count == 0) {
        return;
    }

    if (index_ + count < buffered_bytes_) {
        index_ += count;
    } else {
        long pos = position_ + index_ + count;
        _DiscardBuffer();
        index_ = pos & 0x01ff;
        position_ = pos - index_;
    }
}

void Decoder::SkipBits(size_t bitsCount)
{
    long current = position_ + index_;
    long pointer = (current << BYTES_TO_BITS) + offset_;
    pointer += bitsCount;
    long newPos = pointer >> BITS_TO_BYTES;
    Skip(newPos - current);
    offset_ = pointer & LOWEST3;
}

void Decoder::AlignToByte() {
    if (offset_ > 0) {
        index_ += 1;
        offset_ = 0;
    }
}

long Decoder::BytesRead() const {
    return (position_ + index_) - locations_.back();
}

uint32_t Decoder::_ScanBits(int numberOfBits, bool updatePointer)
{
    if (error_ != NoError) {
        return 0;
    }
    
    long pointer = (index_ << BYTES_TO_BITS) + offset_;

    if (pointer + numberOfBits > (buffered_bytes_ << BYTES_TO_BITS)) {
        _Fill();
        pointer = (index_ << BYTES_TO_BITS) + offset_;
    }

    uint32_t value = 0;

    if (numberOfBits > 0) {
        if (pointer + numberOfBits > (buffered_bytes_ << BYTES_TO_BITS)) {
            _SetError(ArrayIndexOutOfBounds);
            return 0;
        }

        // The bit order within bytes in the SWF file format is big-endian: 
        // the most significant bit is stored first, and the least 
        // significant bit is stored last.
        for (int i = BITS_PER_INT; 
            (i > 0)  && (index_ < buffer_size_); 
            i -= BITS_PER_BYTE) {
            value |= (buffer_[index_++] & BYTE_MASK) << (i - BITS_PER_BYTE);
        }

        value <<= offset_;

        if (updatePointer) {
            pointer += numberOfBits;
        }
        
        index_ = pointer >> BITS_TO_BYTES;
        offset_ = pointer & LOWEST3;
    }

    return value;
}

int32_t Decoder::ReadSignedBits(int numberOfBits) {
    int32_t value = (int32_t)_ScanBits(numberOfBits, true);
    value >>= (BITS_PER_INT - numberOfBits);
    return value;
}

uint32_t Decoder::ReadUnsignedBits(int numberOfBits) {
    uint32_t value = _ScanBits(numberOfBits, true);
    value >>= (BITS_PER_INT - numberOfBits);
    return value;
}

int32_t Decoder::ScanSignedBits( int numberOfBits )
{
    int32_t value = (int32_t)_ScanBits(numberOfBits, false);
    value >>= (BITS_PER_INT - numberOfBits);
    return value;
}

uint32_t Decoder::ScanUnsignedBits(int numberOfBits) {
    uint32_t value = _ScanBits(numberOfBits, false);
    value >>= (BITS_PER_INT - numberOfBits);
    return value;
}

uint8_t Decoder::_ScanByte(bool updatePointer) {
    if (error_ != NoError) {
        return 0;
    }
    
    if (index_ + 1 > buffered_bytes_) {
        _Fill();
    }

    if (index_ + 1 > buffered_bytes_) {
        _SetError(ArrayIndexOutOfBounds);
        return 0;
    }

    uint8_t value = buffer_[index_] & BYTE_MASK;
    if (updatePointer) {
        index_++;
    }
    return value;
}

uint16_t Decoder::_ScanShort(bool updatePointer) {
    if (error_ != NoError) {
        return 0;
    }
    
    if (index_ + 2 > buffered_bytes_) {
        _Fill();
    }

    if (index_ + 2 > buffered_bytes_) {
        _SetError(ArrayIndexOutOfBounds);
        return 0;
    }

    uint16_t value = buffer_[index_] & BYTE_MASK;
    value |= (buffer_[index_ + 1] & BYTE_MASK) << TO_BYTE1;
    if (updatePointer) {
        index_ += 2;
    }
    return value;
}

uint32_t Decoder::_ScanInt(bool updatePointer) {
    if (error_ != NoError) {
        return 0;
    }
    
    if (index_ + 4 > buffered_bytes_) {
        _Fill();
    }

    if (index_ + 4 > buffered_bytes_) {
        _SetError(ArrayIndexOutOfBounds);
        return 0;
    }

    uint32_t value = buffer_[index_] & BYTE_MASK;
    value |= (buffer_[index_ + 1] & BYTE_MASK) << TO_BYTE1;
    value |= (buffer_[index_ + 2] & BYTE_MASK) << TO_BYTE2;
    value |= (buffer_[index_ + 3] & BYTE_MASK) << TO_BYTE3;
    if (updatePointer) {
        index_ += 4;
    }
    return value;
}

uint8_t Decoder::ScanUnsignedByte()
{
    return _ScanByte(false);
}

uint8_t Decoder::ReadUnsignedByte()
{
    return _ScanByte(true);
}

int8_t Decoder::ScanSignedByte()
{
    return (int8_t)_ScanByte(false);
}

int8_t Decoder::ReadSignedByte()
{
    return (int8_t)_ScanByte(true);
}

uint16_t Decoder::ScanUnsignedShort()
{
    return _ScanShort(false);
}

uint16_t Decoder::ReadUnsignedShort()
{
    return _ScanShort(true);
}

int16_t Decoder::ScanSignedShort()
{
    return (int16_t)_ScanShort(false);
}

int16_t Decoder::ReadSignedShort()
{
    return (int16_t)_ScanShort(true);
}

uint32_t Decoder::ScanUnsignedInt()
{
    return _ScanInt(false);
}

uint32_t Decoder::ReadUnsignedInt()
{
    return _ScanInt(true);
}

int32_t Decoder::ScanSignedInt()
{
    return (int32_t)_ScanInt(false);
}

int32_t Decoder::ReadSignedInt()
{
    return (int32_t)_ScanInt(true);
}

size_t Decoder::ReadBytes(uint8_t* bytes, size_t wanted) {
    long dest = 0;
    long read = 0;
    long available;
    long remaining;
    
    if (error_ != NoError) {
        return 0;
    }

    while (read < wanted) {
        if (index_ >= buffered_bytes_) {
            _Fill();
        }
        available = buffered_bytes_ - index_;
        if (available <= 0) {
            _SetError(ReachedEndOfFile);
            break;
        }
        remaining = wanted - read;
        if (available > remaining) {
            available = remaining;
        }
        memcpy(bytes + dest, buffer_ + index_, available);
        read += available;
        index_ += available;
        dest += available;
    }

    return read;
}

string& Decoder::ReadString(string& out, int length) {
    char* bytes = new char[length];
    size_t read = ReadBytes((uint8_t*)bytes, length);
    if (bytes[read - 1] == 0) {
        read = read - 1;
    }
    out.append(bytes, read);
    delete[] bytes;
    return out;
}

string& Decoder::ReadString(string& out)
{
    if (error_ != NoError) {
        return out;
    }
    
    long start = index_;
    //long length = 0;
    long available;
    bool finished = false;
    long count;

    while (!finished) {
        available = buffered_bytes_ - index_;
        if (available == 0) {
            _Fill();
            available = buffered_bytes_ - index_;
        }
        start = index_;
        count = 0;
        for (int i = 0; i < available; i++) {
            if (buffer_[index_++] == 0) {
                finished = true;
                break;
            } else {
                count++;
            }
        }
        out.append((const char*)(buffer_ + start), count);
        //length += count;
    }

    return out;
}

} /* binary_coder */
