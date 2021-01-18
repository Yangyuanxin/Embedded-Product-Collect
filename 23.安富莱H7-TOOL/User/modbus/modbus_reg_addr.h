/*
*********************************************************************************************************
*
*    模块名称 : MODBUS 寄存器地址定义 （应用层）
*    文件名称 : modbus_reg_addr.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_REG_ADDR_H
#define __MODBUS_REG_ADDR_H

/* 硬件配置: DO,DI,AI,AO个数 
AI : 示波器 CH1 均值, mV单位 
     示波器 CH2 均值, mV单位
     高端电流检测-电压值  均值, mV单位
     高端电流检测-电流值  均值, uA单位
     USB 5V电压 均值, mV单位
     外部12V 电压 均值, mV单位
     TVCC电压 均值, mV单位
*/
#define MODS_DO_NUM 10
#define MODS_DI_NUM 10
#define MODS_AI_NUM 7
#define MODS_AO_NUM 2

/* 数字量输出状态寄存器，01H指令读线圈状态 05H、0FH是写线圈状态 */
#define REG01_Y01 0x0000 /* 第1路输出状态，0表示断开，1表示闭合 */
#define REG01_Y02 0x0001 /* 第2路输出状态 */
#define REG01_Y03 0x0002 /* 第3路输出状态 */
#define REG01_Y04 0x0003 /* 第4路输出状态 */
#define REG01_Y05 0x0004 /* 第5路输出状态 */
#define REG01_Y06 0x0005 /* 第6路输出状态 */
#define REG01_Y07 0x0006 /* 第7路输出状态 */
#define REG01_Y08 0x0007 /* 第8路输出状态 */
#define REG01_Y09 0x0008 /* 第7路输出状态 */
#define REG01_Y10 0x0009 /* 第8路输出状态 */
#define REG01_MAX REG01_Y10

/* 数字量输入寄存器，02H指令  读输入端口状态 */
#define REG02_X01 0x0000 /* 第1路输入状态，0表示低电平 1表示高电平 */
#define REG02_X02 0x0001 /* 第2路输入状态 */
#define REG02_X03 0x0002 /* 第3路输入状态 */
#define REG02_X04 0x0003 /* 第4路输入状态 */
#define REG02_X05 0x0004 /* 第5路输入状态 */
#define REG02_X06 0x0005 /* 第6路输入状态 */
#define REG02_X07 0x0006 /* 第7路输入状态 */
#define REG02_X08 0x0007 /* 第8路输入状态 */
#define REG02_X09 0x0008 /* 第9路输入状态 */
#define REG02_X10 0x0009 /* 第10路输入状态 */
#define REG02_MAX REG02_X10

/* 03H指令  保持寄存器， 06、10H用于写 */
#define REG03_DEV_ID0 0x0000 /*设备序列码：12个字节（96位），6个寄存器（只读），全球唯一的ID*/
#define REG03_DEV_ID1 0x0001
#define REG03_DEV_ID2 0x0002
#define REG03_DEV_ID3 0x0003
#define REG03_DEV_ID4 0x0004
#define REG03_DEV_ID5 0x0005

#define REG03_HARD_MODEL 0x0006 /* 硬件型号（只读），内部型号代码 0750 */
#define REG03_APP_VER    0x0007 /* APP固件版本，0x0102 表示V1.02 */

#define REG03_X15_00 0x0008 /* DI的实时状态（只读），Bit0表示D0通道，Bit15表示D15通道 */
#define REG03_X31_16 0x0009 /* DI的实时状态（只读）， */
#define REG03_Y15_00 0x000A /* DO的实时状态（只读）， */
#define REG03_Y31_16 0x000B /* DO的实时状态（只读）， */

#if 0
    AN_CH1 = 0,            /* CH1电压 */
    AN_CH2 = 1,            /* CH2电压 */
    AN_HIGH_SIDE_VOLT,    /* 高端负载电压 */    
    AN_HIGH_SIDE_CURR,    /* 高端负载电流 */

    AN_TVCC_VOLT,    /* TVCC电压检测    */
    AN_TVCC_CURR,    /* TVCC输出电流    */
    AN_NTC_RES,        /* NTC热敏电阻阻值检测 */
    AN_12V_VOLT,    /* 12V供电电压检测 */
    AN_USB_VOLT,    /* USB供电电压检测 */
