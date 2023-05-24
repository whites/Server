#ifndef SOCKETUTILS_H_
#define SOCKETUTILS_H_

#include <sys/types.h>
#include <sys/epoll.h>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace SocketUtils
{
    void setSocketNonblock(int fd);
}


namespace EPollUtils
{
    int epCtl(int epfd, int opType, int fd, uint32_t event);
}

namespace LuaUtils
{
    int doFile( lua_State* L, const char* filename);

    int getTable(lua_State* L, int tableIndex, const char* key);

    template<typename T>
    void getNumber(lua_State* L, int tableIndex, const char* key, T& t);

    void getString(lua_State* L, int tableIndex, const char* key, char* dstStr, size_t strLen);
    
}

#endif /* SOCKETUTILS_H_ */
