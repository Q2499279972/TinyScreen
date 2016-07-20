#ifndef _READLUACONFIG_HPP_
#define _READLUACONFIG_HPP_
extern "C"     
{    
    #include "lua.h"    
    #include "lauxlib.h"    
    #include "lualib.h"    
}
#include <string>
using namespace std;

typedef struct _LuaConfig
{
    int width,height;
    float factor;
}LuaConfig;

void ReadLuaInit();
int ReadLuaConfig(LuaConfig *cfg,char *filename);

#endif  
