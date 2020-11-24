#include "utils.h"

void delayMs(int ms)
{
	struct timeval tv;
	
	tv.tv_sec = ms/1000;
	tv.tv_usec = (ms%1000)*1000;
	select(0, NULL, NULL, NULL, &tv);
}

int pf_exec_simple_cmd(char *cmd , char* retString, unsigned int stringlen)
{
	FILE* pipe = NULL;
	int ret = -1;

	pipe = popen(cmd, "r");
    if(NULL == pipe)
    {
        printf("popen() failed: %s\n", cmd);
        return -1;
    }
    
    while(fgets(retString, stringlen,pipe) != NULL)
    {
        printf("%s", retString);
    }

	ret = pclose(pipe);
	if (-1 == ret)
    {
        printf("pclose() failed: %s\n", strerror(errno));
        return -1;
    }
    if (WIFEXITED(ret))
    {
        printf("subprocess exited, exit code: %d\n", WEXITSTATUS(ret));
        if (0 == WEXITSTATUS(ret))
        {
            // if command returning 0 means succeed
            printf("command succeed\n");
			return 0;
        }
        else
        {
            if(127 == WEXITSTATUS(ret))
            {
                printf("command not found\n");
                return WEXITSTATUS(ret);
            }
            else
            {
                printf("command failed: %s\n", strerror(WEXITSTATUS(ret)));
                return WEXITSTATUS(ret);
            }
        }
    }
    else
    {
        printf("subprocess exit failed\n");
        return -1;
    }
 
    return 0;
}

int findStr(const char* string, const char* match_str)
{
	if(strstr(string,match_str) != NULL)
	{
		return 1;
	}
	return 0;
}

void flushSerial(int fd)
{
	if(fd > 0)
	{
		tcflush(fd, TCIOFLUSH);
		return ;
	}
}

void flushInputSerial(int fd)
{
	if(fd > 0)
	{
		tcflush(fd, TCIFLUSH);
	}
}

void flushOutputSerial(int fd)
{
	if(fd > 0)
	{
		tcflush(fd, TCOFLUSH);
	}
}

unsigned char* shortToHex(unsigned char* outpacket, unsigned short inpacket)
{
	unsigned char* p  = outpacket; 

	*p++ = inpacket/0x100;
	*p++ = inpacket%0x100;
	return p;
}

unsigned short hexToShort(unsigned char* inpacket)
{
	unsigned short tmp  = 0;

	if(inpacket)
	{
		tmp = inpacket[0]*0x100 + inpacket[1];
	}
	return tmp;
}

unsigned short smallEndHexToShort(ST_UCHAR* inpacket)
{
	unsigned short tmp  = 0;

	if(inpacket)
	{
		tmp = inpacket[1]*0x100 + inpacket[0];
	}
	return tmp;
}

ST_UCHAR* smallEndShortToHex(ST_UCHAR* outpacket, unsigned short inpacket)
{
	unsigned char* p  = outpacket; 

	*p++ = inpacket%0x100;
	*p++ = inpacket/0x100;
	return p;
}

unsigned char* bigEndUnintToHex(unsigned char* outpacket, unsigned int inpacket)
{
	unsigned char* p  = outpacket; 

	*p++ = (inpacket/0x1000000);
	*p++ = (inpacket%0x1000000)/0x10000;
	*p++ = (inpacket%0x10000)/0x100;
	*p++ = (inpacket%0x100);
	return p;
}

unsigned int bigEndHexToUint(unsigned char* intpacket)
{
	unsigned char* p  = intpacket; 
	unsigned int ret = 0;

	ret += p[0]*0x1000000;
	ret += p[1]*0x10000;
	ret += p[2]*0x100;
	ret += p[3];
	return ret ;
}

ST_UINT uint32_aligned_len(ST_UINT datalen)
{
	if(datalen%sizeof(ST_UINT))
	{
		return (sizeof(ST_UINT)*((datalen/sizeof(ST_UINT))+1));
	}
	else
	{
		return datalen;
	}
}

ST_UCHAR bcd(ST_UCHAR dec)
{
   return ((dec/10)<<4)+(dec%10);
}

