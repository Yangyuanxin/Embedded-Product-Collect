/*
*********************************************************************************************************
*
*	模块名称 : MODEBUS 通信模块 (主机程序）
*	文件名称 : modbus_host.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MOSBUS_HOST_H
#define __MOSBUS_HOST_H

#define SlaveAddr		0x55			/* 面板作为时，主板作从机 */

#define REG_P16			0x000F
#define REG_P18			0x0011

#define REG_P63			0x003E		/* 主板软件版本号 */


/* RTU 应答代码 */
#define RSP_OK				0		/* 成功 */
#define RSP_ERR_CMD			0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR	0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		0x04	/* 写入失败 */

#define H_RX_BUF_SIZE       (2 * 1024)
#define H_TX_BUF_SIZE       (2 * 1024)

typedef struct
{
	uint8_t *RxBuf;         /* 注意是指针，没有分配内存 */
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;

	uint8_t RspCode;

	uint8_t TxBuf[H_TX_BUF_SIZE];
	uint8_t TxCount;
	
	uint16_t Reg03H;		/* 保存主机发送的03H指令的寄存器首地址 */
	uint16_t Reg30H;		/* 保存主机发送的30H指令的寄存器首地址 */
	uint8_t RegNum;			/* 寄存器个数 */
	
	uint8_t fAck05H;		/* 05H指令的应答 */
	uint8_t fAck06H;		/* 06H指令的应答。0 表示执行失败 1表示执行成功  */
	uint8_t fAck03H;
}MODH_T;

extern MODH_T g_tModH;

void MODH_SendWithCRC(void);

void MODH_Send03H(uint8_t _addr, uint16_t _reg, uint16_t _num);
void MODH_Send05H(uint8_t _addr, uint16_t _reg, uint16_t _value);
void MODH_Send06H(uint8_t _addr, uint16_t _reg, uint16_t _value);
void MODH_Send10H(uint8_t _addr, uint16_t _reg, uint8_t _num, uint8_t *_buf);

uint8_t MODH_WriteParam_06H(uint16_t _reg, uint16_t _value);
uint8_t MODH_WriteParam_05H(uint16_t _reg, uint16_t _value);
uint8_t MODH_ReadParam_03H(uint16_t _reg, uint16_t _num);

uint8_t MODH_Poll(uint8_t *_buf, uint16_t _len);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
