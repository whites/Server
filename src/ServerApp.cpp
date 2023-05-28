#include "ServerApp.h"
#include "Log.h"
#include "Utils.h"
#include <string.h>
#include <unistd.h>
#include <sys/resource.h> 

volatile bool ServerApp::active_ = true;

bool ServerApp::init( int argc, char** argv)
{
    memset(file_name_, 0 , MAX_FILE_NAME_LEN);

    char* strvar = nullptr;
    int opt;

    while ((opt = getopt(argc, argv, "c:")) != EOF)
    {
        switch (opt)
        {
        case 'c':
            strvar = optarg;
            snprintf(file_name_, MAX_FILE_NAME_LEN, "%s", strvar );
            break;
        
        default:
            printf("Usage: -c filename\n");
            break;
        }
    }

    struct rlimit rlim_core;

	rlim_core.rlim_cur = RLIM_INFINITY;	// Soft limit
	rlim_core.rlim_max = RLIM_INFINITY;	// Hard limit (ceiling for rlim_cur)
	if( setrlimit(RLIMIT_CORE, &rlim_core) < 0)
    {
        ERROR("Set corefile size failed, error:%s", strerror(errno));
        return false;
    }

    L_  = luaL_newstate(); 
    luaL_openlibs(L_);

    if( luaL_dofile(L_, file_name_) )
    {
        ERROR("Load config file failed, file:[%s]", file_name_);
        lua_close(L_);
        return false;
    }

    lua_getglobal(L_, "AppConfig");
    if(lua_isnil(L_, -1))
    {
        ERROR("Read config table failed, file:[%s]", file_name_);
        lua_close(L_);
        return false;
    }

    return true;
}

bool ServerApp::registerModule(Module* module)
{
    module_list_.push_back(module);
    module->server_app_ = this;

    return true;
}

bool ServerApp::start()
{
    INFO("ServerApp started.");

    for(auto m: module_list_)
    {
        m->moduleInit();
    }

    mainLoop();

    for(auto m: module_list_)
    {
        m->moduleDestroy();
    }

    INFO("ServerApp stopped.");
    return true;
}

/**
 * @brief main loop of the server
 * 
 */
void ServerApp::mainLoop()
{
    while (active_)
    {
        /* idle */
    }
}