/*
*********************************************************************************************************
*
*    模块名称 : lua 串口模块
*    文件名称 : lua_if_uart.c
*    版    本 : V1.0
*    说    明 : UART串口操作，MODBUS操作函数
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2019-11-23 armfly  首发
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
#include "modbus_slave.h"

#define LUA_ERR_PARAM_PRINT(...)
//#define LUA_ERR_PARAM_PRINT     printf

#define VALID_PORT(x) (x ==  COM1 || x ==  COM4 || x ==  COM7 || x ==  COM8)

#define WRITE_REG_MAX_NUM   16      /* 写保持寄存器最大个数 */
#define READ_REG_MAX_NUM    16      /* 读保持寄存器最大个数 */

#define WRITE_DO_MAX_NUM    32      /* 写DO寄存器最大个数 */
#define READ_DO_MAX_NUM     32      /* 写DO寄存器最大个数 */
#define READ_DI_MAX_NUM     32      /* 读DI寄存器最大个数 */

/*
    用法:
    
    uart_send(1, "123")   --向COM1 (RS485 232, TTL UART）发送 123 字符串
    
    len, buf = uart_recive(1, 8, 300) --读COM1数据,8个字节，超时300ms.
        -- len : 数据长度，字节
        -- buf : 数据 string （可以是二进制，可以包括0字符）
    
    com号: 支持 COM1, COM4, COM7, COM8
*/
static int lua_uart_cfg(lua_State* L);
    
static int lua_uart_send(lua_State* L);
static int lua_uart_recive(lua_State* L);
static int lua_uart_clear_rx_fifo(lua_State* L);

static int lua_uart_WriteReg16(lua_State* L);
static int lua_uart_WriteReg32(lua_State* L);
static int lua_uart_WriteRegFloat(lua_State* L);

static int lua_uart_ReadRegU16(lua_State* L);
static int lua_uart_ReadRegU32(lua_State* L);
static int lua_uart_ReadRegS16(lua_State* L);
static int lua_uart_ReadRegS32(lua_State* L);
static int lua_uart_ReadRegFloat(lua_State* L); 

