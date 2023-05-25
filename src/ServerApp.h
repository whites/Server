#ifndef MODULE_H_
#define MODULE_H_

#include <vector>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


class ServerApp;
class Module
{
public:
    virtual bool moduleInit() { return true; };
    virtual bool moduleDestroy() { return true; };

    friend class ServerApp;

protected:
    ServerApp* server_app_;

};


class ServerApp
{
public:
    bool init( int argc, char** argv);
    bool registerModule(Module* module);

    bool start();

    lua_State* L_;
    static volatile bool active_;

private:
    std::vector<Module*> module_list_;
    void mainLoop();
    
};

#endif /* MODULE_H_ */