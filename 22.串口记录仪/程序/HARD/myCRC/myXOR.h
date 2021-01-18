/**
 * 2020年5月17日07:09:53
 *     上位机跟单片机传输协议：
*      1字节（低6位本次大小，高8位结束帧置1，置0正常帧，高7位置一握手信号，置0 数据信号） + DATA + 1字节XOR校验  
*      上位机握手DATA：0xaa,0x55 
*      下位机应答DATA: 有效数据的序号 1字节 + 错误码
*      有效数据的序号总是自增，上位机发现跟自己发送的次数不匹配，自动结束重发。
*  
*/ 

#ifndef  __MYXOR_H
#define  __MYXOR_H
#include "uhead.h"

/*配置*/
#include "usbd_cdc_if.h"

#define		FILESIZE	64				// 文件缓存大小
#define		MYXOR_SEND(Buf,Len)			CDC_Transmit_FS(Buf, Len)

#define     ENDFILE     0x80            // 结束文件
#define     CONNECT     0x40            // 握手信号  
#define     BYTE2SEND   62              // 一次发送的字节
#define     IS_OK       0            	// 下位机传输成功
#define     IS_ERRO     0x01            // 下位机传输失败

typedef enum{
	XOR_OK = 0,		// 正常
	ERRO_LEN,		// 接收长度超出范围
	ERROR_XOR,		// 异或校验出错
	
}enumError_XOR;

typedef struct{
	u8 filebuf[FILESIZE];		// 提取数据缓存
	u8 rxlen;					// 实际数据长度
	u32 counter;				// 计数接收的帧数
	
}fileRXStr;


extern fileRXStr filerx;

/* 函数 */
enumError_XOR 	ResolveRxData(fileRXStr* filerx,u8* rxbuf,u32 len);	// 处理数据
void 			RespectUp(fileRXStr* filerx,u8 isok);// 回复上位机

#define			myXOR_Resolve(rxbuf,len)		ResolveRxData(&filerx,rxbuf, len)
#define			myXOR_Respect(isok)				RespectUp(&filerx,isok)


void SynchronizationTime(u8* timebuf,u8 buflen);		// 配置时钟 

#endif // !1

