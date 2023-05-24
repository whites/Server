#include "Utils.h"
#include <fcntl.h>
#include <cstring>

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

int LuaUtils::doFile(lua_State *L, const char *filename)
{
    if (luaL_dofile(L, filename))
    {
        return 1;
    }
    return 0;
}

template<typename T>
void LuaUtils::getNumber(lua_State* L, int tableIndex, const char* key, T& t)
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

void LuaUtils::getString(lua_State *L, int tableIndex, const char *key, char *dstStr, size_t strLen)
{
    lua_pushvalue(L, tableIndex);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if(!lua_isnil(L, -1))
    {
        strncpy(dstStr, lua_tostring(L, -1), strLen);
    }

    lua_pop(L, 2);
}

/**
 * @brief get table from lua table by key
 * 
 * @param L 
 * @param tableIndex 
 * @param key  string key
 * @return int 
 */
int LuaUtils::getTable(lua_State* L, int tableIndex, const char* key)
{
    lua_pushvalue(L, tableIndex);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if(lua_isnil(L, -1) || !lua_istable(L, -1) )
    {
        lua_pop(L, 2);
        return -1;
    }
    else
    {
        lua_insert(L, -2);
        lua_pop(L, 1);
        return lua_gettop(L);
    }

    return -1;
}
