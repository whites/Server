#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <atomic>
#include "Buffer.h"
#include <deque>
#include <mutex>
#include "Define.h"

class Connection
{
private:
    Connection() = delete;
    int sock_id_;
    Buffer* read_buf_;
    Buffer* send_buf_;

    std::atomic<int> auto_serial_{0};
    uint32_t dpid_;
    std::deque<Buffer*> message_queue_;
    std::mutex mutex_;

public:
    Connection(int sockId):sock_id_(sockId)
    {
        dpid_ = (auto_serial_++ << 16) | (sock_id_ & 0x0000FFFF);  // generate unique identifier for each connection
        read_buf_ = new Buffer();
        send_buf_ = new Buffer();
    }

    ~Connection()
    {
        SAFE_DELETE(read_buf_);
        SAFE_DELETE(send_buf_);
    }

    uint32_t getDpid() { return dpid_; }
    int getSockId() { return sock_id_; }

    void recvData(char* buf, size_t len );
    void processData(Buffer* buf);

    void postData(Buffer* buf);
    int sendData();

private:
    Buffer* popData();
    Buffer* getNextToSend();
    
};



#endif /* CONNECTION_H_ */