#endif
/* 校准后的模拟量 */
#define REG03_AI_CH1            0x000C  /* 4字节浮点数, 示波器CH1通道电压值均值，单位V */
#define REG03_AI_CH2            0x000E  /* 4字节浮点数, 示波器CH2通道电压值均值，单位V */
#define REG03_AI_HIGH_SIDE_VOLT 0x0010  /* 4字节浮点数, 高端电流检测-电压值  均值，单位V */
#define REG03_AI_HIGH_SIDE_CURR 0x0012  /* 4字节浮点数, 高端电流检测-电流值  均值, 单位A */
#define REG03_AI_TVCC_VOLT      0x0014  /* 4字节浮点数, 目标板电压 均值, 单位V */
#define REG03_AI_TVCC_CURR      0x0016  /* 4字节浮点数, 目标板电压 均值, 单位V */
#define REG03_AI_NTC_RES        0x0018  /* NTC热敏电阻阻值检测, 单位欧姆 */
#define REG03_AI_NTC_TEMP       0x001A  /* 4字节浮点数 NTC温度（换算后） */
#define REG03_AI_USB_5V         0x001C  /* 4字节浮点数, USB 5V电压  均值, 单位V */
#define REG03_AI_EXT_POWER      0x001E  /* 4字节浮点数, 外部12V电源电压均值, 单位V */

/* 空挡预留 */

#define REG03_ADC_CH1               0x0040  /* 4字节浮点数, 示波器CH1通道 adc 均值 */
#define REG03_ADC_CH2               0x0042  /* 4字节浮点数, 示波器CH2通道 adc 均值 */
#define REG03_ADC_HIGH_SIDE_VOLT    0x0044  /* 4字节浮点数, 高端电流检测-电压值 adc 均值 */
#define REG03_ADC_HIGH_SIDE_CURR    0x0046  /* 4字节浮点数, 高端电流检测-电流值  adc 均值 */
#define REG03_ADC_TVCC_VOLT         0x0048  /* 4字节浮点数, 目标板电压 adc 均值 */
#define REG03_ADC_TVCC_CURR         0x004A  /* 4字节浮点数, 目标板电压 adc 均值 */
#define REG03_ADC_NTC_RES           0x004C  /* NTC热敏电阻阻值检测 adc 均值*/
#define REG03_ADC_USB_5V            0x004E  /* 4字节浮点数, USB 5V电压  adc 均值 */
#define REG03_ADC_EXT_POWER         0x0050  /* 4字节浮点数, 外部12V电源电压  adc 均值 */
#define REG03_ADC_PF4               0x0052  /* 保留单元 */

#define REG03_OUT_VOLT_DAC      0x0080  /* 2字节整数 设置输出电压的 DAC值 */
#define REG03_OUT_VOLT_MV       0x0081  /* 2字节整数 设置输出电压的 mV值 */

#define REG03_OUT_CURR_DAC      0x0082  /* 2字节整数 设置输出电流的 DAC值 */
#define REG03_OUT_CURR_UA       0x0083  /* 2字节整数 设置输出电流的 uA值 */

#define REG03_OUT_TVCC_DAC      0x0084  /* 2字节整数 设置TVCC电压的档位值（0-127） */
#define REG03_OUT_TVCC_MV       0x0085  /* 2字节整数 mV值 1260 - 5000mV */

/* 空挡预留 */

