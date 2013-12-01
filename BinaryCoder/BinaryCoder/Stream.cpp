#include "Stream.h"

namespace binary_coder {
    InputFile::InputFile(const char* filename, bool binary)
    {
        if (binary) {
            file_ = fopen(filename, "rb");
        } else {
            file_ = fopen(filename, "rt");
        }
        if (file_ == NULL) {
            _SetError(FailedToOpen);
        }
        own_file_ = true;
        err_ = NoError;
    }
    
    InputFile::InputFile(FILE* file, bool own_file/* = false*/)
    {
        file_ = file;
        if (file_ == NULL) {
            _SetError(InvalidFile);
        }
        own_file_ = own_file;
        err_ = NoError;
    }
    
    InputFile::~InputFile()
    {
        if (own_file_ && file_ != NULL) {
            fclose(file_);
        }
    }
    
    int InputFile::Seek(long offset, int origin)
    {
        if (err_ != NoError) {
            return -1;
        }
        return fseek(file_, offset, origin);
    }
    
    size_t InputFile::Read(void* ptr, size_t size, size_t count)
    {
        if (err_ != NoError) {
            return 0;
        }
        size_t read = fread(ptr, size, count, file_);
        if (read < count) {
            if (ferror(file_) != 0) {
                _SetError(FailedToRead);
            }
        }
        return size*read;
    }
    
    size_t InputFile::Tell() const
    {
        if (err_ != NoError) {
            return -1;
        } else {
            return ftell(file_);
        }
    }
    
    int InputFile::Eof() const
    {
        if (err_ != NoError) {
            return 0;
        } else {
            return feof(file_);
        }
    }
    
    error_t InputFile::Error() const
    {
        return err_;
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    InputMemoryBlock::InputMemoryBlock(const uint8_t* data, size_t dataLength, bool copy)
    {
        if (copy) {
            ptr_ = new uint8_t[dataLength];
            memcpy(ptr_, data, dataLength);
            length_ = dataLength;
            own_data_ = true;
        } else {
            data_ = data;
            length_ = dataLength;
            own_data_ = false;
        }
        
        position_ = 0;
        err_ = NoError;
    }
    
    InputMemoryBlock::~InputMemoryBlock()
    {
        if (own_data_ && ptr_ != NULL) {
            delete[] ptr_;
            ptr_ = NULL;
        }
    }
    
    int InputMemoryBlock::Seek(long offset, int origin)
    {
        size_t begin = 0;
        if (origin == SEEK_CUR) {
            begin = position_;
        } else if (origin == SEEK_END) {
            begin = length_;
        }
        position_ = begin + offset;
        return 0;
    }
    
    size_t InputMemoryBlock::Read(void* ptr, size_t size, size_t count)
    {
        if (data_ == NULL || position_ > length_) {
            _SetError(FailedToRead);
            return 0;
        }
        
        size_t left = length_ - position_;
        size_t toRead = size*count;
        size_t read = toRead > left? left: toRead;
        if (read > 0) {
            memcpy(ptr, data_+position_, read);
            position_ += read;
        }
        return read;
    }
    
    size_t InputMemoryBlock::Tell() const
    {
        return position_;
    }
    
    int InputMemoryBlock::Eof() const
    {
        if (position_ >= length_) {
            return 1;
        } else {
            return 0;
        }
    }
    
    error_t InputMemoryBlock::Error() const
    {
        return err_;
    }
    
    //////////////////////////////////////////////////////////////////////////
    
    OutputFile::OutputFile(const char* filename, bool binary)
    {
        if (binary) {
            file_ = fopen(filename, "wb");
        } else {
            file_ = fopen(filename, "wt");
        }
        if (file_ == NULL) {
            _SetError(FailedToOpen);
        }
        own_file_ = true;
        is_sealed_ = false;
        err_ = NoError;
    }
    
    OutputFile::OutputFile(FILE* file, bool own_file/* = false*/)
    {
        file_ = file;
        if (file_ == NULL) {
            _SetError(InvalidFile);
        }
        own_file_ = own_file;
        is_sealed_ = false;
        err_ = NoError;
    }
    
    OutputFile::~OutputFile()
    {
        if (!is_sealed_) {
            Seal();
        }
        if (own_file_ && file_ != NULL) {
            fclose(file_);
        }
    }
    
    size_t OutputFile::Write(const void* ptr, size_t size, size_t count)
    {
        if (err_ != NoError) {
            return 0;
        }
        if (is_sealed_) {
            _SetError(StreamIsClosed);
            return 0;
        }
        size_t written = fwrite(ptr, size, count, file_);
        if (written < count) {
            if (ferror(file_) != 0) {
                _SetError(FailedToWrite);
            }
        }
        return size*written;
    }
    
    void OutputFile::Flush()
    {
        if (err_ != NoError) {
            return;
        }
        fflush(file_);
    }
    
    void OutputFile::Seal()
    {
        is_sealed_ = true;
    }
} /* binary_coder */
