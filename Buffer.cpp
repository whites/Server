#include "Buffer.h"


void Buffer::append(const char* data, size_t len)
{
    ensureSpace(len);
    std::copy(data, data+len, buffer_.begin() + writeIndex_);
    writeIndex_ += len;
}

char* Buffer::getBuf()
{
    return &*(buffer_.begin() + readIndex_);
}

void Buffer::peek(int len)
{
    readIndex_ += len;
}

bool Buffer::ensureSpace(size_t len)
{
    if(!isEnoughSpace(len))
    {
        makeSpace(len);
    }
    return true;
}

bool Buffer::isEnoughSpace(size_t len)
{
    return buffer_.size() - writeIndex_ > len;
}

void Buffer::makeSpace(size_t len)
{
    if( getWritableSize() < len )
    {
        buffer_.resize(writeIndex_ + len);
    }
    else
    {
        auto first = buffer_.begin() + readIndex_;
        auto end = buffer_.begin() + writeIndex_;

        size_t readSize = getReadableSize();

        std::copy(first, end, buffer_.begin());

        readIndex_ = 0;
        writeIndex_ = readIndex_ + readSize;
    }
}

std::string Buffer::retrieve(int ignoreLen = 0)
{
    std::string res(&*(buffer_.begin() + readIndex_ + ignoreLen), getReadableSize() - ignoreLen);
    return res;
}