/* DAC输出波形控制 
    0：输出电平 
    1：输出正弦
    2：输出方波
    3：输出三角波
*/
#define REG03_DAC_WAVE_VOLT_RANGE   0x0100  /* 电压量程，保留不用。固定正负10V */
#define REG03_DAC_WAVE_TYPE         0x0101  /* DAC波形类型 */
#define REG03_DAC_WAVE_VOLT_MIN     0x0102  /* 波形最小电压，mV */
#define REG03_DAC_WAVE_VOLT_MAX     0x0103  /* 波形最大电压，mV */
#define REG03_DAC_WAVE_FREQ         0x0104  /* 32bit  波形频率，Hz */
#define REG03_DAC_WAVE_FREQ_Low     0x0105  /* 32bit  波形频率，Hz */
#define REG03_DAC_WAVE_DUTY         0x0106  /* 波形占空比 百分比 */
#define REG03_DAC_WAVE_COUNT_SET    0x0107  /* 32bit  波形个数设置 高16bit */
#define REG03_DAC_WAVE_COUNT_SET0   0x0108  /* 32bit  波形个数设置 低16bit */
#define REG03_DAC_WAVE_START        0x0109  /* DAC波形启动停止控制 */

/* 空挡保留未用 */

#define REG03_D0_GPIO_MODE          0x0140 /* DO口的GPIO模式 - 0=输入，1=输出 2=其他特殊功能 */
#define REG03_D1_GPIO_MODE          0x0141
#define REG03_D2_GPIO_MODE          0x0142
#define REG03_D3_GPIO_MODE          0x0143
#define REG03_D4_GPIO_MODE          0x0144
#define REG03_D5_GPIO_MODE          0x0145
#define REG03_D6_GPIO_MODE          0x0146
#define REG03_D7_GPIO_MODE          0x0147
#define REG03_D8_GPIO_MODE          0x0148
#define REG03_D9_GPIO_MODE          0x0149

#define REG03_D10_GPIO_MODE         0x014A /* TTL-TX */
#define REG03_D11_GPIO_MODE         0x014B /* TTL-RX */
#define REG03_D12_GPIO_MODE         0x014C /* CAN-TX */
#define REG03_D13_GPIO_MODE         0x014D /* CAN-RX */

/* 空挡保留未用 */

#define REG03_RTC_YEAR              0x0180 /* 设备时钟 */
#define REG03_RTC_MON               0x0181
#define REG03_RTC_DAY               0x0182
#define REG03_RTC_HOUR              0x0183
#define REG03_RTC_MIN               0x0184
#define REG03_RTC_SEC               0x0185

#define REG03_NTC_COEF_K            0x0186 /* NTC温度传感器温度修正公式 y = kx + b 之K值 浮点 缺省 = 1 */
#define REG03_NTC_COEF_B            0x0187 /* NTC温度传感器温度修正公式 y = kx + b 之B值 浮点 缺省 = 0 */

/* ADC 示波器控制 */
#define REG03_DSO_MEASURE_MODE      0x01FF  /* 1表示电流检测模式 */
#define REG03_CH1_DC                0x0200  /* CH1通道，AC/DC耦合切换 1表示DC */
#define REG03_CH2_DC                0x0201  /* CH2通道，AC/DC耦合切换 */
#define REG03_CH1_GAIN              0x0202  /* CH1通道，增益切换0-7 */
#define REG03_CH2_GAIN              0x0203  /* CH2通道，增益切换0-7 */
#define REG03_CH1_OFFSET            0x0204  /* CH1通道，直流偏值（0-2500mV 保留，不支持) */
#define REG03_CH2_OFFSET            0x0205  /* CH2通道，直流偏值（0-2500mV 保留，不支持) */
#define REG03_DSO_FREQ_ID           0x0206  /* 示波器采样频率档位  */
    #define DSO_FREQ_100            0
    #define DSO_FREQ_200            1
    #define DSO_FREQ_500            2
    #define DSO_FREQ_1K             3
    #define DSO_FREQ_2K             4
    #define DSO_FREQ_5K             5
    #define DSO_FREQ_10K            6
    #define DSO_FREQ_20K            7
    #define DSO_FREQ_50K            8
    #define DSO_FREQ_100K           9
    #define DSO_FREQ_200K           10
    #define DSO_FREQ_500K           11
    #define DSO_FREQ_1M             12
    #define DSO_FREQ_2M             13
    #define DSO_FREQ_5M             14
    #define DSO_FREQ_10M            15
    #define DSO_FREQ_20M            16

