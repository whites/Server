#include "NetMng.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/epoll.h>
#include <error.h>
#include <unistd.h>
#include "Buffer.h"
#include "Log.h"
#include "Utils.h"
#include "Define.h"

bool NetModule::moduleInit()
{
    APP_GET_TABLE("Network");
    APP_GET_NUMBER("Port", port_);
    APP_END_TABLE();

    INFO("port: [%d]", port_);
    GetInstance()->start();
    return true;
}


bool NetModule::moduleDestroy()
{
    GetInstance()->stop();
    return true;
}

/**
 * @brief polling thread function, implemented with epoll
 * 
 * @param listen_fd
 */
void NetModule::pollThread(int listen_fd)
{
    struct epoll_event evlist[100];

    int nfds, client_fd;
    while (active_)
    {
        if( (nfds = epoll_wait(epoll_fd_, evlist, 100, 100)) < 0 )
        {
            ERROR("epoll_wait faild");
            exit(0);
        }

        for(int i=0; i<nfds; i++)
        {
            if(evlist[i].data.fd == listen_fd) // new connect
            {
                doAccept(listen_fd);
            }
            else if(evlist[i].events & EPOLLIN) //recv data
            {
                client_fd = evlist[i].data.fd;
                doRecv(client_fd);
            }
            else if(evlist[i].events & EPOLLOUT) 
            {
                client_fd = evlist[i].data.fd;
                doSend(client_fd);
            }
        }
    }
}

void NetModule::doAccept(int listen_fd)
{
    sockaddr_in in_addr;
    socklen_t size = sizeof(in_addr);
    int client_fd;
    while (true) // control the numer of new connections per loop to prevent starvation
    {
        client_fd = accept(listen_fd, (sockaddr *)&in_addr, &size);
        if (client_fd < 0)
        {
            if (errno == EINTR) // interrupt, retry
                continue; 

            if (errno == EAGAIN) // no more new connections
                return; 

            ERROR("accept faild");
            return;
        }

        SocketUtils::setSocketNonblock(client_fd);
        Connection *conn = new Connection(client_fd);
        connection_map_.insert({client_fd, conn});

        if (EPollUtils::epCtl(epoll_fd_, EPOLL_CTL_ADD, client_fd, EPOLLIN | EPOLLET) < 0) // edge trigger
        {
            ERROR("EPOLL_CTL_ADD faild.");
            return;
        }

        INFO("new connection: %d", client_fd);
    }
}

void NetModule::doRecv(int client_fd)
{
    Connection *conn = getConnection(client_fd);
    if ( nullptr == conn )
    {
        ERROR("can not find the connection by fd: %d", client_fd);
        return;
    }

    int buflen = 1024 ;
    char buf[buflen];

    while (true)
    {
        int ret = recv(client_fd, buf, buflen, 0);
        if (ret < 0)
        {
            if (errno == EAGAIN) // recv over
                return;

            ERROR("Recv data faild, fd: %d", client_fd);
            closeConnection(conn);
            return;
        }
        
        if (ret == 0)
        {
            INFO("connection closed: %d", client_fd);
            closeConnection(conn);
            return;
        }

        conn->recvData(buf, ret);
    }
}

void NetModule::doSend(int client_fd)
{
    Connection *conn = getConnection(client_fd);
    if ( nullptr == conn )
    {
        ERROR("can not find the connection by fd: %d", client_fd);
        return;
    }

    int ret = conn->sendData();
    if (ret == FAILD)
    {
        closeConnection(conn);
        return;
    }

    uint32_t event = EPOLLIN | EPOLLET;
    if (ret == ERR_SEND_UNCOMP) // sending uncomplete
    {
        event |= EPOLLOUT;
    }

    if (EPollUtils::epCtl(epoll_fd_, EPOLL_CTL_MOD, client_fd, event) < 0)
    {
        ERROR("modify epoll event faild, dpid: %d", conn->getDpid());
    }
}


void NetModule::closeConnection(Connection *conn)
{
    struct epoll_event ev;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, conn->getSockId() , &ev) < 0)
    {
        ERROR("EPOLL_CTL_DEL error");
    }
    close(conn->getSockId());
    SAFE_DELETE(conn);
}

/**
 * @brief interface to send data with specific connection
 *   1 add data to a buffer list waiting to be sent
 *   2 modify epoll event to trigger epollout(write ready) event
 *   3 trigger epollout event and start sending 
 * 
 * @param dpid unique identifier for each connection
 * @param data data to send
 * @param len length of the data
 */
void NetModule::sendData(int dpid, char* data, int len)
{
    Connection* conn = getConnection(dpid);
    if( nullptr == conn )
    {
        ERROR("can not find the connection by dpid: %d", dpid);
        return;
    }

    Buffer* buf = new Buffer();
    buf->append(data, len);

    conn->postData(buf);

    if (EPollUtils::epCtl(epoll_fd_, EPOLL_CTL_MOD, conn->getSockId(), EPOLLOUT | EPOLLIN | EPOLLET) < 0)
    {
        ERROR("modify epoll event faild, dpid: %d", conn->getDpid());
    }
}

/**
 * @brief set up the environment and start the network module
 * 
 */
void NetModule::start()
{
    sockaddr_in svr_addr;

    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(port_);
    svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listen_fd;

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        ERROR("create socket faild!");
        exit(0);
    }

    SocketUtils::setSocketNonblock(listen_fd);

    int opt = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0 )
    {
        ERROR("setsockopt faild!");
        exit(0);
    }

    if (bind(listen_fd, (sockaddr *)&svr_addr, sizeof(svr_addr)) < 0)
    {
        ERROR("bind socket faild!");
        exit(0);
    }

    if(listen(listen_fd, 20))
    {
        ERROR("listen faild!");
        exit(0);
    }

    if( ( epoll_fd_ = epoll_create(epoll_size_) ) < 0 )
    {
        ERROR("epoll_create faild!");
        exit(0);
    }

    if (EPollUtils::epCtl(epoll_fd_, EPOLL_CTL_ADD, listen_fd, EPOLLIN | EPOLLET) < 0) // edge trigger
    {
        ERROR("add listen fd to epoll faild.");
        exit(0);
    }

    for(int i=0; i<4; i++)
    {
        std::thread* t = new std::thread(&NetModule::pollThread, this, listen_fd);
        select_list_.push_back(t);
    }

    INFO("network started...");
}

/**
 * @brief stop the network module and clear up
 * 
 */
void NetModule::stop()
{
    active_ = false;

    for(auto t: select_list_)
    {
        if(t) t->join();
        SAFE_DELETE(t);
    }

    INFO("network stopped...");
}


Connection* NetModule::getConnection(int fd)
{
    auto res = connection_map_.find(fd);
    if(res != connection_map_.end())
    {
        return res->second;
    }

    return nullptr;
}
