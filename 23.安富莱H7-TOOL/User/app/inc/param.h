/*
*********************************************************************************************************
*
*    模块名称 : 应用程序参数模块
*    文件名称 : param.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2012-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __PARAM_H
#define __PARAM_H

#define PARAM_ADDR          0           /* 基本参数区地址 */
#define PARAM_SIZE          256         /* 最大空间，用于编译查错 */

#define PARAM_CALIB_ADDR    1024        /* 校准参数区地址 */
#define PARAM_CALIB_SIZE    512         /* 最大空间，用于编译查错 */

#define PARAM_VER           0x00000101  /* 基本参数版本 100 */

#define CALIB_VER           0x00000201  /* 校准参数版本 200 */

/* 暂未启用 程序缓存 */
#define APP_BUF_ADDR        0x08000000 + 1 * 1024 * 1024

/* 校准参数结构，两点校准， 通用校准参数，ADC */
typedef struct
{
    float x1;
    float y1;
    float x2;
    float y2;
} AN_CALIB_T;

/* 校准参数结构，多点校准，解决DAC电路非线性不好的问题 */
typedef struct
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    int16_t x3;
    int16_t y3;
    int16_t x4;
    int16_t y4;
} AN_CALIB_DAC_T;

/* 校准参数结构，多点校准，解决电流非线性不好的问题 */
typedef struct
{
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float x4;
    float y4;
} AN_CALIB_ADC_T;

/* 全局参数 */
typedef struct
{
    uint32_t UpgradeFlag;       /*升级标记,0x55AAA55A表示需要更新APP，0xFFFF表示更新完毕*/
    uint32_t ParamVer;          /* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */

    uint8_t DispDir;            /* 显示方向 */

    uint8_t Addr485;

    uint8_t LocalIPAddr[4];     /* 本机IP地址 */
    uint8_t NetMask[4];         /* 子网掩码 */
    uint8_t Gateway[4];         /* 网关 */
    uint16_t LocalTCPPort;      /* 本机TCP端口 */
    uint16_t LocalUDPPort;      /* 本机UDP端口 */

    uint8_t RemoteIPAddr[4];    /* 远端(前置）IP地址 */
    uint16_t RemoteTcpPort;     /* 远端（前置）TCP端口 */

    uint8_t WorkMode;           /* 工作模式 保留 */

    uint8_t APSelfEn;           /* 本机扮演AP */
    uint8_t AP_SSID[32 + 1];    /* AP名字 */
    uint8_t AP_PASS[16 + 1];    /* AP密码 */
    uint8_t WiFiIPAddr[4];      /* IP地址  192.168.1.50 */
    uint8_t WiFiNetMask[4];     /* 子网掩码 255.255.255.0 */
    uint8_t WiFiGateway[4];     /* 网关 192.168.1.1 */
    uint8_t DHCPEn;             /* DHCP使能  */
    uint8_t WiFiMac[6];

    uint32_t TestWord;          /*　测试单元，用于检测eepromg功能 */
    uint8_t NtcType;            /* NTC热敏电阻类型 0 = 10K_B3950，1 = 100K_B3950 */
    
    uint8_t KeyToneEnable;
    uint8_t UIStyle;            /* UI风格 */
    uint16_t LcdSleepTime;      /* 屏保时间 */
    
    uint8_t FileListFont24;     /* 1表示24点阵显示文件列表，0表示16点阵 */
    
    uint8_t ResetTypeNotUsed;	/* 0表示由lua脚本决定  1表示强制硬件复位 2表示强制软件复位 */
    uint8_t MultiProgMode;      /* 1表示1路，2表示2路，3表示3路，4表示4路 */
    
	uint16_t FactoryId;         /* 工厂代码 */
    uint16_t ToolSn;          	/* 烧录器编号 */
    
    uint8_t StartRun;           /* 开机启动，0表示缺省，1表示单机烧录 2表示多路烧录 */

    /* V1.32 */
    uint8_t UartMonBaud;        /* 串口监视，波特率 */
    uint8_t UartMonParit;       /* 串口监视，奇偶校验 */
    uint8_t UartMonWordWrap;    /* 串口监视，自动换行 */    
    uint8_t UartMonFont;        /* 串口监视，字体 */
    uint8_t UartMonHex;         /* 串口监视，按HEX显示 */
    uint8_t UartMonTimeStamp;   /* 串口监视，加上时间戳 */
    uint8_t UartMonProxy;       /* 串口监视，协议 */
    
    /* DAP-Link相关参数 */
    uint8_t DAP_TVCCVolt;       /* DAP=Link TVCC设置 0-关闭  0-50 表示 0-5.0V */
    uint8_t DAP_BeepEn;         /* DAP=Link 提示音使能 0:关闭 1:烧录完毕鸣叫 */
    uint8_t DAP_Sn;             /* DAP-Link 序号，用于驱动识别: */
} PARAM_T;