#define REG03_DSO_SAMPLE_SIZE 0x0207 /* 采样深度档位 */
    #define DSO_SIZE_1K             0
    #define DSO_SIZE_2K             1
    #define DSO_SIZE_4K             2
    #define DSO_SIZE_8K             3
    #define DSO_SIZE_16K            4
    #define DSO_SIZE_32K            5
    //    #define DSO_SIZE_64K      6        /* 后面暂时不支持 */
    //    #define DSO_SIZE_128K     7
    //    #define DSO_FREQ_512K     8
    #define DSO_SIZE_MAX            DSO_SIZE_32K

#define REG03_DSO_TRIG_LEVEL        0x0208  /* 触发电平，0-65535 */
#define REG03_DSO_TRIG_POS          0x0209  /* 触发位置 0-100 百分比 */
#define REG03_DSO_TRIG_MODE         0x020A  /* 触发模式 0=自动 1=普通 2=单次 */
#define REG03_DSO_TRIG_CHAN         0x020B  /* 触发通道 0=CH1  1=CH2*/
#define REG03_DSO_TRIG_EDGE         0x020C  /* 触发边沿 0下降 1上升 */
#define REG03_DSO_CHAN_EN           0x020D  /* 通道使能控制 bit0 = CH1  bit1 = CH2  bit2 = CH2 - CH1 */
#define REG03_DSO_RUN               0x020E  /* 示波器采集控制,0：停止 1: 启动 */

#define REG03_DSO_CURR_GAIN         0x0211  /* 0表示100mA, 1表示1A量程 */

#define REG03_WAVE_LOCK             0x021C  /* 波形锁定，等待读取 */

#define REG03_LUA_CMD               0x0300  /* LUA控制指令 */
#define REG03_LUA_STATE             0x0301  /* LUA程序状态 */

#define REG03_WORK_MODE             0x1000  /* 终端工作模式。1 ：常规工作模式。其他值：保留做其他应用 */
#define REG03_RS485_ADDR            0x1001  /* 本机RS485 MODBUS地址（1－254）*/
#define REG03_RS485_BAUD            0x1002  /* RS485波特率（0：1200 , 1：2400，2 : 4800，3 : 9600 , 4：19200 , 5：38400，6：57600，7：115200）*/
#define REG03_RS485_PARITY          0x1003  /* RS485奇偶校验位（0：无奇偶校验， 1：奇校验，2：偶校验）*/

#define REG03_IPAddr_Format         0x1100  /*以太网IP地址格式。IPv4或IPv6        0 表示IPv4        1 表示IPv6*/
#define REG03_DHCP 0x1101                   /*以太网DHCP设置。0表示禁止，1表示启用。*/
#define REG03_LocalIPAddr_H         0x1102  /*IPv4本机IP地址（静态地址），4字节。缺省 192.168.1.105*/
#define REG03_LocalIPAddr_L         0x1103  /*IPv4本机IP地址（静态地址），4字节。缺省 192.168.1.105*/
#define REG03_GatewayAddr_H         0x1104  /*IPv4网关地址，4字节。缺省192.168.1.1*/
#define REG03_GatewayAddr_L         0x1105  /*IPv4网关地址，4字节。缺省192.168.1.1*/
#define REG03_SubMask_H             0x1106  /*IPv4子网掩码，4字节。缺省255.255.255.0*/
#define REG03_SubMask_L             0x1107  /*IPv4子网掩码，4字节。缺省255.255.255.0*/
#define REG03_TCP_PORT              0x1108  /*本机TCP服务端口号，缺省值30010*/
#define REG03_MAC12                 0x1109  /*MAC地址1，2字节*/
#define REG03_MAC34                 0x110A  /*MAC地址3，4字节*/
#define REG03_MAC56                 0x110B  /*MAC地址5，6字节*/

/* 用于读取波形数据的寄存器入口 */
#define REG03_CH1_WAVE_0            0x2000  /* CH1波形数据 入口寄存器。 特殊处理，1K整数字节读取 */
#define REG03_CH1_WAVE_1            0x2001  /* CH1波形数据 第2段 1K字节 */
#define REG03_CH1_WAVE_2            0x2002  /* CH1波形数据 第3段 1K字节 */
#define REG03_CH1_WAVE_END          0x2FFF  /* CH1波形数据 第4096段 1K字节 */