static int lua_uart_WriteDO(lua_State* L);
static int lua_uart_ReadDO(lua_State* L);
static int lua_uart_ReadDI(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_uart_RegisterFun
*    功能说明: 注册lua接口api函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_uart_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "uart_cfg", lua_uart_cfg);
    lua_register(g_Lua, "uart_send", lua_uart_send);
    lua_register(g_Lua, "uart_recive", lua_uart_recive);
    lua_register(g_Lua, "uart_clear_rx", lua_uart_clear_rx_fifo);
    
    lua_register(g_Lua, "modbus_write_u16", lua_uart_WriteReg16); 
    lua_register(g_Lua, "modbus_write_u32", lua_uart_WriteReg32);
    lua_register(g_Lua, "modbus_write_float", lua_uart_WriteRegFloat);

    lua_register(g_Lua, "modbus_read_u16", lua_uart_ReadRegU16);    
    lua_register(g_Lua, "modbus_read_u32", lua_uart_ReadRegU32);
    lua_register(g_Lua, "modbus_read_s16", lua_uart_ReadRegS16);    
    lua_register(g_Lua, "modbus_read_s32", lua_uart_ReadRegS32);    
    lua_register(g_Lua, "modbus_read_float", lua_uart_ReadRegFloat);      

    lua_register(g_Lua, "modbus_write_do", lua_uart_WriteDO);
    lua_register(g_Lua, "modbus_read_do", lua_uart_ReadDO);
    lua_register(g_Lua, "modbus_read_di", lua_uart_ReadDI);     
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_cfg
*    功能说明: 配置串口参数
*    形    参: 端口号， 波特率，奇偶校验, 数据长度，停止位
*    返 回 值: 无
*********************************************************************************************************
*/
#define STR_UART_CFG   "\r\nuart_cfg(port, BaudRate, Parity, WordLength, StopBits)\r\n--parameter error : "
static int lua_uart_cfg(lua_State* L)
{
    COM_PORT_E port;
    uint32_t BaudRate;    
    uint32_t Parity;
    uint32_t WordLength;
    uint32_t StopBits;    

    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 : com 端口 */
    {
        port = (COM_PORT_E)luaL_checknumber(L, 1);
        if (VALID_PORT(port))
        {
            ;
        }
        else
        {
            LUA_ERR_PARAM_PRINT(STR_UART_CFG"port\r\n");            
            return 0;
        }
    }
    
    if (lua_type(L, 2) == LUA_TNUMBER)      /* 第2个参数 : 波特率 */
    {
        BaudRate = luaL_checknumber(L, 2);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_UART_CFG"BaudRate\r\n");;            
        return 0;
    }    

    if (lua_type(L, 3) == LUA_TNUMBER)      /* 第3个参数 : 奇偶校验 */
    {
        Parity = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_UART_CFG"Parity\r\n");;            
        return 0;
    }        

    if (lua_type(L, 4) == LUA_TNUMBER)      /* 第4个参数 : 数据长度 */
    {
        WordLength = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_UART_CFG"WordLength\r\n");;            
        return 0;
    }     

    if (lua_type(L, 5) == LUA_TNUMBER)      /* 第5个参数 : 停止位 */
    {
        StopBits = luaL_checknumber(L, 5);
    }    
    else
    {
        LUA_ERR_PARAM_PRINT(STR_UART_CFG"StopBits\r\n");;            
        return 0;
    }      
    
	/* set the Stop bit */
	switch (StopBits)
	{
		case 0:
			StopBits = UART_STOPBITS_1;
			break;
		case 2:
			StopBits = UART_STOPBITS_2;
			break;
		default:
			StopBits = UART_STOPBITS_1;
			break;
	}

	/* set the parity bit */
	switch (Parity)
	{
        case 0:
            Parity = UART_PARITY_NONE;
            break;
        case 1:
            Parity = UART_PARITY_ODD;
            break;
        case 2:
            Parity = UART_PARITY_EVEN;
            break;
        default:
            Parity = UART_PARITY_NONE;
            break;
	}
    
	/* set the data type : only 8bits and 9bits is supported */
	switch (WordLength)
	{
		case 0x07:
			/* With this configuration a parity (Even or Odd) must be set */
			WordLength = UART_WORDLENGTH_8B;
			break;
        
		case 0x08:
			if (Parity == UART_PARITY_NONE)
			{
				WordLength = UART_WORDLENGTH_8B;
			}
			else
			{
				WordLength = UART_WORDLENGTH_9B;
			}
			break;
            
		default:
			WordLength = UART_WORDLENGTH_8B;
			break;
	}
    
    bsp_SetUartParam(port, BaudRate, Parity, WordLength, StopBits);
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_send
*    功能说明: 发送数据的函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_uart_send(lua_State* L)
{
    size_t len;
    const char *data;
    COM_PORT_E port;   /* com号: 支持 COM1, COM4, COM7, COM8 */
    
    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 : com 端口 */
    {
        port = (COM_PORT_E)luaL_checknumber(L, 1);
        if (VALID_PORT(port))
        {
            ;
        }
        else
        {
            LUA_ERR_PARAM_PRINT("\r\nerror : %s,%s,%d\r\n", "uart_send", "port=",port);
            return 0;
        }
    }
    
    if (lua_type(L, 2) == LUA_TSTRING)  /* 判断第2个参数 二级制字符串 */
    {        
        data = luaL_checklstring(L, 2, &len);   /* len是string的长度 */   
    }
    else
    {
        LUA_ERR_PARAM_PRINT("\r\nerror : %sr\n", "uart_send() no param2");
        return 0;        
    }
    
    comSendBuf(port, (uint8_t *)data, len);
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_recive
*    功能说明: 读取串口数据的函数
*    形    参: prot, maxlen, timeout
*    返 回 值: len, readdata
*********************************************************************************************************
*/
static int lua_uart_recive(lua_State* L)
{
    size_t maxlen;
    size_t timeout;
    COM_PORT_E port;        /* com号: 支持 COM1, COM4, COM7, COM8 */
    uint32_t len;
    int32_t time1;
    
    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 : com 端口 */
    {
        port = (COM_PORT_E)luaL_checknumber(L, 1);
        if (VALID_PORT(port))
        {
            ;
        }
        else
        {
            LUA_ERR_PARAM_PRINT("\r\nerror : %s,%s,%d\r\n", "uart_recive", "port=",port);
            return 0;
        }
    }    
    
    maxlen = luaL_checknumber(L, 2);
    if (maxlen == 0 || maxlen > LUA_READ_LEN_MAX)
    {
        LUA_ERR_PARAM_PRINT("\r\nerror : %s,%s,%d\r\n", "uart_recive", "maxlen=",maxlen);
        return 0;
    }
    
    time1 = bsp_GetRunTime();
    timeout = luaL_checknumber(L, 3);   
    len = 0;
    while (1)
    {
        uint8_t ch;
        
        if (comGetChar(port, &ch))
        {
            s_lua_read_buf[len++] = ch;    /* 读1个字节 */
            if (len >= maxlen)
            {
                break;
            }
            time1 = bsp_GetRunTime();
            continue;
        }
        
        bsp_Idle();
        
        if (bsp_CheckRunTime(time1) >= timeout)
        {
            break;  /* 超时 */
        }
    }
    
    lua_pushnumber(L, len);        
    lua_pushlstring(L, (char *)s_lua_read_buf, len); 
    return 2;   /* 返回2个参数 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_clear_rx_fifo
*    功能说明: 清接收缓冲区数据
*    形    参: prot, maxlen, timeout
*    返 回 值: len, readdata
*********************************************************************************************************
*/
static int lua_uart_clear_rx_fifo(lua_State* L)
{
    COM_PORT_E port;        /* com号: 支持 COM1, COM4, COM7, COM8 */
    
    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 : com 端口 */
    {
        port = (COM_PORT_E)luaL_checknumber(L, 1);
        if (VALID_PORT(port))
        {
            ;
        }
        else
        {
            LUA_ERR_PARAM_PRINT("\r\nerror : %s,%s,%d\r\n", "uart_recive", "port=",port);
            return 0;
        }
    }    
    
    comClearRxFifo(port);   /* 清除接收缓冲区 */
    
    return 0;   /* 返回0个参数 */
}

/*
*********************************************************************************************************
*    函 数 名: uart_ReadModbusAck
*    功能说明: 读modbus应答数据
*    形    参: _port : COM口
*              _rx_buf : 接收缓冲区
*              _maxlen : 缓冲区最大长度
*              _timeout : 超时时间 ms (最后1个字节起计算超时）
*    返 回 值: 无
*********************************************************************************************************
*/
static int uart_ReadModbusAck(uint8_t _port, uint8_t *_rx_buf, uint16_t _buf_size, uint16_t _timeout)
{
    int32_t time1;
    uint16_t len;
    uint16_t crc;
    
    time1 = bsp_GetRunTime();
    len = 0;
    while (1)
    {
        uint8_t ch;
        
        if (comGetChar((COM_PORT_E)_port, &ch))
        {            
            if (len < _buf_size)
            {
                _rx_buf[len++] = ch;    /* 读1个字节 */
            }
            time1 = bsp_GetRunTime();
            
            _timeout = 5;    /* 固5ms字符间超时 9600 */
            continue;
        }
        
        bsp_Idle();
        
        if (bsp_CheckRunTime(time1) >= _timeout)
        {
            break;  /* 超时 */
        }
    }
    
    if (len > 0)
    {
        crc = CRC16_Modbus(_rx_buf, len);        
        if (crc == 0)
        {
            return len;     /* 应答数据包CRC检验通过 */
        }
    }
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_WriteReg16
*    功能说明: 写1个或多个寄存器 16bit (最多支持写16个）
*    形    参: port, timeout, addr485, regaddr, value0, value1, value2 ...
*    返 回 值: 0 成功  > 0 错误代码。 错误代码定义见 modbus_salve.h
*********************************************************************************************************
*/
#define STR_W_U16   "\r\nmodbus_write_u16(port, timeout, addr485, regaddr, value0, value1, value2 ...)\r\n--parameter error : "
static int lua_uart_WriteReg16(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t value[WRITE_REG_MAX_NUM];
    uint8_t regnum;
    uint16_t timeout;
    uint8_t txbuf[WRITE_REG_MAX_NUM * 2 + 5];
    uint8_t rxbuf[32];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint8_t i;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U16"port\r\n");
        return 0;
    }

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数: 超时 ms*/
    {
        timeout = (uint16_t)luaL_checknumber(L, 2); 
    }    
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U16"timeout\r\n");
        return 0;
    }    
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 判断第3个参数: 485地址*/
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U16"addr485\r\n");
        return 0;
    }     

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 判断第4个参数 : 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    } 
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U16"regaddr\r\n");
        return 0;
    }    
    
    regnum = 0;
    for (i = 0; i < WRITE_REG_MAX_NUM; i++)
    {
        int32_t temp;
        
        if (lua_type(L, 5 + i) == LUA_TNUMBER)          /* 判断第5个参数 寄存器值，支持多个 */
        {
            temp = luaL_checknumber(L, 5 + i);              /* 寄存器值, 16bit */
            value[regnum++] = (uint16_t)temp;                        
        }  
        else
        {
            break;
        }
    }
    
    if (regnum == 0)
    {
        LUA_ERR_PARAM_PRINT(STR_W_U16"regnum=0\r\n");
        return 0;
    }
    
    if (regnum == 1)    /* 1个寄存器使用0x06功能 */
    {
        
        txbuf[pos++] = addr485;
        txbuf[pos++] = 0x06;
        txbuf[pos++] = regaddr >> 8;
        txbuf[pos++] = regaddr;
        txbuf[pos++] = value[0] >> 8;
        txbuf[pos++] = value[0];
        crc = CRC16_Modbus(txbuf, pos);
        txbuf[pos++] = crc >> 8;
        txbuf[pos++] = crc;
    }
    else    /* 多个寄存器使用0x10功能 */
    {
        txbuf[pos++] = addr485;
        txbuf[pos++] = 0x10;
        txbuf[pos++] = regaddr >> 8;
        txbuf[pos++] = regaddr;
        
        txbuf[pos++] = regnum >> 8;
        txbuf[pos++] = regnum;
        
        txbuf[pos++] = regnum * 2;
        
        for (i = 0; i < regnum; i++)
        {
            txbuf[pos++] = value[i] >> 8;
            txbuf[pos++] = value[i];
        }
        crc = CRC16_Modbus(txbuf, pos);
        txbuf[pos++] = crc >> 8;
        txbuf[pos++] = crc;        
    }
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxlen == 8 && memcmp(rxbuf, txbuf, 6) == 0)
    {
        lua_pushnumber(L, RSP_OK);          /* 成功返回0 */
        return 1;
    }
    
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];     
    }       
    lua_pushnumber(L, errcode);   /* 成功 */
    LUA_ERR_PARAM_PRINT("\r\nmodbus_write_u16() regaddr = %04X errcode = %d\r\n", regaddr, errcode);        
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_ReadRegU16
*    功能说明: 读寄存器 16bit
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_U16   "\r\nmodbus_read_u16(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadRegU16(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_REG_MAX_NUM * 2 + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U16"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U16"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U16"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U16"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U16"regnum\r\n");
        return 0;
    }    
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x03;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    if (num > READ_REG_MAX_NUM)
    {
        LUA_ERR_PARAM_PRINT(STR_R_U16"regnum > %d\r\n", READ_REG_MAX_NUM);
        return 0;
    }
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x03 && rxlen == 2 * num + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            value = BEBufToUint16(&rxbuf[3 + 2 * i]);            
            lua_pushnumber(L, value);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_u16() regaddr = %04X errcode = %d\r\n", regaddr, errcode);    
    return 1;   /* 返回 错误代码 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_ReadRegS16
*    功能说明: 读寄存器 16bit
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_S16   "\r\nmodbus_read_s16(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadRegS16(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_REG_MAX_NUM * 2 + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    int16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S16"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S16"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S16"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S16"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S16"regnum\r\n");
        return 0;
    }    
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x03;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    if (num > READ_REG_MAX_NUM)
    {
        LUA_ERR_PARAM_PRINT(STR_R_S16"regnum > %d\r\n", READ_REG_MAX_NUM);
        return 0;
    }
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x03 && rxlen == 2 * num + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            value = (int16_t)BEBufToUint16(&rxbuf[3 + 2 * i]);            
            lua_pushnumber(L, value);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_s16() regaddr = %04X errcode = %d\r\n", regaddr, errcode);    
    return 1;   /* 返回 错误代码 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteReg32
*    功能说明: 写寄存器 32Bit整数
*    形    参: port, timeout, addr485, regaddr, value0, value1, value2 ...
*    返 回 值: 无
*********************************************************************************************************
*/
#define STR_W_U32   "\r\nmodbus_write_u32(port, timeout, addr485, regaddr, value0, value1, value2 ...)\r\n--parameter error : "
static int lua_uart_WriteReg32(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint32_t value[WRITE_REG_MAX_NUM / 2];
    uint8_t regnum;
    uint16_t timeout;
    uint8_t txbuf[WRITE_REG_MAX_NUM * 2 + 5];
    uint8_t rxbuf[32];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint8_t i;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U32"port\r\n");
        return 0;
    }

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数: 超时 ms*/
    {
        timeout = (uint16_t)luaL_checknumber(L, 2); 
    }    
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U32"timeout\r\n");
        return 0;
    }    
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 判断第3个参数: 485地址*/
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U32"addr485\r\n");
        return 0;
    }     

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 判断第4个参数 : 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    } 
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_U32"regaddr\r\n");
        return 0;
    }    
    
    regnum = 0;
    for (i = 0; i < WRITE_REG_MAX_NUM / 2; i++)
    {
        uint32_t temp;
        
        if (lua_type(L, 5 + i) == LUA_TNUMBER)          /* 判断第5个参数 寄存器值，支持多个 */
        {
            temp = luaL_checknumber(L, 5 + i);              /* 寄存器值, 32bit */
            value[regnum++] = temp;                        
        }  
        else
        {
            break;
        }
    }
    
    if (regnum == 0)
    {
        LUA_ERR_PARAM_PRINT(STR_W_U32"regnum=0\r\n");
        return 0;
    }
    
    regnum = regnum * 2;
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x10;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    
    txbuf[pos++] = regnum >> 8;
    txbuf[pos++] = regnum;
    
    txbuf[pos++] = regnum * 2;
    
    for (i = 0; i < regnum / 2; i++)
    {
        txbuf[pos++] = value[i] >> 24;
        txbuf[pos++] = value[i] >> 16;
        txbuf[pos++] = value[i] >> 8;
        txbuf[pos++] = value[i];        
    }
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxlen == 8 && memcmp(rxbuf, txbuf, 6) == 0)
    {
        lua_pushnumber(L, RSP_OK);          /* 成功返回0 */
        return 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];
    }    
    lua_pushnumber(L, errcode);             /* 成功 */
    LUA_ERR_PARAM_PRINT("\r\nmodbus_write_u32() regaddr = %04X errcode = %d\r\n", regaddr, errcode);    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_ReadRegU32
