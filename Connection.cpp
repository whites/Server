#include "Connection.h"
#include "Buffer.h"
#include <sys/socket.h>
#include "Log.h"
#include "Define.h"

/**
 * @brief recv data, split the data stream to packet and pack data
 * 
 * @param buf 
 * @param len 
 */
void Connection::recvData(char* buf, size_t len )
{
    size_t needLen;
    PacketMeta *packMeta;
    while (len > 0)
    {
        if(read_buf_->isEmpty())
        {
            if (len < sizeof(PacketMeta))
            {
                read_buf_->append(buf, len);
                return;
            }
            packMeta = (PacketMeta *)buf;
        }
        else
        {
            packMeta = (PacketMeta *)read_buf_->getBuf();
        }

        needLen = packMeta->mDataLen - read_buf_->getReadableSize();
        if(len >= needLen)
        {
            read_buf_->append(buf, needLen);
            len -= needLen;
            buf += needLen;

            // post up or process data
            processData(read_buf_);
            
            Buffer *packBuf = new Buffer();
            read_buf_ = packBuf;
        }

        if(len < needLen)
        {
            read_buf_->append(buf, len);
            len = 0;
        }
    }
}

/**
 * @brief Todo: post up the user defined packet to the application layer
 *          here just print it, temporary
 * 
 * @param data 
 */
void Connection::processData(Buffer *data)
{
    INFO("readable: %d, recv data: %s", read_buf_->getReadableSize(), read_buf_->retrieve(sizeof(PacketMeta)).c_str());
    SAFE_DELETE(read_buf_);
}

void Connection::postData(Buffer* buf)
{
    std::lock_guard<std::mutex> lock(mutex_);
    message_queue_.push_back(buf);
}

Buffer* Connection::popData()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(message_queue_.empty()) return nullptr;

    Buffer* buf = message_queue_.front();
    message_queue_.pop_front();
    return buf;
}

Buffer* Connection::getNextToSend()
{
    if(send_buf_ != nullptr) return send_buf_;
    return popData();
}


int Connection::sendData()
{
    while ( (send_buf_ = getNextToSend()) )
    {
        size_t dataLen = send_buf_->getReadableSize();
        size_t ret = send(sock_id_, send_buf_->getBuf(), dataLen, 0);
        if (ret < 0)
        {
            if (errno != EAGAIN)
            {
                ERROR("send faild.");
                return FAILD;
            }

            return ERR_SEND_UNCOMP;
        }

        if (ret < dataLen) // not complete
        {
            send_buf_->peek(ret);
            return ERR_SEND_UNCOMP; 
        }

        SAFE_DELETE(send_buf_);
    }

    return SUCCESS;
}
