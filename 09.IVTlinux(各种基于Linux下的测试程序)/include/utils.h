#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/wait.h>

#include "glabtypes.h"

#define INT32U 		unsigned int 
#define INT16U  	unsigned short 
#define INT8U		unsigned char 

void delayMs(int time_ms);
int pf_exec_simple_cmd(char *cmd , char* retString, unsigned int stringlen);
int findStr(const char* string, const char* match_str);
void flushSerial(int fd);
void flushInputSerial(int fd);
void flushOutputSerial(int fd);
unsigned char* shortToHex(unsigned char* outpacket, unsigned short inpacket);
unsigned short hexToShort(unsigned char* inpacket);
ST_UCHAR* HEX_to_ASCII_Convertor(ST_UCHAR* bcd,int length, ST_UCHAR* desc);
void HEX_to_ASCII_String(ST_UCHAR* bcd,int length);
unsigned char* bigEndUnintToHex(unsigned char* outpacket, unsigned int inpacket);
unsigned int bigEndHexToUint(unsigned char* intpacket);
ST_UINT uint32_aligned_len(ST_UINT datalen);
int isBCDStringValid(ST_UCHAR *c, int len);
int ASCII_to_BCD_Convertor(ST_UCHAR* c,int length, ST_UCHAR* bcd);
void HexStrToByte(const char* source, unsigned char* dest, int sourceLen);
int rtuReceiveMsg(int s, ST_UCHAR *msg, int to_read_len , int Time);
ST_UCHAR cal_Xor(ST_UCHAR * data,int count);
ST_UCHAR* smallEndShortToHex(unsigned char* outpacket, unsigned short inpacket);
unsigned short smallEndHexToShort(ST_UCHAR* inpacket);
int isPrintStr(ST_UCHAR *inpacket,int len);
int  Pthread_mutex_lock(pthread_mutex_t *mutex);
int  Pthread_mutex_unlock(pthread_mutex_t *mutex);
uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);
ST_UCHAR* data_encode(ST_UCHAR *buf, ST_UCHAR* data,ST_UINT datalen);
void Crc16(const INT8U *sucData, INT16U usLen, INT8U *pucCRC);
int  inttobcd(int myint);
int  bcdtoint(int myint);



#endif