ST_UCHAR unbcd(ST_UCHAR bcd)
{
   return ((bcd>>4)*10)+bcd%16;
}

int _ASCII_to_BCD_Convertor(ST_UCHAR* c,int length, ST_UCHAR* bcd)
{
   int i = 0;
   for (i = 0; i < length/2; i++) {
      bcd[i] = ((c[2*i] - '0') << 4) + (c[2*i + 1] - '0');
   }
   return i;
}

int isBCDStringValid(ST_UCHAR *c, int len)
{
   int i = 0;
   for (i = 0; i < len; i++) {
      if (c[i] < '0' || c[i] > '9') return SD_FALSE;
   }
   return SD_TRUE;
}

int ASCII_to_BCD_Convertor(ST_UCHAR* c,int length, ST_UCHAR* bcd)
{
   if (!isBCDStringValid(c, length)) return 0;
   return _ASCII_to_BCD_Convertor(c, length, bcd);
}

#define cvt_nibble(a)   ((ST_UCHAR) ((a) > 9 ? (a) + 'A' - 10 : (a) + '0'))

ST_UCHAR* cvt_byte(ST_UCHAR*dst, ST_UCHAR byte)
{
   ST_UCHAR hex;
   hex = (byte >> 4) & (ST_UCHAR)0x0f;/* write HEX chars for the byte  */
   hex = cvt_nibble (hex);
   dst[0] = hex;

   hex = byte & (ST_UCHAR)0x0f;
   hex = cvt_nibble (hex);
   dst[1] = hex;
   return dst + 2;
}

ST_UCHAR* HEX_to_ASCII_Convertor(ST_UCHAR* bcd,int length, ST_UCHAR* desc)
{
   ST_UCHAR  *tmp;
   int i = 0;

   tmp = desc;
   for (i = 0; i < length; i++) {
      tmp = cvt_byte(tmp, bcd[i]);
      *tmp++ = ' ';
   }
   *(--tmp) = '\0';
   return desc;
}

void HEX_to_ASCII_String(ST_UCHAR* bcd,int length)
{
	char desc[10240] = {0};

	if(length >= (int)(sizeof(desc)/2))
	{
		return;
	}
	HEX_to_ASCII_Convertor (bcd, length, (ST_UCHAR*)desc);
	printf(":%s",desc);
}

//字节流转换为十六进制字符串
void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen )
{
    int  i;
    char szTmp[3];

    for( i = 0; i < nSrcLen; i++ )
    {
        sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );
        memcpy( &sDest[i * 2], szTmp, 2 );
    }
    return ;
}

//十六进制字符串转换为字节流
void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
    
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;

        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
}

int rtu_select(int s, fd_set *rfds, struct timeval *tv, int msg_length)
{
    int s_rc;
    while ((s_rc = select(s+1, rfds, NULL, NULL, tv)) == -1) {
        if (errno == EINTR) {
            if (1) {
                fprintf(stderr, "A non blocked signal was caught\n");
            }
            /* Necessary after an error */
            FD_ZERO(rfds);
            FD_SET(s, rfds);
        } else {
           //fprintf(stderr, "select error");
                return -1;
        }
    }

    if (s_rc == 0) {
        /* Timeout */
            errno = ETIMEDOUT;
        return -1;
    }

    return s_rc;
}

