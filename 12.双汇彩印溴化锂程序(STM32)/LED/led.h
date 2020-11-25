#ifndef _LED_
#define _LED_

#ifndef _LED_C
#define EXT_LED extern
#else 
#define EXT_LED
#endif


EXT_LED void LED_Init(void);
EXT_LED void LED_Run(void);

#endif