#define REG03_CH2_WAVE_0            0x4000  /* CH2波形数据 入口寄存器。 特殊处理，1K整数字节读取  */
#define REG03_CH2_WAVE_END          0x4FFF  /* CH2波形数据 入口寄存器。 特殊处理，1K整数字节读取  */

/********************************** 校准参数 ***********************************/
/* 校准开关， 
    1      = 允许修改校准参数，
    0      = 禁止修改校准参数
    0x5AA5 = 初始化校准参数   InitCalibParam();初始化校准参数
     
*/
#define REG03_CALIB_KEY             0xBFFF

/* 输入模拟量校准 */
#define REG03_CALIB_CH1_R1_X1   0xC000 /* CH1 X1档位 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_CH1_R1_Y1   0xC002
#define REG03_CALIB_CH1_R1_X2   0xC004
#define REG03_CALIB_CH1_R1_Y2   0xC006
#define REG03_CALIB_CH1_R2_X1   0xC008 /* CH1 X2档位 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_CH1_R3_X1   0xC010 /* CH1 X4档位 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_CH1_R4_X1   0xC018 /* CH1 X8档位 校准参数 */
#define REG03_CALIB_CH1_R5_X1   0xC020 /* CH1 X16档位 校准参数 */
#define REG03_CALIB_CH1_R6_X1   0xC028 /* CH1 X32档位 校准参数 */
#define REG03_CALIB_CH1_R7_X1   0xC030 /* CH1 X64档位 校准参数 */
#define REG03_CALIB_CH1_R8_X1   0xC038 /* CH1 X128档位 校准参数 */

#define REG03_CALIB_CH2_R1_X1   0xC040 /* CH2 X1档位 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_CH2_R1_Y1   0xC042
#define REG03_CALIB_CH2_R1_X2   0xC044
#define REG03_CALIB_CH2_R1_Y2   0xC046
#define REG03_CALIB_CH2_R2_X1   0xC048 /* CH2 X2档位 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_CH2_R3_X1   0xC050 /* CH2 X4档位 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_CH2_R4_X1   0xC058 /* CH2 X8档位 校准参数 */
#define REG03_CALIB_CH2_R5_X1   0xC060 /* CH2 X16档位 校准参数 */
#define REG03_CALIB_CH2_R6_X1   0xC068 /* CH2 X32档位 校准参数 */
#define REG03_CALIB_CH2_R7_X1   0xC070 /* CH2 X64档位 校准参数 */
#define REG03_CALIB_CH2_R8_X1   0xC078 /* CH2 X128档位 校准参数 */

#define REG03_CALIB_LOAD_VOLT_X1 0xC080 /* 高端电流检测-电压值 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_LOAD_VOLT_Y1 0xC082
#define REG03_CALIB_LOAD_VOLT_X2 0xC084
#define REG03_CALIB_LOAD_VOLT_Y2 0xC086

#define REG03_CALIB_LOAD_CURR1_X1 0xC088 /* 高端电流检测(小量程)-电流值 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_LOAD_CURR1_Y1 0xC08A
#define REG03_CALIB_LOAD_CURR1_X2 0xC08C
#define REG03_CALIB_LOAD_CURR1_Y2 0xC08E
#define REG03_CALIB_LOAD_CURR1_X3 0xC090
#define REG03_CALIB_LOAD_CURR1_Y3 0xC092
#define REG03_CALIB_LOAD_CURR1_X4 0xC094
#define REG03_CALIB_LOAD_CURR1_Y4 0xC096

#define REG03_CALIB_LOAD_CURR2_X1 0xC098 /* 高端电流检测(小量程)-电流值 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_LOAD_CURR2_Y1 0xC09A
#define REG03_CALIB_LOAD_CURR2_X2 0xC09C
#define REG03_CALIB_LOAD_CURR2_Y2 0xC09E
#define REG03_CALIB_LOAD_CURR2_X3 0xC0A0
#define REG03_CALIB_LOAD_CURR2_Y3 0xC0A2
#define REG03_CALIB_LOAD_CURR2_X4 0xC0A4
#define REG03_CALIB_LOAD_CURR2_Y4 0xC0A6

