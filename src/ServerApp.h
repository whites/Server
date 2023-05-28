#ifndef MODULE_H_
#define MODULE_H_

#include <vector>
#include "Utils.h"

const size_t MAX_FILE_NAME_LEN = 128;
const size_t MAX_CONFIG_STR_LEN = 128;

#define APP_GET_TABLE(table) \
    if( LuaUtils::getTable( server_app_->L_, -1, table ) == 2 ){ 

#define APP_END_TABLE() \
        lua_pop( server_app_->L_, 1 ); \
    }

#define APP_GET_STRING(name,value) LuaUtils::getString( server_app_->L_, -1, name, value, MAX_CONFIG_STR_LEN )
#define APP_GET_NUMBER(name,value) LuaUtils::getNumber( server_app_->L_, -1, name, value )

#define APP_WHILE_TABLE() lua_pushnil(server_app_->L_); while( lua_next( server_app_->L_, -2) ) {
#define APP_END_WHILE() lua_pop(server_app_->L_, 1); }

#define APP_TABLE_LEN ((int)lua_objlen( server_app_->L_, -1 ))


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
    char file_name_[MAX_FILE_NAME_LEN];
    void mainLoop();
    
};

#endif /* MODULE_H_ */