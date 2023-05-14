#ifndef NETMNG_H_
#define NETMNG_H_

#include "Connection.h"
#include <vector>
#include "Buffer.h"
#include <deque>
#include <map>
#include <thread>

class NetMng
{
    public:
        static NetMng* GetInstance()  // singleton
        {
            static NetMng netMng;
            return &netMng;
        }
    
    private:
        NetMng() = default;
        ~NetMng() = default;
        NetMng(const NetMng&);
        NetMng& operator=(const NetMng&);
    
    private:
        int port_ = 3000;
        int epoll_size_ = 1024;

        int epoll_fd_;
        bool active_ = true;
        std::map<int, Connection*> connection_map_;
        const int SELECT_THREAD_NUM = 4;
        void pollThread(int listen_sock);
        std::vector<std::thread*> select_list_;
    
    public:
        void start();
        void stop();
        void sendData(int dpid, char* data, int len);

    private:
        Connection* getConnection(int fd);
        void closeConnection(Connection* conn);
        void doAccept(int listen_fd);
        void doRecv(int fd);
        void doSend(int fd);
        
};


#endif /* NETMNG_H_ */
