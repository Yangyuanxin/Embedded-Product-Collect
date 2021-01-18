/*
*********************************************************************************************************
*
*    模块名称 : MODBUS从机模块
*    文件名称 : tcp_MODS_slave.c
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "Includes.h"

uint8_t fSaveReq_06H = 0;       /* 保存基本参数请求，用于06H和10H写寄存器函数 */
uint8_t fResetReq_06H = 0;      /* 需要复位CPU，因为网络参数变化 */
uint8_t fSaveCalibParam = 0;    /* 保存校准参数请求，用于06H和10H写寄存器函数 */
uint8_t fDisableAck = 0;          /* 强制无需应答，PC发虚拟按键时用到 */

static uint8_t WriteCalibRegValue_06H(uint16_t _reg_addr, uint16_t _reg_value);
static uint8_t ReadCalibRegValue_03H(uint16_t reg_addr, uint16_t *reg_value);

/*
*********************************************************************************************************
*    函 数 名: ReadRegValue_03H
*    功能说明: 读取保持寄存器的值
*    形    参: reg_addr 寄存器地址
*              reg_value 存放寄存器结果
*    返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
uint8_t ReadRegValue_03H(uint16_t reg_addr, uint16_t *reg_value)
{
    uint16_t value;

    /* modbus为大端模式，所以先传高16位，后传低16位 */
    switch (reg_addr)
    {
        case REG03_NET_CPU_ID0: /* MCU序号（只读） */
        case REG03_DEV_ID0:
        case REG03_BOOT_CPU_ID0:           
            value = g_tVar.CPU_Sn[0];
            break;

        case REG03_NET_CPU_ID1:
        case REG03_DEV_ID1:
        case REG03_BOOT_CPU_ID1:
            value = g_tVar.CPU_Sn[0] >> 16;
            break;

        case REG03_NET_CPU_ID2:
        case REG03_DEV_ID2:
        case REG03_BOOT_CPU_ID2:
            value = g_tVar.CPU_Sn[1];
            break;

        case REG03_NET_CPU_ID3:
        case REG03_DEV_ID3:
        case REG03_BOOT_CPU_ID3:
            value = g_tVar.CPU_Sn[1] >> 16;
            break;

        case REG03_NET_CPU_ID4:
        case REG03_DEV_ID4:
        case REG03_BOOT_CPU_ID4:            
            value = g_tVar.CPU_Sn[2];
            break;

        case REG03_NET_CPU_ID5:
        case REG03_DEV_ID5:
        case REG03_BOOT_CPU_ID5:            
            value = g_tVar.CPU_Sn[2] >> 16;
            break;

        case REG03_NET_HARD_VER:
        case REG03_HARD_MODEL:
        case REG03_BOOT_HARD_VER:
            value = HARD_MODEL;
            break;

        case REG03_BOOT_VER:
        case REG03_BOOT_SOFT_VER:
            value = BOOT_VERSION;
            break;

        case REG03_NET_SOFT_VER:
        case REG03_APP_VER:
            value = APP_VERSION;
            break;

        case REG03_X15_00:
        {
    #if 1
            value = EIO_READ_FMC();
    #else
            uint8_t state;
            uint8_t i;

            value = 0;
            for (i = 0; i < 16; i++)
            {
                MODS_GetDIState(i, &state);
                if (state == 1)
                {
                    value |= (1 << i);
                }
            }
    #endif
        }
        break;

        case REG03_X31_16:
        {
            uint8_t state;
            uint8_t i;

            value = 0;
            for (i = 0; i < 16; i++)
            {
                MODS_GetDIState(i + 16, &state);
                if (state == 1)
                {
                    value |= (1 << i);
                }
            }
        }
        break;

        case REG03_Y15_00:
        {
            uint8_t state;
            uint8_t i;

            value = 0;
            for (i = 0; i < 16; i++)
            {
                MODS_GetDOState(i, &state);
                if (state == 1)
                {
                    value |= (1 << i);
                }
            }
        }
        break;

        case REG03_Y31_16:
        {
            uint8_t state;
            uint8_t i;

            value = 0;
            for (i = 0; i < 16; i++)
            {
                MODS_GetDOState(i + 16, &state);
                if (state == 1)
                {
                    value |= (1 << i);
                }
            }
        }
        break;

        /* 4字节浮点数, 示波器CH1通道电压值均值 */
        case REG03_AI_CH1:
            value = GetHigh16OfFloat(g_tVar.CH1Volt);
            break;
        case REG03_AI_CH1 + 1:
            value = GetLow16OfFloat(g_tVar.CH1Volt);
            break;

        /* 4字节浮点数, 示波器CH2通道电压值均值 */
        case REG03_AI_CH2:
            value = GetHigh16OfFloat(g_tVar.CH2Volt);
            break;
        case REG03_AI_CH2 + 1:
            value = GetLow16OfFloat(g_tVar.CH2Volt);
            break;

        /* 4字节浮点数, 高端电流检测-电压值  均值, mV单位 */
        case REG03_AI_HIGH_SIDE_VOLT:
            value = GetHigh16OfFloat(g_tVar.HighSideVolt);
            break;
        case REG03_AI_HIGH_SIDE_VOLT + 1:
            value = GetLow16OfFloat(g_tVar.HighSideVolt);
            break;

        /* 4字节浮点数, 高端电流检测-电流值  均值, mV单位 */
        case REG03_AI_HIGH_SIDE_CURR:
            value = GetHigh16OfFloat(g_tVar.HighSideCurr);
            break;
        case REG03_AI_HIGH_SIDE_CURR + 1:
            value = GetLow16OfFloat(g_tVar.HighSideCurr);
            break;

        /* 4字节浮点数, 目标板电压 均值, mV单位 */
        case REG03_AI_TVCC_VOLT:
            value = GetHigh16OfFloat(g_tVar.TVCCVolt);
            break;
        case REG03_AI_TVCC_VOLT + 1:
            value = GetLow16OfFloat(g_tVar.TVCCVolt);
            break;

        /* 4字节浮点数, 目标板电压 均值, mV单位 */
        case REG03_AI_TVCC_CURR:
            value = GetHigh16OfFloat(g_tVar.TVCCCurr);
            break;
        case REG03_AI_TVCC_CURR + 1:
            value = GetLow16OfFloat(g_tVar.TVCCCurr);
            break;

        /* 4字节浮点数, NTC电阻，欧姆 */
        case REG03_AI_NTC_RES:
            value = GetHigh16OfFloat(g_tVar.NTCRes);
            break;
        case REG03_AI_NTC_RES + 1:
            value = GetLow16OfFloat(g_tVar.NTCRes);
            break;

        /* 4字节浮点数, NTC电阻，欧姆 */
        case REG03_AI_NTC_TEMP:
            value = GetHigh16OfFloat(g_tVar.NTCTemp);
            break;
        case REG03_AI_NTC_TEMP + 1:
            value = GetLow16OfFloat(g_tVar.NTCTemp);
            break;

        /* 4字节浮点数, USB 5V电压  均值, mV单位 */
        case REG03_AI_USB_5V:
            value = GetHigh16OfFloat(g_tVar.USBPowerVolt);
            break;
        case REG03_AI_USB_5V + 1:
            value = GetLow16OfFloat(g_tVar.USBPowerVolt);
            break;

        /* 4字节浮点数, 外部12V电源电压  均值, mV单位 */
        case REG03_AI_EXT_POWER:
            value = GetHigh16OfFloat(g_tVar.ExtPowerVolt);
            break;
        case REG03_AI_EXT_POWER + 1:
            value = GetLow16OfFloat(g_tVar.ExtPowerVolt);
            break;

        /* ADC值 */
        case REG03_ADC_CH1:
            value = GetHigh16OfFloat(g_tVar.ADC_CH1Volt);
            break;
        case REG03_ADC_CH1 + 1:
            value = GetLow16OfFloat(g_tVar.ADC_CH1Volt);
            break;

        case REG03_ADC_CH2:
            value = GetHigh16OfFloat(g_tVar.ADC_CH2Volt);
            break;
        case REG03_ADC_CH2 + 1:
            value = GetLow16OfFloat(g_tVar.ADC_CH2Volt);
            break;

        case REG03_ADC_HIGH_SIDE_VOLT:
            value = GetHigh16OfFloat(g_tVar.ADC_HighSideVolt);
            break;
        case REG03_ADC_HIGH_SIDE_VOLT + 1:
            value = GetLow16OfFloat(g_tVar.ADC_HighSideVolt);
            break;

        case REG03_ADC_HIGH_SIDE_CURR:
            value = GetHigh16OfFloat(g_tVar.ADC_HighSideCurr);
            break;
        case REG03_ADC_HIGH_SIDE_CURR + 1:
            value = GetLow16OfFloat(g_tVar.ADC_HighSideCurr);
            break;

        case REG03_ADC_TVCC_VOLT:
            value = GetHigh16OfFloat(g_tVar.ADC_TVCCVolt);
            break;
        case REG03_ADC_TVCC_VOLT + 1:
            value = GetLow16OfFloat(g_tVar.ADC_TVCCVolt);
            break;

        case REG03_ADC_TVCC_CURR:
            value = GetHigh16OfFloat(g_tVar.ADC_TVCCCurr);
            break;
        case REG03_ADC_TVCC_CURR + 1:
            value = GetLow16OfFloat(g_tVar.ADC_TVCCCurr);
            break;

        case REG03_ADC_NTC_RES:
            value = GetHigh16OfFloat(g_tVar.ADC_NTCRes);
            break;
        case REG03_ADC_NTC_RES + 1:
            value = GetLow16OfFloat(g_tVar.ADC_NTCRes);
            break;

        case REG03_ADC_USB_5V:
            value = GetHigh16OfFloat(g_tVar.ADC_USBPowerVolt);
            break;
        case REG03_ADC_USB_5V + 1:
            value = GetLow16OfFloat(g_tVar.ADC_USBPowerVolt);
            break;

        case REG03_ADC_EXT_POWER:
            value = GetHigh16OfFloat(g_tVar.ADC_ExtPowerVolt);
            break;
        case REG03_ADC_EXT_POWER + 1:
            value = GetLow16OfFloat(g_tVar.ADC_ExtPowerVolt);
            break;

        /********** DAC输出电压电流， TVCC *********************/
        case REG03_OUT_VOLT_DAC: /* 2字节整数 设置输出电压的 DAC值 */
            value = g_tVar.OutVoltDAC;
            break;

        case REG03_OUT_VOLT_MV: /* 2字节整数 设置输出电压的 mV值 */
            value = g_tVar.OutVoltmV;
            break;

        case REG03_OUT_CURR_DAC: /* 2字节整数 设置输出电流的 DAC值 */
            value = g_tVar.OutCurrDAC;
            break;

        case REG03_OUT_CURR_UA: /* 2字节整数 设置输出电流的 uA值 */
            value = g_tVar.OutCurruA;
            break;

        case REG03_OUT_TVCC_DAC: /* 2字节整数 设置TVCC电压的档位值（0-127） */
            value = g_tVar.OutTVCCDac;
            break;

        case REG03_OUT_TVCC_MV: /* 2字节整数 mV值 1260 - 5000mV */
            value = g_tVar.OutTVCCmV;
            break;

        /************* DAC 波形 *************************/
        case REG03_DAC_WAVE_START: /* DAC波形启动停止控制 */
            value = g_tDacWave.Run;
            break;

        case REG03_DAC_WAVE_VOLT_RANGE: /* 电压量程，0-10V,正负10V */
            value = g_tDacWave.VoltRange;
            break;

        case REG03_DAC_WAVE_TYPE: /* DAC波形类型 */
            value = g_tDacWave.Type;
            break;

        case REG03_DAC_WAVE_VOLT_MAX: /* 波形最大电压，mV */
            value = g_tDacWave.VoltMax;
            break;

        case REG03_DAC_WAVE_VOLT_MIN: /* 波形最小电压，mV */
            value = g_tDacWave.VoltMin;
            break;

        case REG03_DAC_WAVE_FREQ: /* 32位 波形频率高16bit，Hz */
            value = g_tDacWave.Freq >> 16;
            break;

        case REG03_DAC_WAVE_FREQ + 1: /* 32位 波形频率低16bit，Hz */
            value = g_tDacWave.Freq;
            break;

        case REG03_DAC_WAVE_DUTY: /* 波形占空比 百分比 */
            value = g_tDacWave.Duty;
            break;

        case REG03_DAC_WAVE_COUNT_SET: /* 32位 波形个数设置 */
            value = g_tDacWave.CycleSetting >> 16;
            break;

        case REG03_DAC_WAVE_COUNT_SET + 1: /* 32位 波形个数设置 */
            value = g_tDacWave.CycleSetting;
            break;

        /******************** ADC参数 ****************************/
        case REG03_CH1_DC: /* CH1通道，AC/DC耦合切换 1表示DC */
            value = g_tDSO.DC1;
            break;

        case REG03_CH2_DC: /* CH2通道，AC/DC耦合切换 */
            value = g_tDSO.DC2;
            break;

        case REG03_CH1_GAIN: /* CH1通道，增益切换0-3 */
            value = g_tDSO.Gain1;
            break;

        case REG03_CH2_GAIN: /* CH2通道，增益切换0-3 */
            value = g_tDSO.Gain2;
            break;

        case REG03_CH1_OFFSET: /* CH1通道，偏置 */
            value = g_tDSO.Offset1;
            break;

        case REG03_CH2_OFFSET: /* CH2通道，偏置 */
            value = g_tDSO.Offset2;
            break;

        case REG03_DSO_FREQ_ID: /* 示波器采样频率档位  */
            value = g_tDSO.FreqID;
            break;

        case REG03_DSO_SAMPLE_SIZE: /* 采样深度档位 */
            value = g_tDSO.SampleSizeID;
            break;

        case REG03_DSO_TRIG_LEVEL: /* 触发电平，*/
            value = g_tDSO.TrigLevel;
            break;

        case REG03_DSO_TRIG_POS: /* 触发位置 */
            value = g_tDSO.TrigPos;
            break;

        case REG03_DSO_TRIG_MODE: /* 触发模式 0=自动 1=普通 */
            value = g_tDSO.TrigMode;
            break;

        case REG03_DSO_TRIG_CHAN: /* 触发通道 0=CH1  1=CH2*/
            value = g_tDSO.TrigChan;
            break;

        case REG03_DSO_TRIG_EDGE: /* 触发边沿 0=下降沿 1=上上升沿 */
            value = g_tDSO.TrigEdge;
            break;

        case REG03_DSO_CHAN_EN: /* 通道使能控制 bit0 = CH1  bit1 = CH2  bit2 = CH2 - CH1 */
            value = g_tDSO.ChanEnable;
            break;

        case REG03_DSO_RUN: /* 示波器采集控制,0：停止 1：CH1，2：CH2，3：CH1+CH2 */
            value = g_tDSO.Run;
            break;

        case REG03_DSO_MEASURE_MODE: /* 电流检测模式 */
            value = g_tDSO.MeasuteMode;
            break;

        case REG03_DSO_CURR_GAIN: /* 电流增益 */
            value = g_tDSO.CurrGain;
            break;

            /********************************** 网络参数 ***********************************/

        case REG03_NET_LOCAL_IP_H:
        case REG03_LocalIPAddr_H:
            value = (g_tParam.LocalIPAddr[0] << 8) + g_tParam.LocalIPAddr[1];
            break;

        case REG03_NET_LOCAL_IP_L:
        case REG03_LocalIPAddr_L:
            value = (g_tParam.LocalIPAddr[2] << 8) + g_tParam.LocalIPAddr[3];
            break;

        case REG03_NET_GATEWAY_H:
        case REG03_GatewayAddr_H:
            value = (g_tParam.Gateway[0] << 8) + g_tParam.Gateway[1];
            break;

        case REG03_NET_GATEWAY_L:
        case REG03_GatewayAddr_L:
            value = (g_tParam.Gateway[2] << 8) + g_tParam.Gateway[3];
            break;

        case REG03_NET_NETMASK_H:
        case REG03_SubMask_H:
            value = (g_tParam.NetMask[0] << 8) + g_tParam.NetMask[1];
            break;

        case REG03_NET_NETMASK_L:
        case REG03_SubMask_L:
            value = (g_tParam.NetMask[2] << 8) + g_tParam.NetMask[3];
            break;

        case REG03_NET_LOCAL_PORT:
        case REG03_TCP_PORT:
            value = g_tParam.LocalTCPPort;
            break;

        case REG03_NET_MAC12:
        case REG03_MAC12:
            value = (g_tVar.MACaddr[0] << 8) + g_tVar.MACaddr[1];
            break;

        case REG03_NET_MAC34:
        case REG03_MAC34:
            value = (g_tVar.MACaddr[2] << 8) + g_tVar.MACaddr[3];
            break;

        case REG03_NET_MAC56:
        case REG03_MAC56:
            value = (g_tVar.MACaddr[4] << 8) + g_tVar.MACaddr[5];
            break;

        case REG03_NET_SYSTEM_IP_H:
            value = (g_tParam.RemoteIPAddr[0] << 8) + g_tParam.RemoteIPAddr[1];
            break;

        case REG03_NET_SYSTEM_IP_L:
            value = (g_tParam.RemoteIPAddr[2] << 8) + g_tParam.RemoteIPAddr[3];
            break;

        case REG03_NET_SYSTEM_PORT:
            value = g_tParam.RemoteTcpPort;
            break;

        /**************************** GPIO控制 *****************************/
        case REG03_D0_GPIO_MODE: /* 配置D0-D9  GPIO模式 */
        case REG03_D1_GPIO_MODE:
        case REG03_D2_GPIO_MODE:
        case REG03_D3_GPIO_MODE:
        case REG03_D4_GPIO_MODE:
        case REG03_D5_GPIO_MODE:
        case REG03_D6_GPIO_MODE:
        case REG03_D7_GPIO_MODE:
        case REG03_D8_GPIO_MODE:
        case REG03_D9_GPIO_MODE:
        case REG03_D10_GPIO_MODE:
        case REG03_D11_GPIO_MODE:
        case REG03_D12_GPIO_MODE:
        case REG03_D13_GPIO_MODE:
        {
            uint8_t idx;

            idx = reg_addr - REG03_D0_GPIO_MODE;
            value = g_tVar.GpioMode[idx];
        }
        break;

            /**************************** 时钟参数 *****************************/

        case REG03_RTC_YEAR:
            RTC_ReadClock();
            value = g_tRTC.Year;
            break;

        case REG03_RTC_MON:
            RTC_ReadClock();
            value = g_tRTC.Year;
            break;

        case REG03_RTC_DAY:
            RTC_ReadClock();
            value = g_tRTC.Day;
            break;

        case REG03_RTC_HOUR:
            RTC_ReadClock();
            value = g_tRTC.Hour;
            break;
            ;

        case REG03_RTC_MIN:
            RTC_ReadClock();
            value = g_tRTC.Min;
            break;

        case REG03_RTC_SEC:
            RTC_ReadClock();
            value = g_tRTC.Sec;
            break;

        default:
            if (reg_addr >= REG03_CALIB_CH1_R1_X1 && reg_addr <= REG03_CALIB_PARAM_END)
            {
                /* 写校准参数 */
                ReadCalibRegValue_03H(reg_addr, reg_value);
                return 1;
            }
            else
            {
                return 0; /* 参数异常，返回 0 */
            }
    }

    *reg_value = value;
    return 1; /* 读取成功 */
}