#define REG03_CALIB_TVCC_VOLT_X1 0xC0A8 /* TVCC-电压值 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_TVCC_VOLT_Y1 0xC0AA
#define REG03_CALIB_TVCC_VOLT_X2 0xC0AC
#define REG03_CALIB_TVCC_VOLT_Y2 0xC0AE

#define REG03_CALIB_TVCC_CURR_X1 0xC0B0 /* TVCC-电流值 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_TVCC_CURR_Y1 0xC0B2
#define REG03_CALIB_TVCC_CURR_X2 0xC0B4
#define REG03_CALIB_TVCC_CURR_Y2 0xC0B6
#define REG03_CALIB_TVCC_CURR_X3 0xC0B8
#define REG03_CALIB_TVCC_CURR_Y3 0xC0BA
#define REG03_CALIB_TVCC_CURR_X4 0xC0BC
#define REG03_CALIB_TVCC_CURR_Y4 0xC0BE

/* 输出模拟量校准 */
#define REG03_CALIB_TVCC_SET_X1 0xC0C0 /* TVCC输出电压设定 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_TVCC_SET_Y1 0xC0C2
#define REG03_CALIB_TVCC_SET_X2 0xC0C4
#define REG03_CALIB_TVCC_SET_Y2 0xC0C6

#define REG03_CALIB_DAC_VOLT_X1 0xC0C8 /* DAC输出电压设定 校准参数，有符号整数 */
#define REG03_CALIB_DAC_VOLT_Y1 0xC0C9
#define REG03_CALIB_DAC_VOLT_X2 0xC0CA
#define REG03_CALIB_DAC_VOLT_Y2 0xC0CB
#define REG03_CALIB_DAC_VOLT_X3 0xC0CC
#define REG03_CALIB_DAC_VOLT_Y3 0xC0CD
#define REG03_CALIB_DAC_VOLT_X4 0xC0CE
#define REG03_CALIB_DAC_VOLT_Y4 0xC0CF

#define REG03_CALIB_DAC_CURR_X1 0xC0D0 /* DAC输出电压设定 校准参数，有符号整数  */
#define REG03_CALIB_DAC_CURR_Y1 0xC0D1
#define REG03_CALIB_DAC_CURR_X2 0xC0D2
#define REG03_CALIB_DAC_CURR_Y2 0xC0D3
#define REG03_CALIB_DAC_CURR_X3 0xC0D4
#define REG03_CALIB_DAC_CURR_Y3 0xC0D5
#define REG03_CALIB_DAC_CURR_X4 0xC0D6
#define REG03_CALIB_DAC_CURR_Y4 0xC0D7

#define REG03_CALIB_NTC_RES_X1  0xC0D8 /* NTC电阻 校准参数，X1 Y1 X2 Y2 均为浮点数. 每个浮点数占用2个寄存器 */
#define REG03_CALIB_NTC_RES_Y1  0xC0DA
#define REG03_CALIB_NTC_RES_X2  0xC0DC
#define REG03_CALIB_NTC_RES_Y2  0xC0DE
#define REG03_CALIB_NTC_RES_X3  0xC0E0
#define REG03_CALIB_NTC_RES_Y3  0xC0E2
#define REG03_CALIB_NTC_RES_X4  0xC0E4
#define REG03_CALIB_NTC_RES_Y4  0xC0E6

#define REG03_CALIB_PARAM_END REG03_CALIB_NTC_RES_Y4 + 1 /* 校准参数如最后1个寄存器 */

#define REG03_BOOT_VER          0xE000  /* BOOT 固件版本，0x0102 表示V1.02 */

