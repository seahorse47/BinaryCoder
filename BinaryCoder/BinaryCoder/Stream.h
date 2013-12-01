#ifndef BINARYCODER_STREAM_H_
#define BINARYCODER_STREAM_H_

#include "STDHeaders.h"
#include "Constants.h"

namespace binary_coder {
    
    /**
     * Input Stream.
     */
    class InputStream
    {
    public:
        virtual ~InputStream() {}
        //virtual void Open(const char* path, bool binary) = 0;
        //virtual void Close() = 0;
        
        /**
         * Sets the position indicator
         * @param offset
         *          Number of bytes to offset from origin.
         * @param origin
         *          Position from where offset is added. It is specified by
         *      one of the following constants defined in <cstdio>:
         *          SEEK_SET	Beginning of file
         *          SEEK_CUR	Current position of the file pointer
         *          SEEK_END	End of file
         * @return a nonzero value if seek failed.
         */
        virtual int Seek(long offset, int origin) = 0;
        
        /**
         * Reads an array of count elements, each one with a size of size
         * bytes, from the stream and stores them in the block of memory
         * specified by ptr. The postion indicator of the stream is advanced
         * by the total amount of bytes read.
         * @param ptr
         *          Pointer to a block of memory with a minimum size of
         *      (size*count) bytes.
         * @param size
         *          Size in bytes of each element to be read.
         * @param count
         *          Number of elements, each one with a size of size bytes.
         * @return the total number of elements successfully read.
         */
        virtual size_t Read(void* ptr, size_t size, size_t count) = 0;
        
        /**
         * Get the current position indicator.
         * @return the current value of the position indicator.
         */
        virtual size_t Tell() const = 0;
        
        /**
         * Checks whether the End-of-File indicator associated with stream
         * is set, returning a value different from zero if it is.
         * @return non-zero if the End-of-File is set.
         */
        virtual int Eof() const = 0;
        
        /**
         * Checks if some error occurs.
         * @return the error code.
         */
        virtual error_t Error() const = 0;
    };
    
    /**
     * Output Stream.
     */
    class OutputStream
    {
    public:
        virtual ~OutputStream() {}
        /**
         * Writes an array of count elements, each one with a size of size
         * bytes, to the stream. The postion indicator of the stream is advanced
         * by the total amount of bytes read.
         * @param ptr
         *          Pointer to a block of memory with a minimum size of
         *      (size*count) bytes.
         * @param size
         *          Size in bytes of each element to be written.
         * @param count
         *          Number of elements, each one with a size of size bytes.
         * @return the total number of elements successfully written.
         */
        virtual size_t Write(const void* ptr, size_t size, size_t count) = 0;
        
        virtual void Flush() = 0;
        
        virtual void Seal() = 0;
        
        /**
         * Checks if some error occurs.
         * @return the error code.
         */
        virtual error_t Error() const = 0;
    };
    
    class InputFile: public InputStream
    {
    public:
        InputFile(const char* filename, bool binary);
        InputFile(FILE* file, bool own_file = false);
        ~InputFile();
        
        int Seek(long offset, int origin);
        size_t Read(void* ptr, size_t size, size_t count);
        size_t Tell() const;
        int Eof() const;
        error_t Error() const;
    private:
        FILE* file_;
        error_t err_;
        bool own_file_;
        
        void _SetError(error_t err) {
            if (err_ == NoError) {
                err_ = err;
            }
        }
    };
    
    class InputMemoryBlock: public InputStream
    {
    public:
        InputMemoryBlock(const uint8_t* data, size_t dataLength, bool copy);
        ~InputMemoryBlock();
        
        int Seek(long offset, int origin);
        size_t Read(void* ptr, size_t size, size_t count);
        size_t Tell() const;
        int Eof() const;
        error_t Error() const;
    private:
        union {
            const uint8_t* data_;
            uint8_t* ptr_;
        };
        
        size_t length_;
        size_t position_;
        error_t err_;
        bool own_data_;
        
        void _SetError(error_t err) {
            if (err_ == NoError) {
                err_ = err;
            }
        }
    };
    
    //////////
    class OutputFile: public OutputStream
    {
    public:
        OutputFile(const char* filename, bool binary);
        OutputFile(FILE* file, bool own_file = false);
        virtual ~OutputFile();
        
        virtual size_t Write(const void* ptr, size_t size, size_t count);
        virtual void Flush();
        virtual void Seal();
        virtual error_t Error() const { return err_; }
    private:
        FILE* file_;
        error_t err_;
        bool own_file_;
        bool is_sealed_;
        
        void _SetError(error_t err) {
            if (err_ == NoError) {
                err_ = err;
            }
        }
    };
    
} /* binary_coder */

#endif