#define DEFAULT_TIME		500000
int rtuReceiveMsg(int s, ST_UCHAR *msg, int to_read_len , int time)
{
	int rc;
	fd_set rfds;
	struct timeval tv;
	int length_to_read;
	ST_UCHAR *p_msg;
	int msg_length = 0;
	//ST_UCHAR desc[10240]={0};

	FD_ZERO(&rfds);
   	FD_SET(s, &rfds);

	/* We need to analyse the message step by step.  At the first step, we want
	* to reach the function code because all packets contain this
	* information. */
	length_to_read = to_read_len;

	tv.tv_sec = (DEFAULT_TIME + time)/1000000;
	tv.tv_usec = (DEFAULT_TIME + time)%1000000;

	p_msg = (ST_UCHAR*)msg;
	while (length_to_read != 0) 
   	{
   		rc = rtu_select (s, &rfds, &tv, length_to_read);
		if (rc == -1) 
		{
			return -1;
		}

		rc = read (s, p_msg, length_to_read);
		if (rc == 0) 
		{
			errno = ECONNRESET;
			rc = -1;
			return rc;
		}

		/* Moves the pointer to receive other data */
		p_msg = &(p_msg[rc]);
		/* Sums bytes received */
		msg_length += rc;
		/* Computes remaining bytes */
		length_to_read -= rc;

		if (length_to_read > 0) 
		{
		 /* If no character at the buffer wait
		  TIME_OUT_END_OF_TRAME before raising an error. */
		 tv.tv_sec = 0;
		 tv.tv_usec = 50000;
		}
	}
	
	/* Display the hex code of each character received */
	//HEX_to_ASCII_Convertor (p_msg, msg_length, desc);
	//printf("rtuReceiveMsg:%s\n",desc);
	
	return msg_length;	
}

ST_UCHAR cal_Xor(ST_UCHAR * data,int count)
{
	ST_UCHAR verify = 0x00;
	int i;
	
	for(i=0;i<count;i++)
	{
		verify = verify^data[i];
	}
	return verify;
}

/*判断字符串是否全是可见字符*/
int isPrintStr(ST_UCHAR *inpacket,int len)
{
	int i;

	for(i=0;i<len;i++)
	{
		if(!isprint(inpacket[i]))
		{
			return SD_FALSE;
		}
	}
	return SD_TRUE;
}

int  Pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if (pthread_mutex_lock(&(*mutex)) != 0) {
	perror("pthread_mutex_lock");
	return 0;
    }
	return 1;
}

int  Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    if (pthread_mutex_unlock(&(*mutex)) != 0) {
		perror("pthread_mutex_unlock");
		return 0;
    }
	return 1;
}

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

ST_UCHAR* data_encode(ST_UCHAR *buf, ST_UCHAR* data,ST_UINT datalen)
{
   memcpy(buf, data, datalen);
   return buf + datalen;
}


//================================================================
/*
 * 输    入: 
 * 输    出: 
 * 功    能: CreatTable16
 * 创建人员: 朱超 2014
 */
static INT32U CreatTable16(INT32U uiPoly, INT32U uiIndex)
{
    INT32U uiData = 0;
    INT32U uiAccum = 0;
    INT8U  uci = 0;
    
    uiData = (INT32U)(uiIndex << 8);
    for (uci = 0; uci < 8; uci++)
    {
        if ((uiData ^ uiAccum) & 0x8000) 
            uiAccum = (uiAccum << 1) ^ uiPoly;
        else 
            uiAccum <<= 1;
		
        uiData <<= 1;
    }
	
    return (uiAccum);
}

void UShortToUChar(INT16U uiAccum,INT8U *pucCRC)
{
	pucCRC[0] = uiAccum/0x100;
	pucCRC[1] = uiAccum%0x100;
}

/*
 * 输    入: 
 * 输    出: 
 * 功    能: Crc16计算校验
 * 创建人员: 朱超 2014
 */
void Crc16(const INT8U *sucData, INT16U usLen, INT8U *pucCRC)
{
    INT16U usi = 0;
    INT8U *p = (INT8U*)sucData;
    
    INT32U uiIndex = 0;
    INT32U uiAccum = 0;
    INT32U uiTableCRC = 0;
    
    for (usi = 0; usi < usLen; usi++ )
    {
        uiIndex    = (uiAccum >> 8) ^ (*p);
        uiTableCRC = CreatTable16 (0x8005, uiIndex);
        uiAccum    = (uiAccum << 8) ^ uiTableCRC;
        p++;
    }

	UShortToUChar((INT16U)uiAccum, pucCRC);
    //UIntToUChar(uiAccum, pucCRC);
}

int  inttobcd(int myint)
{
	  return (myint/10)*16+myint%10;
}

int  bcdtoint(int myint)
{
	  return (myint/0x10)*10+myint%0x10;
}

