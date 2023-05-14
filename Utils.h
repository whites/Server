#ifndef SOCKETUTILS_H_
#define SOCKETUTILS_H_

#include <sys/types.h>
#include <sys/epoll.h>

namespace SocketUtils
{
    void setSocketNonblock(int fd);
}


namespace EPollUtils
{
    int epCtl(int epfd, int opType, int fd, uint32_t event);
}

#endif /* SOCKETUTILS_H_ */
