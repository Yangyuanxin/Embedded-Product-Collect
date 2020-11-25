#ifndef _KEY_H
#define _KEY_H


#ifndef _KEY_C
#define EXT_KEY	extern 
#else
#define EXT_KEY
#endif

#define KEY_SET		(1<<0)
#define KEY_ADD		(1<<1)
#define KEY_DEC		(1<<2)
#define KEY_ESC	    (1<<3)

//µ±Ç°°´¼üÉ¨Ãè


//
/********
#define GET_KEY_SET()		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)	
#define GET_KEY_ADD()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)	
#define GET_KEY_DEC()		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13)	
#define GET_KEY_ESC()		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_14)
******************/
EXT_KEY UINT8 key1,key2,key3,key_buffer,focus,key_times;


EXT_KEY void InitKey(void);
EXT_KEY void ProKey(void);
EXT_KEY u8 take_key(void);
EXT_KEY u16 cur_key_value;

#endif

