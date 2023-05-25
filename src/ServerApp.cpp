#include "ServerApp.h"
#include "Log.h"

volatile bool ServerApp::active_ = true;

bool ServerApp::init( int argc, char** argv)
{
    L_  = luaL_newstate(); 
    luaL_openlibs(L_);

    //todo read config

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