/* 重映射部分寄存器，专用于UDP网路搜索 */
#define REG03_NET_CPU_ID0       0xFF00
#define REG03_NET_CPU_ID1       0xFF01
#define REG03_NET_CPU_ID2       0xFF02
#define REG03_NET_CPU_ID3       0xFF03
#define REG03_NET_CPU_ID4       0xFF04
#define REG03_NET_CPU_ID5       0xFF05
#define REG03_NET_HARD_VER      0xFF06  /* 硬件型号（只读），内部型号代码，非型号全称，0xC200 表示TC200*/
#define REG03_NET_SOFT_VER      0xFF07  /* 硬件版本（只读），如0x0100，表示H1.00 */
#define REG03_NET_MAC12         0xFF08  /* MAC */
#define REG03_NET_MAC34         0xFF09  /* MAC */
#define REG03_NET_MAC56         0xFF0A  /* MAC */
#define REG03_NET_LOCAL_IP_H    0xFF0B  /* IPv4本机IP地址（静态地址），高2字节 */
#define REG03_NET_LOCAL_IP_L    0xFF0C  /* IPv4本机IP地址（静态地址），低2字节 */
#define REG03_NET_NETMASK_H     0xFF0D  /* IPv4子网掩码，高2字节。 */
#define REG03_NET_NETMASK_L     0xFF0E  /* IPv4子网掩码，低2字节。 */
#define REG03_NET_GATEWAY_H     0xFF0F  /* IPv4网关地址，高2字节。 */
#define REG03_NET_GATEWAY_L     0xFF10  /* IPv4网关地址，第2字节。 */
#define REG03_NET_LOCAL_PORT    0xFF11  /* 本地端口 */
#define REG03_NET_SYSTEM_IP_H   0xFF12  /* 管理系统IP地址高位 */
#define REG03_NET_SYSTEM_IP_L   0xFF13  /* 管理系统IP地址低位  */
#define REG03_NET_SYSTEM_PORT   0xFF14  /* 远程服务器端口 */


#define REG03_SEND_KEY          0xFF7E  /* 特殊寄存器，模拟一个按键操作 */


#define REG03_RESET_TO_BOOT     0xFF7F  /* 特殊寄存器，复位进入BOOT
    1 - 复位进入APP
    2 - 复位进入BOOT
    3 - 进入DAP
    4 - 进入U盘模式
    5 - 进入APP联机界面00
*/
    #define JUMP_TO_APP    1
    #define JUMP_TO_BOOT   2
    #define JUMP_TO_DAP    3
    #define JUMP_TO_EMMC   4
    #define JUMP_TO_LINK          5
    #define JUMP_TO_UPDATE_BOOT   6     /* 更新boot */
    

/* BOOT 程序用的寄存器 */
#define REG03_BOOT_CPU_ID0      0xFF80
#define REG03_BOOT_CPU_ID1      0xFF81
#define REG03_BOOT_CPU_ID2      0xFF82
#define REG03_BOOT_CPU_ID3      0xFF83
#define REG03_BOOT_CPU_ID4      0xFF84
#define REG03_BOOT_CPU_ID5      0xFF85
#define REG03_BOOT_HARD_VER     0xFF86  /* 硬件型号（只读），内部型号代码，非型号全称，0xC200 表示TC200*/
#define REG03_BOOT_SOFT_VER     0xFF87  /* 硬件版本（只读），如0x0100，表示H1.00 */

#define REG03_BOOT_PROG_TYPE    0xFF88  /* APP程序类型: 0 = CPU内部Flash ; 1 = QSPI Flash */
#define REG03_BOOT_PROG_ADDR    0xFF89  /* APP程序地址 32位 */
#define REG03_BOOT_PROG_SIZE    0xFF8B  /* APP程序长度 32位 */

/* 系统控制参数  
    1 - 通知开始升级，设备开始擦除flash. 根据前面2个寄存器决定擦除空间 
    2 - 通知设备程序下载完毕
    3 - 通知系统复位
    
    4 - 进入U盘模式
    5 - 退出U盘模式（需要RJ45）
*/
#define REG03_BOOT_SYSTEM       0xFF8D

#define REG03_BOOT_STATUS       0xFFA0  /* 设备状态寄存器  0空闲，1表示正忙 */
#define REG03_BOOT_CRC32_HIGH   0xFFA1  /* 程序区CRC32校验 - 未用 */
#define REG03_BOOT_CRC32_LOW    0xFFA2  /* 程序区CRC32校验 - 未用 */

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
