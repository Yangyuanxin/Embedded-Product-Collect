#ifndef _UPDATA_CRC_H
#define	_UPDATA_CRC_H

#include "sys.h"

u16 CRC16InRam(u8 *BufPtr, u16 Length, u16 CRC16);
u16 CalCrc16(u8 *buff, u16 ulLen, u16 crc);

#endif