/*
*********************************************************************************************************
*    函 数 名: ReadCalibRegValue_03H
*    功能说明: 读校准参数寄存器 
*    形    参: _reg_addr 寄存器地址
*              _reg_value 寄存器值
*    返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t ReadCalibRegValue_03H(uint16_t _reg_addr, uint16_t *_reg_value)
{
    uint16_t value;

    switch (_reg_addr)
    {
        /* 输入模拟量校准参数 */
        case REG03_CALIB_LOAD_VOLT_X1:
            value = GetHigh16OfFloat(g_tCalib.LoadVolt.x1);
            break;
        case REG03_CALIB_LOAD_VOLT_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadVolt.x1);
            break;
        case REG03_CALIB_LOAD_VOLT_Y1:
            value = GetHigh16OfFloat(g_tCalib.LoadVolt.y1);
            break;
        case REG03_CALIB_LOAD_VOLT_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadVolt.y1);
            break;
        case REG03_CALIB_LOAD_VOLT_X2:
            value = GetHigh16OfFloat(g_tCalib.LoadVolt.x2);
            break;
        case REG03_CALIB_LOAD_VOLT_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadVolt.x2);
            break;
        case REG03_CALIB_LOAD_VOLT_Y2:
            value = GetHigh16OfFloat(g_tCalib.LoadVolt.y2);
            break;
        case REG03_CALIB_LOAD_VOLT_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadVolt.y2);
            break;

        case REG03_CALIB_LOAD_CURR1_X1:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].x1);
            break;
        case REG03_CALIB_LOAD_CURR1_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].x1);
            break;
        case REG03_CALIB_LOAD_CURR1_Y1:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].y1);
            break;
        case REG03_CALIB_LOAD_CURR1_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].y1);
            break;
        case REG03_CALIB_LOAD_CURR1_X2:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].x2);
            break;
        case REG03_CALIB_LOAD_CURR1_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].x2);
            break;
        case REG03_CALIB_LOAD_CURR1_Y2:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].y2);
            break;
        case REG03_CALIB_LOAD_CURR1_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].y2);
            break;
        case REG03_CALIB_LOAD_CURR1_X3:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].x3);
            break;
        case REG03_CALIB_LOAD_CURR1_X3 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].x3);
            break;
        case REG03_CALIB_LOAD_CURR1_Y3:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].y3);
            break;
        case REG03_CALIB_LOAD_CURR1_Y3 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].y3);
            break;
        case REG03_CALIB_LOAD_CURR1_X4:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].x4);
            break;
        case REG03_CALIB_LOAD_CURR1_X4 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].x4);
            break;
        case REG03_CALIB_LOAD_CURR1_Y4:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[0].y4);
            break;
        case REG03_CALIB_LOAD_CURR1_Y4 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[0].y4);
            break;

        case REG03_CALIB_LOAD_CURR2_X1:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].x1);
            break;
        case REG03_CALIB_LOAD_CURR2_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].x1);
            break;
        case REG03_CALIB_LOAD_CURR2_Y1:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].y1);
            break;
        case REG03_CALIB_LOAD_CURR2_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].y1);
            break;
        case REG03_CALIB_LOAD_CURR2_X2:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].x2);
            break;
        case REG03_CALIB_LOAD_CURR2_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].x2);
            break;
        case REG03_CALIB_LOAD_CURR2_Y2:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].y2);
            break;
        case REG03_CALIB_LOAD_CURR2_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].y2);
            break;
        case REG03_CALIB_LOAD_CURR2_X3:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].x3);
            break;
        case REG03_CALIB_LOAD_CURR2_X3 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].x3);
            break;
        case REG03_CALIB_LOAD_CURR2_Y3:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].y3);
            break;
        case REG03_CALIB_LOAD_CURR2_Y3 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].y3);
            break;
        case REG03_CALIB_LOAD_CURR2_X4:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].x4);
            break;
        case REG03_CALIB_LOAD_CURR2_X4 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].x4);
            break;
        case REG03_CALIB_LOAD_CURR2_Y4:
            value = GetHigh16OfFloat(g_tCalib.LoadCurr[1].y4);
            break;
        case REG03_CALIB_LOAD_CURR2_Y4 + 1:
            value = GetLow16OfFloat(g_tCalib.LoadCurr[1].y4);
            break;

        case REG03_CALIB_TVCC_VOLT_X1:
            value = GetHigh16OfFloat(g_tCalib.TVCCVolt.x1);
            break;
        case REG03_CALIB_TVCC_VOLT_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCVolt.x1);
            break;
        case REG03_CALIB_TVCC_VOLT_Y1:
            value = GetHigh16OfFloat(g_tCalib.TVCCVolt.y1);
            break;
        case REG03_CALIB_TVCC_VOLT_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCVolt.y1);
            break;
        case REG03_CALIB_TVCC_VOLT_X2:
            value = GetHigh16OfFloat(g_tCalib.TVCCVolt.x2);
            break;
        case REG03_CALIB_TVCC_VOLT_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCVolt.x2);
            break;
        case REG03_CALIB_TVCC_VOLT_Y2:
            value = GetHigh16OfFloat(g_tCalib.TVCCVolt.y2);
            break;
        case REG03_CALIB_TVCC_VOLT_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCVolt.y2);
            break;

        case REG03_CALIB_TVCC_CURR_X1:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.x1);
            break;
        case REG03_CALIB_TVCC_CURR_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.x1);
            break;
        case REG03_CALIB_TVCC_CURR_Y1:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.y1);
            break;
        case REG03_CALIB_TVCC_CURR_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.y1);
            break;
        case REG03_CALIB_TVCC_CURR_X2:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.x2);
            break;
        case REG03_CALIB_TVCC_CURR_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.x2);
            break;
        case REG03_CALIB_TVCC_CURR_Y2:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.y2);
            break;
        case REG03_CALIB_TVCC_CURR_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.y2);
            break;
        case REG03_CALIB_TVCC_CURR_X3:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.x3);
            break;
        case REG03_CALIB_TVCC_CURR_X3 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.x3);
            break;
        case REG03_CALIB_TVCC_CURR_Y3:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.y3);
            break;
        case REG03_CALIB_TVCC_CURR_Y3 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.y3);
            break;
        case REG03_CALIB_TVCC_CURR_X4:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.x4);
            break;
        case REG03_CALIB_TVCC_CURR_X4 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.x4);
            break;
        case REG03_CALIB_TVCC_CURR_Y4:
            value = GetHigh16OfFloat(g_tCalib.TVCCCurr.y4);
            break;
        case REG03_CALIB_TVCC_CURR_Y4 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCCurr.y4);
            break;

        case REG03_CALIB_NTC_RES_X1:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.x1);
            break;
        case REG03_CALIB_NTC_RES_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.x1);
            break;
        case REG03_CALIB_NTC_RES_Y1:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.y1);
            break;
        case REG03_CALIB_NTC_RES_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.y1);
            break;
        case REG03_CALIB_NTC_RES_X2:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.x2);
            break;
        case REG03_CALIB_NTC_RES_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.x2);
            break;
        case REG03_CALIB_NTC_RES_Y2:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.y2);
            break;
        case REG03_CALIB_NTC_RES_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.y2);
            break;
        case REG03_CALIB_NTC_RES_X3:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.x3);
            break;
        case REG03_CALIB_NTC_RES_X3 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.x3);
            break;
        case REG03_CALIB_NTC_RES_Y3:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.y3);
            break;
        case REG03_CALIB_NTC_RES_Y3 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.y3);
            break;
        case REG03_CALIB_NTC_RES_X4:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.x4);
            break;
        case REG03_CALIB_NTC_RES_X4 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.x4);
            break;
        case REG03_CALIB_NTC_RES_Y4:
            value = GetHigh16OfFloat(g_tCalib.NtcRes.y4);
            break;
        case REG03_CALIB_NTC_RES_Y4 + 1:
            value = GetLow16OfFloat(g_tCalib.NtcRes.y4);
            break;

        /* 输出模拟量校准参数 */
        case REG03_CALIB_TVCC_SET_X1:
            value = GetHigh16OfFloat(g_tCalib.TVCCSet.x1);
            break;
        case REG03_CALIB_TVCC_SET_X1 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCSet.x1);
            break;
        case REG03_CALIB_TVCC_SET_Y1:
            value = GetHigh16OfFloat(g_tCalib.TVCCSet.y1);
            break;
        case REG03_CALIB_TVCC_SET_Y1 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCSet.y1);
            break;
        case REG03_CALIB_TVCC_SET_X2:
            value = GetHigh16OfFloat(g_tCalib.TVCCSet.x2);
            break;
        case REG03_CALIB_TVCC_SET_X2 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCSet.x2);
            break;
        case REG03_CALIB_TVCC_SET_Y2:
            value = GetHigh16OfFloat(g_tCalib.TVCCSet.y2);
            break;
        case REG03_CALIB_TVCC_SET_Y2 + 1:
            value = GetLow16OfFloat(g_tCalib.TVCCSet.y2);
            break;

        case REG03_CALIB_DAC_VOLT_X1:
            value = (uint16_t)g_tCalib.Dac10V.x1;
            break;
        case REG03_CALIB_DAC_VOLT_Y1:
            value = (uint16_t)g_tCalib.Dac10V.y1;
            break;
        case REG03_CALIB_DAC_VOLT_X2:
            value = (uint16_t)g_tCalib.Dac10V.x2;
            break;
        case REG03_CALIB_DAC_VOLT_Y2:
            value = (uint16_t)g_tCalib.Dac10V.y2;
            break;
        case REG03_CALIB_DAC_VOLT_X3:
            value = (uint16_t)g_tCalib.Dac10V.x3;
            break;
        case REG03_CALIB_DAC_VOLT_Y3:
            value = (uint16_t)g_tCalib.Dac10V.y3;
            break;
        case REG03_CALIB_DAC_VOLT_X4:
            value = (uint16_t)g_tCalib.Dac10V.x4;
            break;
        case REG03_CALIB_DAC_VOLT_Y4:
            value = (uint16_t)g_tCalib.Dac10V.y4;
            break;

        case REG03_CALIB_DAC_CURR_X1:
            value = (uint16_t)g_tCalib.Dac20mA.x1;
            break;
        case REG03_CALIB_DAC_CURR_Y1:
            value = (uint16_t)g_tCalib.Dac20mA.y1;
            break;
        case REG03_CALIB_DAC_CURR_X2:
            value = (uint16_t)g_tCalib.Dac20mA.x2;
            break;
        case REG03_CALIB_DAC_CURR_Y2:
            value = (uint16_t)g_tCalib.Dac20mA.y2;
            break;
        case REG03_CALIB_DAC_CURR_X3:
            value = (uint16_t)g_tCalib.Dac20mA.x3;
            break;
        case REG03_CALIB_DAC_CURR_Y3:
            value = (uint16_t)g_tCalib.Dac20mA.y3;
            break;
        case REG03_CALIB_DAC_CURR_X4:
            value = (uint16_t)g_tCalib.Dac20mA.x4;
            break;
        case REG03_CALIB_DAC_CURR_Y4:
            value = (uint16_t)g_tCalib.Dac20mA.y4;
            break;

        default:
            if (_reg_addr >= REG03_CALIB_CH1_R1_X1 && _reg_addr < REG03_CALIB_CH1_R1_X1 + 0x40) /* CH1通道8档量程的校准参数 */
            {
                uint16_t m, n, rg;

                m = _reg_addr - REG03_CALIB_CH1_R1_X1;
                rg = m / 8;
                n = (m % 8);
                switch (n)
                {
                    case 0:
                        value = GetHigh16OfFloat(g_tCalib.CH1[rg].x1);
                        break;
                    case 1:
                        value = GetLow16OfFloat(g_tCalib.CH1[rg].x1);
                        break;
                    case 2:
                        value = GetHigh16OfFloat(g_tCalib.CH1[rg].y1);
                        break;
                    case 3:
                        value = GetLow16OfFloat(g_tCalib.CH1[rg].y1);
                        break;
                    case 4:
                        value = GetHigh16OfFloat(g_tCalib.CH1[rg].x2);
                        break;
                    case 5:
                        value = GetLow16OfFloat(g_tCalib.CH1[rg].x2);
                        break;
                    case 6:
                        value = GetHigh16OfFloat(g_tCalib.CH1[rg].y2);
                        break;
                    case 7:
                        value = GetLow16OfFloat(g_tCalib.CH1[rg].y2);
                        break;
                }
            }
            else if (_reg_addr >= REG03_CALIB_CH2_R1_X1 && _reg_addr < REG03_CALIB_CH2_R1_X1 + 0x40) /* CH1通道8档量程的校准参数 */
            {
                uint16_t m, n, rg;

                m = _reg_addr - REG03_CALIB_CH2_R1_X1;
                rg = m / 8;
                n = (m % 8);
                switch (n)
                {
                    case 0:
                        value = GetHigh16OfFloat(g_tCalib.CH2[rg].x1);
                        break;
                    case 1:
                        value = GetLow16OfFloat(g_tCalib.CH2[rg].x1);
                        break;
                    case 2:
                        value = GetHigh16OfFloat(g_tCalib.CH2[rg].y1);
                        break;
                    case 3:
                        value = GetLow16OfFloat(g_tCalib.CH2[rg].y1);
                        break;
                    case 4:
                        value = GetHigh16OfFloat(g_tCalib.CH2[rg].x2);
                        break;
                    case 5:
                        value = GetLow16OfFloat(g_tCalib.CH2[rg].x2);
                        break;
                    case 6:
                        value = GetHigh16OfFloat(g_tCalib.CH2[rg].y2);
                        break;
                    case 7:
                        value = GetLow16OfFloat(g_tCalib.CH2[rg].y2);
                        break;
                }
            }
            else
            {
                return 0;
            }
            break;
    }

    *_reg_value = value;
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: WriteRegValue_06H
*    功能说明: 读取保持寄存器的值    写单个寄存器
*    形    参: reg_addr 寄存器地址
*              reg_value 寄存器值
*    返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
extern void JumpToDAPLink(void);
uint8_t WriteRegValue_06H(uint16_t reg_addr, uint16_t reg_value)
{
    uint8_t IPAddr[4];
    uint8_t NetMask[4];
    uint8_t Gateway_Addr[4];
    uint8_t SystemIP[4];

    switch (reg_addr)
    {
        /******************** DAC参数 ****************************/
        case REG03_OUT_VOLT_DAC: /* 2字节整数 设置输出电压的 DAC值 */
            g_tVar.OutVoltDAC = reg_value;
            bsp_SetDAC1(g_tVar.OutVoltDAC);
            break;

        case REG03_OUT_VOLT_MV: /* 2字节整数 设置输出电压的 mV值 */
        {
            uint16_t dac;

            g_tVar.OutVoltmV = reg_value;
            dac = dac1_VoltToDac(g_tVar.OutVoltmV);
            bsp_SetDAC1(dac);
        }
        break;

        case REG03_OUT_CURR_DAC: /* 2字节整数 设置输出电流的 DAC值 */
            g_tVar.OutCurrDAC = reg_value;
            bsp_SetDAC1(g_tVar.OutCurrDAC);
            break;

        case REG03_OUT_CURR_UA: /* 2字节整数 设置输出电流的 uA值 */
        {
            uint16_t dac;

            g_tVar.OutCurruA = reg_value;
            dac = dac1_CurrToDac(g_tVar.OutCurruA);
            bsp_SetDAC1(dac);
        }
        break;

        case REG03_OUT_TVCC_DAC: /* 2字节整数 设置TVCC电压的档位值（0-127） */
            g_tVar.OutTVCCDac = reg_value;
            MCP4018_WriteData(g_tVar.OutTVCCDac);
            break;

        case REG03_OUT_TVCC_MV: /* 2字节整数 mV值 1260 - 5000mV */
            g_tVar.OutTVCCmV = reg_value;
            bsp_SetTVCC(g_tVar.OutTVCCmV);
            break;

        /* DAC波形控制 */
        case REG03_DAC_WAVE_VOLT_RANGE: /* 电压量程 保留不用 固定正负10V */
            g_tDacWave.VoltRange = reg_value;
            break;

        case REG03_DAC_WAVE_TYPE: /* DAC波形类型 */
            g_tDacWave.Type = reg_value;
            break;

        case REG03_DAC_WAVE_VOLT_MAX: /* 波形最大电压，mV */
            g_tDacWave.VoltMax = reg_value;
            break;

        case REG03_DAC_WAVE_VOLT_MIN: /* 波形最小电压，mV */
            g_tDacWave.VoltMin = reg_value;
            break;

        case REG03_DAC_WAVE_FREQ: /* 32位 波形频率高16bit，Hz */
            g_tDacWave.Freq = (uint32_t)reg_value << 16;
            break;

        case REG03_DAC_WAVE_FREQ + 1: /* 32位 波形频率低16bit，Hz */
            g_tDacWave.Freq += reg_value;
            break;

        case REG03_DAC_WAVE_DUTY: /* 波形占空比 千分比 */
            g_tDacWave.Duty = reg_value;
            break;

        case REG03_DAC_WAVE_COUNT_SET: /* 32位 波形个数设置 */
            g_tDacWave.CycleSetting = (uint32_t)reg_value << 16;
            break;

        case REG03_DAC_WAVE_COUNT_SET + 1: /* 32位 波形个数设置 */
            g_tDacWave.CycleSetting += reg_value;
            break;

        case REG03_DAC_WAVE_START: /* DAC波形启动停止控制 */
            if (reg_value == 0)
            {
                g_tDacWave.Run = 0;
                dac1_StopWave();
            }
            else
            {
                g_tDacWave.Run = 1;
                dac1_StartDacWave();
            }
            break;

        /******************** ADC参数 ****************************/
        case 0x01FF:
            g_tDSO.MeasuteMode = reg_value;
            //            if (g_tDSO.Run == 1)
            //            {
            //                if (g_tDSO.FreqID < FREQ_NUM - 1)
            //                {
            //
            //                    DSO_StartADC(TabelFreq[g_tDSO.FreqID ]);
            //                }
            //            }
            break;

        case REG03_CH1_DC: /* CH1通道，AC/DC耦合切换 1表示DC */
            g_tDSO.DC1 = reg_value;
            DSO_SetDC(1, reg_value);
            break;

        case REG03_CH2_DC: /* CH2通道，AC/DC耦合切换 */
            g_tDSO.DC2 = reg_value;
            DSO_SetDC(2, reg_value);
            break;

        case REG03_CH1_GAIN: /* CH1通道，增益切换0-3 */
            g_tDSO.Gain1 = reg_value;
            DSO_SetGain(1, reg_value);
            break;

        case REG03_CH2_GAIN: /* CH2通道，增益切换0-3 */
            g_tDSO.Gain2 = reg_value;
            DSO_SetGain(2, reg_value);
            break;

        case REG03_CH1_OFFSET: /* CH1通道，偏置 */
            g_tDSO.Offset1 = reg_value;
            DSO_SetOffset(1, reg_value);
            break;

        case REG03_CH2_OFFSET: /* CH2通道，偏置 */
            g_tDSO.Offset2 = reg_value;
            DSO_SetOffset(2, reg_value);
            break;

        case REG03_DSO_FREQ_ID: /* 示波器采样频率档位   */
            g_tDSO.FreqID = reg_value;
            //            if (g_tDSO.Run == 1)
            //            {
            //                if (g_tDSO.FreqID < FREQ_NUM - 1)
            //                {
            //                    DSO_StartADC(TabelFreq[g_tDSO.FreqID ]);
            //                }
            //            }
            break;

        case REG03_DSO_SAMPLE_SIZE: /* 采样深度档位 */
            if (reg_value <= DSO_SIZE_MAX)
            {
                g_tDSO.SampleSizeID = reg_value;
            }
            //            if (g_tDSO.Run == 1)
            //            {
            //                if (g_tDSO.FreqID < FREQ_NUM - 1)
            //                {
            //
            //                    DSO_StartADC(TabelFreq[g_tDSO.FreqID ]);
            //                }
            //            }
            break;

        case REG03_DSO_TRIG_LEVEL: /* 触发电平，mV */
            g_tDSO.TrigLevel = reg_value;
            DSO_SetTriger();
            break;

        case REG03_DSO_TRIG_POS: /* 触发位置 */
            g_tDSO.TrigPos = reg_value;
            DSO_SetTriger();
            break;

        case REG03_DSO_TRIG_MODE: /* 触发模式 0=自动 1=普通 */
            g_tDSO.TrigMode = reg_value;
            DSO_SetTriger();
            //            if (g_tDSO.Run == 1)
            //            {
            //                if (g_tDSO.FreqID < FREQ_NUM - 1)
            //                {
            //
            //                    DSO_StartADC(TabelFreq[g_tDSO.FreqID ]);
            //                }
            //            }
            break;

        case REG03_DSO_TRIG_CHAN: /* 触发通道 0=CH1  1=CH2*/
            g_tDSO.TrigChan = reg_value;
            DSO_SetTriger();
            //            if (g_tDSO.Run == 1)
            //            {
            //                if (g_tDSO.FreqID < FREQ_NUM - 1)
            //                {
            //
            //                    DSO_StartADC(TabelFreq[g_tDSO.FreqID ]);
            //                }
            //            }
            break;

        case REG03_DSO_TRIG_EDGE: /* 触发边沿 0=下降沿 1=上上升沿 */
            g_tDSO.TrigEdge = reg_value;
            break;

        case REG03_DSO_CHAN_EN: /* 通道使能控制 bit0 = CH1  bit1 = CH2  bit2 = CH2 - CH1 */
            g_tDSO.ChanEnable = reg_value;
            break;

        case REG03_DSO_RUN: /* 示波器采集控制,0：停止 1：CH1，2：CH2，3：CH1+CH2 */
            g_tDSO.Run = reg_value;
            if (g_tDSO.Run == 1)
            {
                if (g_tDSO.FreqID < FREQ_NUM - 1)
                {
                    DSO_StartADC(TabelFreq[g_tDSO.FreqID]);
                }
            }
            else if (g_tDSO.Run == 0)
            {
                DSO_StopADC();
            }
            break;

        case REG03_DSO_CURR_GAIN:
            g_tDSO.CurrGain = reg_value;
            DSO_SetCurrGain(g_tDSO.CurrGain);
            break;

        /******************** 读取波形 ****************************/
        case REG03_WAVE_LOCK: /* 波形锁定，等待读取 */
            if (reg_value == 1)
            {
                if (g_tDSO.TrigMode == TRIG_MODE_AUTO) /* 自动触发 */
                {
                    DSO_LockWave();
                }
                else if (g_tDSO.TrigMode == TRIG_MODE_NORMAL) /* 普通触发 - 循环触发 */
                {
                    if (g_tDSO.TrigFlag == 1)
                    {
                        DSO_LockWave();
                        g_tDSO.TrigFlag = 0;

                        if (g_tDSO.Run == 1)
                        {
                            if (g_tDSO.FreqID < FREQ_NUM - 1)
                            {
                                DSO_StartADC(TabelFreq[g_tDSO.FreqID]);
                            }
                        }
                    }
                }
                else if (g_tDSO.TrigMode == TRIG_MODE_SINGLE) /* 单次触发 - 只触发一次 */
                {
                    if (g_tDSO.TrigFlag == 1)
                    {
                        DSO_LockWave();
                        g_tDSO.TrigFlag = 0;
                    }
                }
            }
            break;

        /******************** 网络参数 ****************************/
        case REG03_NET_LOCAL_IP_H:
        case REG03_LocalIPAddr_H:
            IPAddr[0] = reg_value >> 8;
            IPAddr[1] = reg_value;

            if (IPAddr[0] == 0 && IPAddr[1] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.LocalIPAddr[0] != IPAddr[0] || g_tParam.LocalIPAddr[1] != IPAddr[1])
                {
                    fResetReq_06H = 1; /* 如果IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.LocalIPAddr[0] = reg_value >> 8;
                g_tParam.LocalIPAddr[1] = reg_value;
            }
            break;

        case REG03_NET_LOCAL_IP_L:
        case REG03_LocalIPAddr_L:
            IPAddr[2] = reg_value >> 8;
            IPAddr[3] = reg_value;

            if (IPAddr[2] == 0 && IPAddr[3] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.LocalIPAddr[2] != IPAddr[2] || g_tParam.LocalIPAddr[3] != IPAddr[3])
                {
                    fResetReq_06H = 1; /* 如果IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.LocalIPAddr[2] = reg_value >> 8;
                g_tParam.LocalIPAddr[3] = reg_value;
            }
            break;

        case REG03_NET_GATEWAY_H:
        case REG03_GatewayAddr_H:
            Gateway_Addr[0] = reg_value >> 8;
            Gateway_Addr[1] = reg_value;

            if (Gateway_Addr[0] == 0 && Gateway_Addr[1] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.Gateway[0] != Gateway_Addr[0] || g_tParam.Gateway[1] != Gateway_Addr[1])
                {
                    fResetReq_06H = 1; /* 如果IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.Gateway[0] = reg_value >> 8;
                g_tParam.Gateway[1] = reg_value;
            }
            break;

        case REG03_NET_GATEWAY_L:
        case REG03_GatewayAddr_L:
            Gateway_Addr[2] = reg_value >> 8;
            Gateway_Addr[3] = reg_value;

            if (Gateway_Addr[2] == 0 && Gateway_Addr[3] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.Gateway[2] != Gateway_Addr[2] || g_tParam.Gateway[3] != Gateway_Addr[3])
                {
                    fResetReq_06H = 1; /* 如果IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.Gateway[2] = reg_value >> 8;
                g_tParam.Gateway[3] = reg_value;
            }
            break;

        case REG03_NET_NETMASK_H:
        case REG03_SubMask_H:
            NetMask[0] = reg_value >> 8;
            NetMask[1] = reg_value;

            if (NetMask[0] == 0 && NetMask[1] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.NetMask[0] != NetMask[0] || g_tParam.NetMask[1] != NetMask[1])
                {
                    fResetReq_06H = 1; /* 如果IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.NetMask[0] = reg_value >> 8;
                g_tParam.NetMask[1] = reg_value;
            }
            break;

        case REG03_NET_NETMASK_L:
        case REG03_SubMask_L:
            NetMask[2] = reg_value >> 8;
            NetMask[3] = reg_value;

            if (NetMask[2] == 0 && NetMask[3] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.NetMask[2] != NetMask[2] || g_tParam.NetMask[3] != NetMask[3])
                {
                    fResetReq_06H = 1; /* 如果IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.NetMask[2] = reg_value >> 8;
                g_tParam.NetMask[3] = reg_value;
            }
            break;

        case REG03_NET_LOCAL_PORT:
        case REG03_TCP_PORT:
            fSaveReq_06H = 1; /* 需要保存参数 */
            g_tParam.LocalTCPPort = reg_value;
            break;

        case REG03_NET_SYSTEM_IP_H:
            SystemIP[0] = reg_value >> 8;
            SystemIP[1] = reg_value;
            if (SystemIP[0] == 0 && SystemIP[1] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.RemoteIPAddr[0] != SystemIP[0] || g_tParam.RemoteIPAddr[1] != SystemIP[1])
                {
                    fResetReq_06H = 1; /* 如果管理系统IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.RemoteIPAddr[0] = reg_value >> 8;
                g_tParam.RemoteIPAddr[1] = reg_value;
            }
            break;

        case REG03_NET_SYSTEM_IP_L:
            SystemIP[2] = reg_value >> 8;
            SystemIP[3] = reg_value;

            if (SystemIP[2] == 0 && SystemIP[3] == 0) /* 错误数据 */
            {
                ;
            }
            else
            {
                if (g_tParam.RemoteIPAddr[2] != SystemIP[2] || g_tParam.RemoteIPAddr[3] != SystemIP[3])
                {
                    fResetReq_06H = 1; /* 如果管理系统IP地址变化,则复位标志置1 */
                    fSaveReq_06H = 1;    /* 需要保存参数 */
                }
                g_tParam.RemoteIPAddr[2] = reg_value >> 8;
                g_tParam.RemoteIPAddr[3] = reg_value;
            }
            break;

        case REG03_NET_SYSTEM_PORT:
            g_tParam.RemoteTcpPort = reg_value;
            fSaveReq_06H = 1; /* 需要保存参数 */
            break;

        case REG03_MAC12: /* 禁止修改MAC */
        case REG03_MAC34:
        case REG03_MAC56:
            break;

            /**************************** GPIO控制 *****************************/

        case REG03_D0_GPIO_MODE: /* 配置D0-D9  GPIO模式 */
        case REG03_D1_GPIO_MODE:
        case REG03_D2_GPIO_MODE:
        case REG03_D3_GPIO_MODE:
        case REG03_D4_GPIO_MODE:
        case REG03_D5_GPIO_MODE:
        case REG03_D6_GPIO_MODE:
        case REG03_D7_GPIO_MODE:
        case REG03_D8_GPIO_MODE:
        case REG03_D9_GPIO_MODE:
        case REG03_D10_GPIO_MODE:
        case REG03_D11_GPIO_MODE:
        case REG03_D12_GPIO_MODE:
        case REG03_D13_GPIO_MODE:
        {
            uint8_t idx;

            idx = reg_addr - REG03_D0_GPIO_MODE + EIO_D0;
            EIO_ConfigPort(idx, (EIO_SELECT_E)reg_value);
        }
        break;

        case REG03_SEND_KEY:        /* 不应答给PC，因为lua打印返回会冲突 */
            {
                uint8_t kid;
                uint8_t state;
                
                kid = reg_value >> 8;
                state = reg_value;
                
                if (kid == KID_S)
                {
                    g_tVar.PC_CKeyState = 0;
                    g_tVar.PC_SKeyState = state;
                    g_tVar.PC_KeyEnabled = 1;
                }
                else if (kid == KID_C)
                {
                    g_tVar.PC_SKeyState = 0;
                    g_tVar.PC_CKeyState = state;
                    g_tVar.PC_KeyEnabled = 2;
                }
                
                fDisableAck = 1;  /* 不要应答 */
            }
            break;
        
        /*********************************************************/
        case REG03_RESET_TO_BOOT:
            if (reg_value == JUMP_TO_APP)         /* 复位进入APP */
            {                
                g_tVar.ReqJump = JUMP_TO_APP;
            }
            else if (reg_value == JUMP_TO_BOOT)    /* 复位进入BOOT 升级 */
            {
                g_tVar.ReqJump = JUMP_TO_BOOT;
            }
            else if (reg_value == JUMP_TO_DAP)    /* 进入DAP */
            {
                g_tVar.ReqJump = JUMP_TO_DAP;
            }            
            else if (reg_value == JUMP_TO_EMMC)    /*  进入U盘模式 */
            {
                g_MainStatus = MS_USB_EMMC;
            }
            else if (reg_value == JUMP_TO_LINK)    /*  进入APP联机 */
            {
                g_MainStatus = MS_LINK_MODE;
            }
            else if (reg_value == JUMP_TO_UPDATE_BOOT)    /*  进入文件管理界面 */
            {                
                g_tVar.ReqWriteBoot = 1;
                g_MainStatus = MS_FILE_MANAGE;
            }
            break;

        /**************************** 时钟参数 *****************************/

        case REG03_RTC_YEAR:
            RTC_ReadClock();
            g_tRTC.Year = reg_value;
            RTC_WriteClock(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            break;

        case REG03_RTC_MON:
            RTC_ReadClock();
            g_tRTC.Mon = reg_value;
            RTC_WriteClock(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            break;

        case REG03_RTC_DAY:
            RTC_ReadClock();
            g_tRTC.Day = reg_value;
            RTC_WriteClock(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            break;

        case REG03_RTC_HOUR:
            RTC_ReadClock();
            g_tRTC.Hour = reg_value;
            RTC_WriteClock(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            break;
            ;

        case REG03_RTC_MIN:
            RTC_ReadClock();
            g_tRTC.Min = reg_value;
            RTC_WriteClock(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            break;

        case REG03_RTC_SEC:
            RTC_ReadClock();
            g_tRTC.Sec = reg_value;
            RTC_WriteClock(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            break;

        /**************** LUA （未用）*********************/
        case REG03_LUA_CMD:
            if (reg_value == 1)
            {
                //lua_Run();
                g_tVar.LuaRunOnce = 1;
            }
            else if (reg_value == 2)
            {
                lua_do("init()");
            }
            break;

        case REG03_CALIB_KEY: /* 校准参数写入使能控制 */
            if (reg_value == 1)
            {
                g_tVar.CalibEnable = 1;     /* 允许修改校准参数 */
            }
            else if (reg_value == 0)
            {
                g_tVar.CalibEnable = 0;     /* 禁止修改校准参数 */
            }
            else if (reg_value == 0x5AA5)
            {
                InitCalibParam();
            }
            else if (reg_value == 0xEF55)   /* 恢复基本参数缺省值，不含校准参数 */
            {
                InitBaseParam();
            }                        
            break;

        default:
            if (g_tVar.CalibEnable == 1 && reg_addr >= REG03_CALIB_CH1_R1_X1 && reg_addr <= REG03_CALIB_PARAM_END)
            {
                /* 写校准参数 */
                WriteCalibRegValue_06H(reg_addr, reg_value);

                fSaveCalibParam = 1; /* 需要保存校准参数 */
            }
            else
            {
                return 0; /* 参数异常，返回 0 */
            }
    }

    //    RegBak = reg_addr;        /* 记录上次的寄存器 */

    return 1; /* 读取成功 */
}

/*
*********************************************************************************************************
*    函 数 名: WriteCalibRegValue_06H
*    功能说明: 写校准参数寄存器    写单个寄存器
*    形    参: _reg_addr 寄存器地址
*              _reg_value 寄存器值
*    返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t WriteCalibRegValue_06H(uint16_t _reg_addr, uint16_t _reg_value)
{
    static uint16_t value1 = 0, value2 = 0;
    uint32_t data;

    switch (_reg_addr)
    {
        /* 输入模拟量校准参数 */
        case REG03_CALIB_LOAD_VOLT_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_VOLT_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadVolt.x1) = data;
            break;
        case REG03_CALIB_LOAD_VOLT_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_VOLT_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadVolt.y1) = data;
            break;
        case REG03_CALIB_LOAD_VOLT_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_VOLT_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadVolt.x2) = data;
            break;
        case REG03_CALIB_LOAD_VOLT_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_VOLT_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadVolt.y2) = data;
            break;

        case REG03_CALIB_LOAD_CURR1_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].x1) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].y1) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].x2) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].y2) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_X3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_X3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].x3) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_Y3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_Y3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].y3) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_X4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_X4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].x4) = data;
            break;
        case REG03_CALIB_LOAD_CURR1_Y4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR1_Y4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[0].y4) = data;
            break;

        case REG03_CALIB_LOAD_CURR2_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].x1) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].y1) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].x2) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].y2) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_X3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_X3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].x3) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_Y3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_Y3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].y3) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_X4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_X4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].x4) = data;
            break;
        case REG03_CALIB_LOAD_CURR2_Y4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_LOAD_CURR2_Y4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.LoadCurr[1].y4) = data;
            break;

        case REG03_CALIB_TVCC_VOLT_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_VOLT_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCVolt.x1) = data;
            break;
        case REG03_CALIB_TVCC_VOLT_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_VOLT_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCVolt.y1) = data;
            break;
        case REG03_CALIB_TVCC_VOLT_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_VOLT_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCVolt.x2) = data;
            break;
        case REG03_CALIB_TVCC_VOLT_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_VOLT_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCVolt.y2) = data;
            break;

        case REG03_CALIB_TVCC_CURR_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.x1) = data;
            break;
        case REG03_CALIB_TVCC_CURR_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.y1) = data;
            break;
        case REG03_CALIB_TVCC_CURR_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.x2) = data;
            break;
        case REG03_CALIB_TVCC_CURR_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.y2) = data;
            break;
        case REG03_CALIB_TVCC_CURR_X3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_X3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.x3) = data;
            break;
        case REG03_CALIB_TVCC_CURR_Y3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_Y3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.y3) = data;
            break;
        case REG03_CALIB_TVCC_CURR_X4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_X4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.x4) = data;
            break;
        case REG03_CALIB_TVCC_CURR_Y4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_CURR_Y4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCCurr.y4) = data;
            break;

        case REG03_CALIB_NTC_RES_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.x1) = data;
            break;
        case REG03_CALIB_NTC_RES_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.y1) = data;
            break;
        case REG03_CALIB_NTC_RES_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.x2) = data;
            break;
        case REG03_CALIB_NTC_RES_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.y2) = data;
            break;
        case REG03_CALIB_NTC_RES_X3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_X3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.x3) = data;
            break;
        case REG03_CALIB_NTC_RES_Y3:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_Y3 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.y3) = data;
            break;
        case REG03_CALIB_NTC_RES_X4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_X4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.x4) = data;
            break;
        case REG03_CALIB_NTC_RES_Y4:
            value1 = _reg_value;
            break;
        case REG03_CALIB_NTC_RES_Y4 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.NtcRes.y4) = data;
            break;

        /* 输出模拟量校准参数 */
        case REG03_CALIB_TVCC_SET_X1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_SET_X1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCSet.x1) = data;
            break;
        case REG03_CALIB_TVCC_SET_Y1:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_SET_Y1 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCSet.y1) = data;
            break;
        case REG03_CALIB_TVCC_SET_X2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_SET_X2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCSet.x2) = data;
            break;
        case REG03_CALIB_TVCC_SET_Y2:
            value1 = _reg_value;
            break;
        case REG03_CALIB_TVCC_SET_Y2 + 1:
            value2 = _reg_value;
            data = ((uint32_t)value1 << 16) + value2;
            *((uint32_t *)&g_tCalib.TVCCSet.y2) = data;
            break;

        case REG03_CALIB_DAC_VOLT_X1:
            g_tCalib.Dac10V.x1 = _reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_Y1:
            g_tCalib.Dac10V.y1 = (int16_t)_reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_X2:
            g_tCalib.Dac10V.x2 = _reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_Y2:
            g_tCalib.Dac10V.y2 = (int16_t)_reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_X3:
            g_tCalib.Dac10V.x3 = _reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_Y3:
            g_tCalib.Dac10V.y3 = (int16_t)_reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_X4:
            g_tCalib.Dac10V.x4 = _reg_value;
            break;
        case REG03_CALIB_DAC_VOLT_Y4:
            g_tCalib.Dac10V.y4 = (int16_t)_reg_value;
            break;

        case REG03_CALIB_DAC_CURR_X1:
            g_tCalib.Dac20mA.x1 = _reg_value;
            break;
        case REG03_CALIB_DAC_CURR_Y1:
            g_tCalib.Dac20mA.y1 = (int16_t)_reg_value;
            break;
        case REG03_CALIB_DAC_CURR_X2:
            g_tCalib.Dac20mA.x2 = _reg_value;
            break;
        case REG03_CALIB_DAC_CURR_Y2:
            g_tCalib.Dac20mA.y2 = (int16_t)_reg_value;
            break;
        case REG03_CALIB_DAC_CURR_X3:
            g_tCalib.Dac20mA.x3 = _reg_value;
            break;
        case REG03_CALIB_DAC_CURR_Y3:
            g_tCalib.Dac20mA.y3 = (int16_t)_reg_value;
            break;
        case REG03_CALIB_DAC_CURR_X4:
            g_tCalib.Dac20mA.x4 = _reg_value;
            break;
        case REG03_CALIB_DAC_CURR_Y4:
            g_tCalib.Dac20mA.y4 = (int16_t)_reg_value;
            break;

        default:
            if (_reg_addr >= REG03_CALIB_CH1_R1_X1 && _reg_addr < REG03_CALIB_CH1_R1_X1 + 0x40) /* CH1通道8档量程的校准参数 */
            {
                uint16_t m, n, rg;

                m = _reg_addr - REG03_CALIB_CH1_R1_X1;
                rg = m / 8;
                n = (m % 8);
                if ((m % 2) == 0)
                {
                    value1 = _reg_value;
                    break;
                }
                else
                {
                    value2 = _reg_value;
                    data = ((uint32_t)value1 << 16) + value2;

                    if (n == 1)
                        *((uint32_t *)&g_tCalib.CH1[rg].x1) = data;
                    else if (n == 3)
                        *((uint32_t *)&g_tCalib.CH1[rg].y1) = data;
                    else if (n == 5)
                        *((uint32_t *)&g_tCalib.CH1[rg].x2) = data;
                    else if (n == 7)
                        *((uint32_t *)&g_tCalib.CH1[rg].y2) = data;
                    break;
                }
            }
            else if (_reg_addr >= REG03_CALIB_CH2_R1_X1 && _reg_addr < REG03_CALIB_CH2_R1_X1 + 0x40) /* CH1通道8档量程的校准参数 */
            {
                uint16_t m, n, rg;

                m = _reg_addr - REG03_CALIB_CH2_R1_X1;
                rg = m / 8;
                n = (m % 8);
                if ((m % 2) == 0)
                {
                    value1 = _reg_value;
                    break;
                }
                else
                {
                    value2 = _reg_value;
                    data = ((uint32_t)value1 << 16) + value2;

                    if (n == 1)
                        *((uint32_t *)&g_tCalib.CH2[rg].x1) = data;
                    else if (n == 3)
                        *((uint32_t *)&g_tCalib.CH2[rg].y1) = data;
                    else if (n == 5)
                        *((uint32_t *)&g_tCalib.CH2[rg].x2) = data;
                    else if (n == 7)
                        *((uint32_t *)&g_tCalib.CH2[rg].y2) = data;
                    break;
                }
            }
            break;
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ReadRegValue_04H
*    功能说明: 读取模拟量输入寄存器的值
*    形    参: _reg_addr 寄存器地址
*              _reg_value 存放寄存器结果
*    返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
uint8_t ReadRegValue_04H(uint16_t _reg_addr, uint16_t *_reg_value)
{
    uint16_t reg_value;
#if 0
    switch (_reg_addr)
    {
        case REG04_ADC01:
        case REG04_ADC02:
        case REG04_ADC03:
        case REG04_ADC04:
        case REG04_ADC05:
        case REG04_ADC06:
        case REG04_ADC07:
        case REG04_ADC08:
            {
                uint8_t m;
                
                m = _reg_addr - REG04_ADC01;
                
                reg_value = (uint16_t)g_tAD7606.sNowAdc[m];
            }
            break;

        default:
            return 0;    
    }

#endif
    *_reg_value = reg_value;
    return 1; /* 读取成功 ，返回读到的值*/
}

