#include "NetMng.h"
#include <signal.h>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

static bool running = true;

static void sig_int(int num)
{
    NetMng::GetInstance()->stop();
    running = false;
}

int main()
{
    lua_State * ls = luaL_newstate(); 
    luaL_openlibs(ls);

    NetMng::GetInstance()->start();

    signal(SIGINT, sig_int);

    while(running)
    {
        //idle
    }
    return 0;
}