*    功能说明: 读寄存器 32bit 无符号
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_U32   "\r\nmodbus_read_u32(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadRegU32(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_REG_MAX_NUM * 2 + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint32_t value32;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U32"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U32"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U32"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U32"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_U32"regnum\r\n");
        return 0;
    }    
    
    if (num > READ_REG_MAX_NUM / 2)
    {
        LUA_ERR_PARAM_PRINT(STR_R_U32"regnum > %d\r\n", READ_REG_MAX_NUM / 2);
        return 0;
    }
    
    num = num * 2;
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x03;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x03 && rxlen == 2 * num + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            value32 = BEBufToUint32(&rxbuf[3 + 4 * i]);            
            lua_pushnumber(L, value32);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];    
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_u32() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;   /* 返回 错误代码 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_ReadRegS32
*    功能说明: 读寄存器 32bit 有符号
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_S32   "\r\nmodbus_read_s32(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadRegS32(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_REG_MAX_NUM * 2 + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    int32_t value32;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S32"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S32"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S32"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S32"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_S32"regnum\r\n");
        return 0;
    }    
    
    if (num > READ_REG_MAX_NUM / 2)
    {
        LUA_ERR_PARAM_PRINT(STR_R_S32"regnum > %d\r\n", READ_REG_MAX_NUM / 2);
        return 0;
    }
    
    num = num * 2;
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x03;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x03 && rxlen == 2 * num + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            value32 = (int32_t)BEBufToUint32(&rxbuf[3 + 4 * i]);            
            lua_pushnumber(L, value32);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];    
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_s32() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;   /* 返回 错误代码 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteRegFloat
*    功能说明: 写寄存器 32Bit浮点
*    形    参: port, timeout, addr485, regaddr, value0, value1, value2 ...
*    返 回 值: 无
*********************************************************************************************************
*/
#define STR_W_FLOAT   "\r\nmodbus_write_float(port, timeout, addr485, regaddr, value0, value1, value2 ...)\r\n--parameter error : "
static int lua_uart_WriteRegFloat(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint32_t value[WRITE_REG_MAX_NUM / 2];
    uint8_t regnum;
    uint16_t timeout;
    uint8_t txbuf[WRITE_REG_MAX_NUM * 2 + 5];
    uint8_t rxbuf[32];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint8_t i;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_FLOAT"port\r\n");
        return 0;
    }

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数: 超时 ms*/
    {
        timeout = (uint16_t)luaL_checknumber(L, 2); 
    }    
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_FLOAT"timeout\r\n");
        return 0;
    }    
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 判断第3个参数: 485地址*/
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_FLOAT"addr485\r\n");
        return 0;
    }     

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 判断第4个参数 : 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    } 
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_FLOAT"regaddr\r\n");
        return 0;
    }    
    
    regnum = 0;
    for (i = 0; i < WRITE_REG_MAX_NUM / 2; i++)
    {
        float temp;
        
        if (lua_type(L, 5 + i) == LUA_TNUMBER)          /* 判断第5个参数 寄存器值，支持多个 */
        {
            temp = luaL_checknumber(L, 5 + i);              /* 寄存器值, 32bit */
            value[regnum++] = (uint32_t)temp;                        
        }  
        else
        {
            break;
        }
    }
    
    if (regnum == 0)
    {
        LUA_ERR_PARAM_PRINT(STR_W_FLOAT"regnum=0\r\n");
        return 0;
    }
    
    regnum = regnum * 2;
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x10;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    
    txbuf[pos++] = regnum >> 8;
    txbuf[pos++] = regnum;
    
    txbuf[pos++] = regnum * 2;
    
    for (i = 0; i < regnum / 2; i++)
    {
        txbuf[pos++] = value[i] >> 24;
        txbuf[pos++] = value[i] >> 16;
        txbuf[pos++] = value[i] >> 8;
        txbuf[pos++] = value[i];        
    }
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxlen == 8 && memcmp(rxbuf, txbuf, 6) == 0)
    {
        lua_pushnumber(L, RSP_OK);          /* 成功返回0 */
        return 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];
    }    
    lua_pushnumber(L, errcode);   /* 成功 */
    LUA_ERR_PARAM_PRINT("\r\nmodbus_write_float() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadRegFloat
*    功能说明: 读寄存器 浮点
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_FLOAT   "\r\nmodbus_read_float(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadRegFloat(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_REG_MAX_NUM * 2 + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    float valueff;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_FLOAT"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_FLOAT"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_FLOAT"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_FLOAT"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_FLOAT"regnum\r\n");
        return 0;
    }    
    
    if (num > READ_REG_MAX_NUM / 2)
    {
        LUA_ERR_PARAM_PRINT(STR_R_FLOAT"regnum > %d\r\n", READ_REG_MAX_NUM / 2);
        return 0;
    }
    
    num = num * 2;
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x03;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x03 && rxlen == 2 * num + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            valueff = BEBufToFloat(&rxbuf[3 + 4 * i]);            
            lua_pushnumber(L, valueff);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];      
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_float() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;   /* 返回 错误代码 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadRegFloat
*    功能说明: 读寄存器 浮点
*    形    参: port, timeout, addr485, regaddr, value0, value1 ...
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_W_DO   "\r\nmodbus_write_do(port, timeout, addr485, regaddr, value0, value1 ...)\r\n--parameter error : "
static int lua_uart_WriteDO(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint8_t value[(WRITE_DO_MAX_NUM + 7) / 8];
    uint8_t regnum;
    uint16_t timeout;
    uint8_t txbuf[WRITE_DO_MAX_NUM / 8 + 5 + 1];
    uint8_t rxbuf[32];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint8_t i;
    uint8_t bytes;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_DO"port\r\n");
        return 0;
    }

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数: 超时 ms*/
    {
        timeout = (uint16_t)luaL_checknumber(L, 2); 
    }    
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_DO"timeout\r\n");
        return 0;
    }    
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 判断第3个参数: 485地址*/
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_DO"addr485\r\n");
        return 0;
    }     

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 判断第4个参数 : 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    } 
    else
    {
        LUA_ERR_PARAM_PRINT(STR_W_DO"regaddr\r\n");
        return 0;
    }    
    
    regnum = 0;
    memset(value, 0, sizeof(value));
    for (i = 0; i < WRITE_DO_MAX_NUM; i++)
    {
        uint8_t temp;
        
        if (lua_type(L, 5 + i) == LUA_TNUMBER)          /* 判断第5个参数 寄存器值，支持多个 */
        {
            temp = luaL_checknumber(L, 5 + i);          /* 寄存器值, 32bit */
            
            if (temp == 1)
            {
                value[regnum / 8] |= (1 << (i % 8)); 
            }  
            regnum++;
        }  
        else
        {
            break;
        }
    }
    
    if (regnum == 0)
    {
        LUA_ERR_PARAM_PRINT(STR_W_DO"regnum=0\r\n");
        return 0;
    }
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x0F;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    
    txbuf[pos++] = regnum >> 8;     /* 寄存器个数 */
    txbuf[pos++] = regnum;
    
    bytes = (regnum + 7) / 8;       /* 字节数 */    
    txbuf[pos++] = bytes ;
    
    for (i = 0; i < bytes; i++)
    {
        txbuf[pos++] = value[i];   
    }
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxlen == 8 && memcmp(rxbuf, txbuf, 6) == 0)
    {
        lua_pushnumber(L, RSP_OK);          /* 成功返回0 */
        return 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];
    }    
    lua_pushnumber(L, errcode);   /* 成功 */
    LUA_ERR_PARAM_PRINT("\r\nmodbus_write_do() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;
}
    
