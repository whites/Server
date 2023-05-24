#ifndef BUFFER_H_
#define BUFFER_H_


#include <vector>
#include <stddef.h>
#include <algorithm>
#include <string>

class Buffer
{
public:
    Buffer() : buffer_(kInitialSize), readIndex_(0), writeIndex_(0){}


    const char* peek() const{ return &buffer_[0] + readIndex_; }
    size_t getReadableSize() const{ return writeIndex_ - readIndex_; }
    size_t getWritableSize() const{ return readIndex_ + buffer_.size() - writeIndex_; }

    void append(const char* data, size_t len);
    char* getBuf();
    void peek(int len);

    bool isEmpty() const{ return getReadableSize() == 0; };

    std::string retrieve(int ignoreLen);

private:
    bool ensureSpace(size_t len);
    bool isEnoughSpace(size_t len);
    void makeSpace(size_t len);


private:
    static const size_t kInitialSize = 1024;
    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};


#endif // BUFFER_H_