/*
*********************************************************************************************************
*    函 数 名: MODS_RelayIsOn
*    功能说明: 判断继电器状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t MODS_GetDOState(uint16_t _reg, uint8_t *_value)
{
    *_value = EIO_GetOutLevel(_reg);
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: MODS_GetDIState
*    功能说明: 读取输入状态
*    形    参: _reg 寄存器地址
*              _value 寄存器结果
*    返 回 值: 1表示OK 0表示错误(地址错误)
*********************************************************************************************************
*/
uint8_t MODS_GetDIState(uint16_t _reg, uint8_t *_value)
{
    /* 未做滤波处理 */
    if (_reg < REG02_MAX)
    {
        *_value = 0;
        return 1;
    }
    else
    {
        *_value = 0;
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: MODS_WriteRelay
*    功能说明: 强制单线圈（对应D01/D02/D03）
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t MODS_WriteRelay(uint16_t _reg, uint8_t _on)
{
#if REG01_Y01 == 0
    if (_reg < REG01_Y01 + MODS_DO_NUM)
#else
    if (_reg >= REG01_Y01 && _reg < REG01_Y01 + MODS_DO_NUM)
#endif
    {
        if (_on == 1)
        {
            EIO_SetOutLevel(_reg, 1);
        }
        else
        {
            EIO_SetOutLevel(_reg, 0);
        }
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: PC_CmdTask
*    功能说明: 处理PC控制指令，这些指令不能在USB中断中执行。放到 bsp_Idle() 中去执行
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void PC_CmdTask(void)
{
    /* V1.44, 处理USB通信中的相关执行，这些指令在USB中断中不方便执行 */
    {
        if (g_tVar.ReqJump > 0)
        {
            if (g_tVar.ReqJump == JUMP_TO_APP)         /* 复位进入APP */
            {                
                /* 关闭USB延迟2秒 */                           
                usbd_UnInit();
                bsp_DelayUS(2000*1000);
                
                *(uint32_t *)0x20000000 = 0;
                NVIC_SystemReset(); /* 复位CPU */
            }
            else if (g_tVar.ReqJump == JUMP_TO_BOOT)    /* 复位进入BOOT 升级 */
            {
                /* 关闭USB延迟2秒 */                           
                usbd_UnInit();
                bsp_DelayUS(2000*1000);                
                
                *(uint32_t *)0x20000000 = 0x5AA51234;
                NVIC_SystemReset(); /* 复位CPU */
            }
            else if (g_tVar.ReqJump == JUMP_TO_DAP)    /* 进入DAP */
            {
                JumpToDAPLink();
            }

            g_tVar.ReqJump = 0;
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
