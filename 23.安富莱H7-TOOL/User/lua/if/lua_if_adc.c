#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"

/* 为了避免和DAP驱动中的函数混淆，本模块函数名前缀用 h7swd */

static int lua_ReadAdc(lua_State* L);
static int lua_ReadAnalog(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_adc_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_adc_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "read_adc", lua_ReadAdc);    
    lua_register(g_Lua, "read_analog", lua_ReadAnalog);
}
            
/*
*********************************************************************************************************
*    函 数 名: lua_ReadAdc
*    功能说明: 读ADC值
*    形    参: 通道号 0-8
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_ReadAdc(lua_State* L)
{
    uint8_t ch;
    float re;

    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 */
    {
        ch = luaL_checknumber(L, 1);    /* 模拟通道 */
    }

    if (ch == AN_CH1)                   re = g_tVar.ADC_CH1Volt;        /* CH1电压 */
    else if (ch == AN_CH2)              re = g_tVar.ADC_CH2Volt;        /* CH2电压 */
    else if (ch == AN_HIGH_SIDE_VOLT)   re = g_tVar.ADC_HighSideVolt;
    else if (ch == AN_HIGH_SIDE_CURR)   re = g_tVar.ADC_HighSideCurr;
    else if (ch == AN_TVCC_VOLT)        re = g_tVar.ADC_TVCCVolt;       /* TVCC实测电压 */
    else if (ch == AN_TVCC_CURR)        re = g_tVar.ADC_TVCCCurr;       /* TVCC实测电压 */
    else if (ch == AN_NTC_RES)          re = g_tVar.ADC_NTCRes;         /* NTC电阻 */
    else if (ch == AN_12V_VOLT)         re = g_tVar.ADC_ExtPowerVolt;
    else if (ch == AN_USB_VOLT)         re = g_tVar.ADC_USBPowerVolt;    
    else re = 0;
    
    lua_pushnumber(L, re);    /* 成功,返回数据 */
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadAdc
*    功能说明: 读校准后的模拟量
*    形    参: 通道号 0-8
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_ReadAnalog(lua_State* L)
{
    uint8_t ch;
    float re;

    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 */
    {
        ch = luaL_checknumber(L, 1);    /* 模拟通道 */
    }

    if (ch == AN_CH1)                   re = g_tVar.CH1Volt;            /* CH1电压 */
    else if (ch == AN_CH2)              re = g_tVar.CH2Volt;            /* CH2电压 */
    else if (ch == AN_HIGH_SIDE_VOLT)   re = g_tVar.HighSideVolt;
    else if (ch == AN_HIGH_SIDE_CURR)   re = g_tVar.HighSideCurr;
    else if (ch == AN_TVCC_VOLT)        re = g_tVar.TVCCVolt;           /* TVCC实测电压 */
    else if (ch == AN_TVCC_CURR)        re = g_tVar.TVCCCurr;           /* TVCC实测电压 */
    else if (ch == AN_NTC_RES)          re = g_tVar.NTCRes;             /* NTC电阻 */
    else if (ch == AN_12V_VOLT)         re = g_tVar.ExtPowerVolt;
    else if (ch == AN_USB_VOLT)         re = g_tVar.USBPowerVolt;    
    else re = 0;
    
    lua_pushnumber(L, re);    /* 成功,返回数据 */
    
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
