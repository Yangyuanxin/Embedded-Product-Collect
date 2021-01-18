//本模块还未实现

#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

void lua_tim_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    //lua_register(g_Lua, "udp_print", lua_udp_print);    
}




/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