/* 模拟量校准参数 */
typedef struct
{
    uint32_t CalibVer;          /* 校准参数版本. 用于升级 */
    AN_CALIB_T CH1[8];          /* CH1 示波器通道ADC校准参数, 对应8档硬件增益 */
    AN_CALIB_T CH2[8];          /* CH1 示波器通道ADC校准参数, 对应8档硬件增益 */

    AN_CALIB_T LoadVolt;        /* 负载电压校准参数 */
    AN_CALIB_ADC_T LoadCurr[2]; /* 负载电流校准参数，2个量程  */

    AN_CALIB_T TVCCVolt;        /* TVCC监视电压 */
    AN_CALIB_ADC_T TVCCCurr;    /* TVCC电流 */

    AN_CALIB_ADC_T NtcRes;      /* NTC测温接口参考电阻阻值 */

    AN_CALIB_T TVCCSet;         /* TVCC输出设置电压 */

    AN_CALIB_DAC_T Dac10V;      /* DAC电压通道校准参数，正负10V */
    AN_CALIB_DAC_T Dac20mA;     /* DAC电流通道校准参数，20mA */

    AN_CALIB_T USBVolt;         /* USB供电电压 */
    AN_CALIB_T ExtPowerVolt;    /* 外部供电电压 */
} CALIB_T;

/* 全局变量 */
typedef struct
{
    /* MCU ID */
    uint32_t CPU_Sn[3];

    uint8_t WiFiDebugEn;
    uint8_t RemoteTCPServerOk;
    uint8_t HomeWiFiLinkOk;

    uint8_t WiFiRecivedIPD;

    uint8_t MACaddr[6];         /* 以太网MAC地址 */

    uint8_t InputState[32];
    uint8_t RelayState[32];

    float CH1Volt;
    float CH2Volt;
    float HighSideVolt;
    float HighSideCurr;
    float USBPowerVolt;
    float ExtPowerVolt;
    float TVCCVolt;             /* TVCC实测电压 */
    float TVCCCurr;             /* TVCC实测电压 */
    float NTCRes;               /* NTC电阻 */
    float NTCTemp;              /* NTC温度 */

    float ADC_CH1Volt;
    float ADC_CH2Volt;
    float ADC_HighSideVolt;
    float ADC_HighSideCurr;
    float ADC_USBPowerVolt;
    float ADC_ExtPowerVolt;
    float ADC_TVCCVolt;         /* TVCC实测电压 */
    float ADC_TVCCCurr;         /* TVCC实测电压 */
    float ADC_NTCRes;           /* NTC电阻 */
    
    float BatteryCapacity;      /* 电池容量累计 */
    uint8_t StartBatCap;        /* 开始累计容量 */

    uint16_t OutTVCCDac;        /* 输出TVCC dac值 */
    uint16_t OutTVCCmV;         /* 输出TVCC mV值 */

    uint16_t OutVoltDAC;        /* 输出电压 dac值 */
    int16_t OutVoltmV;          /* 输出电压 mV值 有符号数 支持负电压*/

    uint16_t OutCurrDAC;        /* 输出电流 dac值 */
    uint16_t OutCurruA;         /* 输出电流 uA值 */

    uint8_t LuaRunOnce;         /* 用于Modbus通信程序延迟执行lua */

    uint8_t CalibEnable;

    uint8_t GpioMode[16];       /* 保存GPIO模式参数、输入、输出或特殊功能 */

    uint8_t SaveAlgoToCFile;    /* 解析FLM时，保存解析结果到C文件，仅仅用于debug */

    uint8_t ReqWriteBoot;       /* 用于PC机通知刷新boot */     
    
    uint8_t ReqJump;            /* 请求跳转 */ 
    uint8_t UsbEmmcRemoved;     /* U盘移除 */
    
    uint8_t DelayOpenUSB;       /* 延迟启动USB */
    
    uint8_t LinkState;          /* 和PC机联机状态 */
    int32_t LinkTime;
    
    uint8_t PC_SKeyState;       /* PC控制的S键状态 */
    uint8_t PC_CKeyState;       /* PC控制的C键状态 */
    uint8_t PC_KeyEnabled;      /* PC控制按键进行中... */
} VAR_T;

enum
{
    LINK_NONE = 0,      /* 无连接 */
    LINK_USB_OK = 1,    /* USB连接中 */
    LINK_RJ45_OK = 2,   /* RJ45连接中 */
    LINK_WIFI_OK = 3,   /* WiFi连接中 */
};

extern PARAM_T g_tParam;
extern CALIB_T g_tCalib;
extern VAR_T g_tVar;

void LoadParam(void);
void SaveParam(void);
void InitCalibParam(void);
void InitBaseParam(void);
void WriteParamUint16(uint16_t _addr, uint16_t _value);
void SaveCalibParam(void);

uint16_t GetSleepTimeMinute(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
