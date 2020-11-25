#ifndef __DEBUG_H__
#define __DEBUG_H__
/*******************************************************************************/
/** only for test **/
#if	1
	#define	reversion_GPIOA7()	do{\
									static unsigned char tmp = 0;	\
									if(tmp++ & 1)	\
									{	\
										GPIO_SetBits(GPIOA, GPIO_Pin_7);	\
									}	\
									else	\
									{	\
										GPIO_ResetBits(GPIOA, GPIO_Pin_7);	\
									}	\
								}while(0)
#else
	void reversion_GPIOA7(void)
	{
	    static unsigned char tmp = 0;
	    
	    if(tmp++ & 1)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		}
	}
#endif


#if	1
	#define	output_GPIO7(x) do{\
									if(x)	\
									{	\
										GPIO_SetBits(GPIOA, GPIO_Pin_7);	\
									}	\
									else	\
									{	\
										GPIO_ResetBits(GPIOA, GPIO_Pin_7);	\
									}	\
							}while(0)
#else
	void output_GPIO7(int output)
	{
		if(output)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
		}
		else
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		}
	}
#endif
/*******************************************************************************/
#endif
