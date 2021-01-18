/*
*********************************************************************************************************
*
*    模块名称 : lua 显示接口
*    文件名称 : lua_if_disp.c
*    版    本 : V1.0
*    说    明 : 提供绘图函数、显示函数
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2020-11-29 armfly  首发
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"
#include "disp_lib.h"

/*
    API函数

    lcd_clr(color)  --清屏
    lcd_disp_str(x, y, str, fontzize, front_color, back_color) --显示字符串
    lcd_disp_str(x, y, str, fontzize, front_color, back_color, width, align) --显示字符串
    lcd_fill_rect(x, y, h, w, color) --清矩形窗口
    lcd_draw_rect(x, y, h, w, color) 

    lcd_draw_circle(x, y, r, color)
    lcd_draw_line(x1, y1, x2, y2, color)      

    lcd_disp_label(x, y, h, w, color, str, fontzize, front_color, back_color)
    
    void DispLabelRound(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    uint16_t _usColor, char *_pStr, FONT_T *_tFont)
    
void LCD_ClrScr(uint16_t _usColor);
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY);
void LCD_DrawLine(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor);
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor);
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void LCD_SetBackLight(uint8_t _bright);
uint8_t LCD_GetBackLight(void);

void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);    
*/
static int lua_RGB565(lua_State* L);
static int lua_LcdRefresh(lua_State* L);
static int lua_LcdClr(lua_State* L);
static int lua_DispStr(lua_State* L);
static int lua_FillRect(lua_State* L);
static int lua_DrawRect(lua_State* L);
static int lua_DrawCircle(lua_State* L);
static int lua_DrawLine(lua_State* L);
//static int lua_DrawPoints(lua_State* L);
static int lua_DrawLabel(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_adc_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_lcd_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "RGB565", lua_RGB565);
    lua_register(g_Lua, "lcd_refresh", lua_LcdRefresh);
    lua_register(g_Lua, "lcd_clr", lua_LcdClr);
    lua_register(g_Lua, "lcd_disp_str", lua_DispStr);  
    lua_register(g_Lua, "lcd_fill_rect", lua_FillRect);  
    lua_register(g_Lua, "lcd_draw_rect", lua_DrawRect);  
    lua_register(g_Lua, "lcd_draw_circle", lua_DrawCircle);  
    lua_register(g_Lua, "lcd_draw_line", lua_DrawLine);  
//    lua_register(g_Lua, "lcd_draw_points", lua_DrawPoints);  
    lua_register(g_Lua, "lcd_disp_label", lua_DrawLabel);        
}

