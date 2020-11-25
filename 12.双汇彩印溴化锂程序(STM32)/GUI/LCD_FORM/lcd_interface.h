#ifndef _LCD_INTERFACE_H
#define _LCD_INTERFACE_H

#ifndef _LCD_INTERFACE_C
#define EXT_LCD_INF extern
#else
#define EXT_LCD_INF
#endif


#define TOKEN_LOGO   0xFFFF     //首屏

#define TOKEN_MAIN   0x0000     //主菜单

#define TOKEN_PARA   0x1000     //参数设置菜单
#define TOKEN_PORT   0x2000     //端口设置菜单
#define TOKEN_SERIAL 0x3000     //串口显示菜单
#define TOKEN_CAN    0x4000     //CAM显示菜单
#define TOKEN_ETH    0x5000     //以太网显示菜单
#define TKOEN_TERM   0x6000     //终端显示菜单

#define TOKEN_TIME   0x1100     //时间设置显示
#define TOKEN_BKG    0x1200     //背光延时显示
#define TOKEN_COMM   0x1300     //通讯设置显示
#define TOKEN_FAULT  0x1400     //故障灯定义显示
#define TOKEN_ALARM  0x1500     //报警设置显示
#define TOKEN_PWD    0x1600     //密码设置显示

#define TOKEN_TERMCS 0x2100     //串口终端配置显示
#define TOKEN_TERMCE 0x2200     //以太网终端配置显示

#define TOKEN_PORTS  0x3100     //串行口状态显示
#define TOKEN_FRAME1 0x3200     //报文过滤显示
#define TOKEN_RECE1  0x3300     //串行口接收数据显示
#define TOKEN_SEND1  0x3400     //串行口发收数据显示

#define TOKEN_CANS   0x4100     //CAN状态显示
#define TOKEN_FRAME2 0x4200     //CAN报文过滤
#define TOKEN_RECE2  0x4300     //CAN接收数据显示
#define TOKEN_SEND2  0x4400     //CAN发送数据显示

#define TOKEN_ETHS   0x5100     //以太网socket套接口状态显示
#define TOKEN_RECE3  0x5200     //以太网接收报文显示
#define TOKEN_SEND3  0x5300     //以太网发送报文显示

#define TOKEN_TERMS  0x6100     //终端状态显示
#define TOKEN_YX     0x6200     //遥信显示
#define TOKEN_YC     0x6300     //遥测显示
#define TOKEN_KWH    0x6400     //电度量的显示
#define TOKEN_SOE    0x6500     //SOE的显示

#define TOKEN_YXC    0x2110     //遥信配置显示
#define TOKEN_YCC    0x2120     //遥测配置显示


#define M_MENU_ITEM  0x01       //菜单类
#define M_DLIST_ITEM 0x02       //枚举型列表框类
#define M_NUM_ITEM   0x03       //数字显示类
#define M_IP_ITEM    0x04       //IP地址类




//---------------------------------------
#define TASK_LCD_INF_STK_SIZE	256

EXT_LCD_INF OS_STK TaskLcdInfStk[TASK_LCD_INF_STK_SIZE];

EXT_LCD_INF void TaskLcdInf(void *pdata);

EXT_LCD_INF void WaitKey(s16 time_long,u16 key);


#endif