/*
*********************************************************************************************************
*
*    模块名称 : 字符串操作\数值转换
*    文件名称 : bsp_user_lib.h
*    版    本 : V1.2
*    说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_USER_LIB_H
#define __BSP_USER_LIB_H

int str_len(char *_str);
void str_cpy(char *_tar, char *_src);
int str_cmp(char *s1, char *s2);
void mem_set(char *_tar, char _data, int _len);

void int_to_str(int _iNumber, char *_pBuf, unsigned char _len);
int str_to_int(char *_pStr);

uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);

uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen);
int32_t CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);
float CaculTwoPointFloat(float x1, float y1, float x2, float y2, float x);

char BcdToChar(uint8_t _bcd);
void HexToAscll(uint8_t *_pHex, char *_pAscii, uint16_t _BinBytes);
uint16_t AsciiToHex(char *_pAscii, uint8_t *_pHex, uint16_t _MaxLen);

uint32_t AsciiToUint32(char *pAscii);

uint8_t CharToInt(char _ch);
uint8_t TwoCharToInt(char *_ch);

int str_to_int2(char *_pStr);
int str_to_int3(char *_pStr);
uint8_t ip_str_decode(char *_ipstr, uint8_t *_out);

uint16_t GetHigh16OfFloat(float _ff);
uint16_t GetLow16OfFloat(float _ff);
uint32_t Get32BitOfFloat(float _ff);
uint8_t float_isnan(float _ff);

float BEBufToFloat(uint8_t *_pBuf);

char *strlwr(char *str);
char *strupr(char *str);
char *StrUTF8ToGBK(char *utf8, char *gbk, uint16_t gbk_size);

uint8_t CheckBlankBuf(const char *_buf, uint32_t _len, uint8_t _EmptyValue);

uint32_t soft_crc32(uint8_t *pStart, uint32_t uSize);
uint32_t STM32_CRC32_Word(uint32_t *_pBuf, uint32_t _Len);

uint32_t Soft_CRC32_byte(uint8_t *pData, uint32_t Length);
uint32_t STM32_CRC32_byte(uint8_t *_pBuf, uint32_t _Len);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
