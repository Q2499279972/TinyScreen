#include "ReadLuaConfig.hpp"
#include <string>
#include <iostream>
#include "debug.hpp"
using namespace std;

static lua_State *L;

void ReadLuaInit()
{
    L = lua_open();    
    luaL_openlibs(L);
}

int ReadLuaConfig(LuaConfig *cfg,char *filename)
{
    ReadLuaInit();
    DBGPRINT("reading configure file %s\n",filename);
    if(luaL_loadfile(L,filename) || lua_pcall(L,0,0,0)){    
        return 0;
    }
    /////////////////////////////
    lua_getglobal(L,"width"); 
    if(!lua_isnumber(L,-1))
    {
        lua_pop(L, 1);
        return 0;
    }
    else
    {    
        cfg->width = (int)lua_tonumber(L,-1);
        DBGPRINT("width is %d\n",cfg->width);
        lua_pop(L, 1);
    }
    ////////////////////////////
    lua_getglobal(L,"height"); 
    if(!lua_isnumber(L,-1))
    {
        lua_pop(L, 1);
        return 0;
    }
    else
    {    
        cfg->height = (int)lua_tonumber(L,-1);
        DBGPRINT("height is %d\n",cfg->height);
        lua_pop(L, 1);
    }
    ////////////////////////////
    lua_getglobal(L,"factor"); 
    if(!lua_isnumber(L,-1))
    {
        lua_pop(L, 1);
        return 0;
    }
    else
    {    
        cfg->factor = (float)lua_tonumber(L,-1);
        DBGPRINT("height is %f\n",cfg->factor);
        lua_pop(L, 1);
    }
    lua_close(L);
    return 1;
}
