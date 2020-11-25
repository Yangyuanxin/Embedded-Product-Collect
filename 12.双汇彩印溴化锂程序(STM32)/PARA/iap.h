#ifndef _IAP_H
#define _IAP_H


#ifndef _IAP_C
#define EXT_IAP	extern 
#else
#define EXT_IAP
#endif





EXT_IAP u8 WritePara(u32 * p_data,u8 length);

#endif
