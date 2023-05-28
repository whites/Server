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
    int doFile(lua_State *L, const char *filename);

    int getTable(lua_State *L, int tableIndex, const char *key);

    template <typename T>
    inline void getNumber(lua_State *L, int tableIndex, const char *key, T &t);

    void getString(lua_State *L, int tableIndex, const char *key, char *dstStr, size_t strLen);
    
}

template<typename T>
inline void LuaUtils::getNumber(lua_State* L, int tableIndex, const char* key, T& t)
{
    lua_pushvalue(L, tableIndex);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if(!lua_isnil(L, -1))
    {
        t = (T)lua_tonumber(L, -1);
    }

    lua_pop(L, 2);
}

#endif /* SOCKETUTILS_H_ */
