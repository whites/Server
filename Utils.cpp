#include "Utils.h"
#include <fcntl.h>

void SocketUtils::setSocketNonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}



int EPollUtils::epCtl(int epfd, int opType, int fd, uint32_t event)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;
    return epoll_ctl(epfd, opType, fd, &ev);
}