/*
*********************************************************************************************************
*    函 数 名: lua_uart_ReadDO
*    功能说明: 读DO寄存器
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_DO   "\r\nmodbus_read_do(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadDO(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_DO_MAX_NUM + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint8_t bytes;
    uint8_t value;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DO"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DO"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DO"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DO"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DO"regnum\r\n");
        return 0;
    }    
    
    if (num > READ_DO_MAX_NUM)
    {
        LUA_ERR_PARAM_PRINT(STR_R_DO"regnum > %d\r\n", READ_DO_MAX_NUM);
        return 0;
    }
    
    bytes = (num + 7) / 8;       /* 字节数 */   
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x01;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x01 && rxlen == bytes + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            if (rxbuf[3 + i / 8] & (1 << (i % 8)))
            {
                value = 1;
            }
            else
            {
                value = 0;
            }          
            lua_pushnumber(L, value);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];      
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_do() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;   /* 返回 错误代码 */
}

/*
*********************************************************************************************************
*    函 数 名: lua_uart_ReadDI
*    功能说明: 读DI寄存器
*    形    参: port, timeout, addr485, regaddr, regnum
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
#define STR_R_DI   "\r\nmodbus_read_di(port, timeout, addr485, regaddr, regnum)\r\n--parameter error : "
static int lua_uart_ReadDI(lua_State* L)
{
    uint8_t port;
    uint16_t addr485;
    uint16_t regaddr;
    uint16_t num;
    uint16_t timeout;
    uint8_t txbuf[32];
    uint8_t rxbuf[READ_DO_MAX_NUM + 5];
    uint8_t pos = 0;
    uint8_t rxlen;
    uint16_t crc;
    uint8_t errcode;
    uint8_t bytes;
    uint8_t value;

    if (lua_type(L, 1) == LUA_TNUMBER)                  /* 第1个参数: COM端口号 */
    {
        port = luaL_checknumber(L, 1);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DI"port\r\n");
        return 0;
    }    

    if (lua_type(L, 2) == LUA_TNUMBER)                  /* 第2个参数 : 超时 */
    {
        timeout = (uint16_t)luaL_checknumber(L, 2);
    }  
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DI"timeout\r\n");
        return 0;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)                  /* 第3个参数 : 485地址 */
    {
        addr485 = luaL_checknumber(L, 3);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DI"addr485\r\n");
        return 0;
    }    

    if (lua_type(L, 4) == LUA_TNUMBER)                  /* 第4个参数 寄存器地址 */
    {
        regaddr = luaL_checknumber(L, 4);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DI"regaddr\r\n");
        return 0;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)                  /* 第5个参数 : 寄存器个数*/
    {
        num = (uint16_t)luaL_checknumber(L, 5);
    }
    else
    {
        LUA_ERR_PARAM_PRINT(STR_R_DI"regnum\r\n");
        return 0;
    }    
    
    if (num > READ_DI_MAX_NUM)
    {
        LUA_ERR_PARAM_PRINT(STR_R_DI"regnum > %d\r\n", READ_DI_MAX_NUM);
        return 0;
    }
    
    bytes = (num + 7) / 8;       /* 字节数 */   
    
    txbuf[pos++] = addr485;
    txbuf[pos++] = 0x02;
    txbuf[pos++] = regaddr >> 8;
    txbuf[pos++] = regaddr;
    txbuf[pos++] = num >> 8;
    txbuf[pos++] = num;
    crc = CRC16_Modbus(txbuf, pos);
    txbuf[pos++] = crc >> 8;
    txbuf[pos++] = crc;
    
    /* 发送命令 */
    comSendBuf((COM_PORT_E)port, txbuf, pos);
    
    errcode = ERR_TIMEOUT;
    rxlen = uart_ReadModbusAck(port, rxbuf, sizeof(rxbuf), timeout);
    if (rxbuf[1] == 0x02 && rxlen == bytes + 5)
    {        
        uint8_t i;
        
        lua_pushnumber(L, RSP_OK);      /* 成功返回0 */
            
        for (i = 0; i < num; i++)
        {
            if (rxbuf[3 + i / 8] & (1 << (i % 8)))
            {
                value = 1;
            }
            else
            {
                value = 0;
            }          
            lua_pushnumber(L, value);
        }
        return num + 1;
    }
    if (rxlen == 5 && (rxbuf[1] & 0x80))    /* 错误应答 */
    {
        /* 01 86 02 C1 C2 */
        errcode = rxbuf[2];      
    }    
    lua_pushnumber(L, errcode); 
    LUA_ERR_PARAM_PRINT("\r\nmodbus_read_di() regaddr = %04X errcode = %d\r\n", regaddr, errcode);       
    return 1;   /* 返回 错误代码 */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