/*
*********************************************************************************************************
*    函 数 名: lua_LcdRefresh
*    功能说明: lcd_refresh()
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void ST7789_DrawScreenHard(void);
static int lua_LcdRefresh(lua_State* L)
{
    ST7789_DrawScreenHard();    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_RGB565
*    功能说明: RGB565(255,255,255)  颜色转换为565
*    形    参: R : G : B 红蓝分量(0-255)
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_RGB565(lua_State* L)
{
    uint16_t r,g,b;
    uint16_t color = 0;

    if (lua_type(L, 1) == LUA_TNUMBER)
    {
        r = luaL_checknumber(L, 1);
        if (lua_type(L, 2) == LUA_TNUMBER)
        {
            g = luaL_checknumber(L, 2);
            if (lua_type(L, 3) == LUA_TNUMBER)
            {
                b = luaL_checknumber(L, 3);
                
                color = RGB(r,g,b);
            }             
        }  
    }

    lua_pushnumber(L, color);    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_LcdClr
*    功能说明: lcd_clr(color)  --清屏
*    形    参: color : 颜色 RGB565格式
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_LcdClr(lua_State* L)
{
    uint16_t color;

    if (lua_type(L, 1) == LUA_TNUMBER)  /* 第1个参数 */
    {
        color = luaL_checknumber(L, 1);
    }
    
    LCD_ClrScr(color);
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_DispStr
*    功能说明: lcd_disp_str(x, y, str, fontzize, front_color, back_color, width, align) --显示字符串
*    形    参: ...
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_DispStr(lua_State* L)
{
    uint16_t x, y;
    const char *str;
    size_t len; 
    uint8_t fontzize;
    uint16_t front_color, back_color;
    uint16_t width;
    uint8_t align;

    /* 第1个参数 */
    if (lua_type(L, 1) == LUA_TNUMBER)  
    {
        x = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }
    
    /* 第2个参数 */
    if (lua_type(L, 2) == LUA_TNUMBER)  
    {
        y = luaL_checknumber(L, 2);
    }
    else
    {
        return 0;
    }    
    
    /* 第3个参数 */
    if (lua_type(L, 3) == LUA_TSTRING)  
    {
        str = luaL_checklstring(L, 3, &len);
    }
    else
    {
        return 0;
    }

    /* 第4个参数 */
    if (lua_type(L, 4) == LUA_TNUMBER)  
    {
        fontzize = luaL_checknumber(L, 4);
    }
    else
    {
        return 0;
    } 
    
    /* 第5个参数 */
    if (lua_type(L, 5) == LUA_TNUMBER)  
    {
        front_color = luaL_checknumber(L, 5);
    }
    else
    {
        return 0;
    }
    
    /* 第6个参数 */
    if (lua_type(L, 6) == LUA_TNUMBER)  
    {
        back_color = luaL_checknumber(L, 6);
    }
    else
    {
        return 0;
    }
    
    /* 第7个参数 */
    if (lua_type(L, 7) == LUA_TNUMBER)  
    {
        width = luaL_checknumber(L, 7);
    }
    else
    {
        return 0;
    }
    
    /* 第8个参数 */
    if (lua_type(L, 8) == LUA_TNUMBER)  
    {
        align = luaL_checknumber(L, 8);
    }
    else
    {
        return 0;
    }
    
    {
        FONT_T tFont;
        
        if (fontzize == 12) tFont.FontCode = FC_ST_12;
        else if (fontzize == 16) tFont.FontCode = FC_ST_16;
        else if (fontzize == 24) tFont.FontCode = FC_ST_24;
        else if (fontzize == 32) tFont.FontCode = FC_ST_32;
        else {
            return 0;
        }        
        tFont.FrontColor = front_color;     /* 字体颜色 */
        tFont.BackColor = back_color;       /* 文字背景颜色 */
        tFont.Space = 0;
        
        LCD_SetEncode(ENCODE_GBK);
        LCD_DispStrEx(x, y, (char *)str, &tFont, width, align);
        LCD_SetEncode(ENCODE_UTF8);
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_FillRect
*    功能说明: lcd_fill_rect(x, y, h, w, color) --清矩形窗口
*    形    参: ...
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_FillRect(lua_State* L)
{
    uint16_t x, y, h, w;
    uint16_t color;

    /* 第1个参数 */
    if (lua_type(L, 1) == LUA_TNUMBER)  
    {
        x = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }
    
    /* 第2个参数 */
    if (lua_type(L, 2) == LUA_TNUMBER)  
    {
        y = luaL_checknumber(L, 2);
    }
    else
    {
        return 0;
    }    
    
    /* 第3个参数 */
    if (lua_type(L, 3) == LUA_TNUMBER)  
    {
        h = luaL_checknumber(L, 3);
    }
    else
    {
        return 0;
    } 

    /* 第4个参数 */
    if (lua_type(L, 4) == LUA_TNUMBER)  
    {
        w = luaL_checknumber(L, 4);
    }
    else
    {
        return 0;
    }

    /* 第5个参数 */
    if (lua_type(L, 5) == LUA_TNUMBER)  
    {
        color = luaL_checknumber(L, 5);
    }
    else
    {
        return 0;
    }

    LCD_Fill_Rect(x, y, h, w, color);

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_DrawRect
*    功能说明: lcd_draw_rect(x, y, h, w, color) --清矩形窗口
*    形    参: ...
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_DrawRect(lua_State* L)
{
    uint16_t x, y, h, w;
    uint16_t color;

    /* 第1个参数 */
    if (lua_type(L, 1) == LUA_TNUMBER)  
    {
        x = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }
    
    /* 第2个参数 */
    if (lua_type(L, 2) == LUA_TNUMBER)  
    {
        y = luaL_checknumber(L, 2);
    }
    else
    {
        return 0;
    }    
    
    /* 第3个参数 */
    if (lua_type(L, 3) == LUA_TNUMBER)  
    {
        h = luaL_checknumber(L, 3);
    }
    else
    {
        return 0;
    } 

    /* 第4个参数 */
    if (lua_type(L, 4) == LUA_TNUMBER)  
    {
        w = luaL_checknumber(L, 4);
    }
    else
    {
        return 0;
    }

    /* 第5个参数 */
    if (lua_type(L, 5) == LUA_TNUMBER)  
    {
        color = luaL_checknumber(L, 5);
    }
    else
    {
        return 0;
    }

    LCD_DrawRect(x, y, h, w, color);     

    return 0;
}
    
    
/*
*********************************************************************************************************
*    函 数 名: lua_DrawCircle
*    功能说明: lcd_fill_circle(x, y, r, color)
*    形    参: ...
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_DrawCircle(lua_State* L)
{
    uint16_t x, y, r;
    uint16_t color;

    /* 第1个参数 */
    if (lua_type(L, 1) == LUA_TNUMBER)  
    {
        x = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }
    
    /* 第2个参数 */
    if (lua_type(L, 2) == LUA_TNUMBER)  
    {
        y = luaL_checknumber(L, 2);
    }
    else
    {
        return 0;
    }    
    
    /* 第3个参数 */
    if (lua_type(L, 3) == LUA_TNUMBER)  
    {
        r = luaL_checknumber(L, 3);
    }
    else
    {
        return 0;
    } 

    /* 第4个参数 */
    if (lua_type(L, 4) == LUA_TNUMBER)  
    {
        color = luaL_checknumber(L, 4);
    }
    else
    {
        return 0;
    }

    LCD_DrawCircle(x, y, r, color); 
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_DrawLine
*    功能说明: lcd_draw_line(x1, y1, x2, y2, color)
*    形    参: ...
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_DrawLine(lua_State* L)
{
    uint16_t x1, y1, x2, y2;
    uint16_t color;

    /* 第1个参数 */
    if (lua_type(L, 1) == LUA_TNUMBER)  
    {
        x1 = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }
    
    /* 第2个参数 */
    if (lua_type(L, 2) == LUA_TNUMBER)  
    {
        y1 = luaL_checknumber(L, 2);
    }
    else
    {
        return 0;
    }    
    
    /* 第3个参数 */
    if (lua_type(L, 3) == LUA_TNUMBER)  
    {
        x2 = luaL_checknumber(L, 3);
    }
    else
    {
        return 0;
    } 

    /* 第4个参数 */
    if (lua_type(L, 4) == LUA_TNUMBER)  
    {
        y2 = luaL_checknumber(L, 4);
    }
    else
    {
        return 0;
    }

    /* 第5个参数 */
    if (lua_type(L, 5) == LUA_TNUMBER)  
    {
        color = luaL_checknumber(L, 5);
    }
    else
    {
        return 0;
    }
    
    LCD_DrawLine(x1, y1, x2, y2, color);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_DrawLabel
*    功能说明: lcd_disp_label(x, y, h, w, color, str, fontzize, front_color, back_color)
*    形    参: ...
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_DrawLabel(lua_State* L)
{
    uint16_t x, y, h, w;
    uint16_t color;
    const char *str;
    size_t len;
    uint16_t fontzize, front_color, back_color;

    /* 第1个参数 */
    if (lua_type(L, 1) == LUA_TNUMBER)  
    {
        x = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }
    
    /* 第2个参数 */
    if (lua_type(L, 2) == LUA_TNUMBER)  
    {
        y = luaL_checknumber(L, 2);
    }
    else
    {
        return 0;
    }    
    
    /* 第3个参数 */
    if (lua_type(L, 3) == LUA_TNUMBER)  
    {
        h = luaL_checknumber(L, 3);
    }
    else
    {
        return 0;
    } 

    /* 第4个参数 */
    if (lua_type(L, 4) == LUA_TNUMBER)  
    {
        w = luaL_checknumber(L, 4);
    }
    else
    {
        return 0;
    }

    /* 第5个参数 */
    if (lua_type(L, 5) == LUA_TNUMBER)  
    {
        color = luaL_checknumber(L, 5);
    }
    else
    {
        return 0;
    }
    
    /* 第6个参数 */
    if (lua_type(L, 6) == LUA_TSTRING)  
    {
        str = luaL_checklstring(L, 6, &len);
    }
    else
    {
        return 0;
    }
    
    /* 第7个参数 */
    if (lua_type(L, 7) == LUA_TNUMBER)  
    {
        fontzize = luaL_checknumber(L, 7);
    }
    else
    {
        return 0;
    }

    
    /* 第8个参数 */
    if (lua_type(L, 8) == LUA_TNUMBER)  
    {
        front_color = luaL_checknumber(L, 8);
    }
    else
    {
        return 0;
    }    
    
    /* 第9个参数 */
    if (lua_type(L, 9) == LUA_TNUMBER)  
    {
        back_color = luaL_checknumber(L, 9);
    }
    else
    {
        return 0;
    } 
    
    {
        FONT_T tFont;
        
        if (fontzize == 12) tFont.FontCode = FC_ST_12;
        else if (fontzize == 16) tFont.FontCode = FC_ST_16;
        else if (fontzize == 24) tFont.FontCode = FC_ST_24;
        else if (fontzize == 32) tFont.FontCode = FC_ST_32;
        else {
            return 0;
        }        
        tFont.FrontColor = front_color;     /* 字体颜色 */
        tFont.BackColor = back_color;       /* 文字背景颜色 */
        tFont.Space = 0;
        
        DispLabelRound(x, y, h, w, color, (char *)str, &tFont);
    }

    return 0;
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
