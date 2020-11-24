#include "key.h"
#include "stm32f10x.h"
#include "sys.h"

void Key_init(void)
{
 //KEY0 PE4
 //KEY1 PE3
 //KEY2 PE2
 //KEY_UP PA0
    GPIO_InitTypeDef GPIO_Initstruct;
    
    //--------------GPIO时钟初始化-=-------------------//
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
    //--------------KEY0初始化-------------------------//
    GPIO_Initstruct.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Initstruct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_Initstruct);   
    
}


//int Key_scan(void)
//{
//    static unsigned char state=1;  //按键状态
//    unsigned char returnvalue=6;
//    unsigned char KEY=0x00;
//    KEY=0x00;
//    if(KEY1==0) KEY|=0x02;
//    if(KEY2==0) KEY|=0x04;
//    if(KEY3==1) KEY|=0x08; 
//    
//    switch(state)
//    {
//        case 1:if(KEY!=0x00) state=2; break;
//        case 2:if(KEY!=0x00) 
//               {       
//                    switch(KEY)
//                    {
//                         case 0x01:returnvalue=1;break;// 需要
//                         case 0x02:returnvalue=2;break;
//                         case 0x04:returnvalue=3;break;
//                         case 0x08:returnvalue=4;break;
//                         default:  returnvalue=0;break;
//                    }
//                    state=3;
//                }
//                else
//                state=1;
//                break;
//        case 3:if(KEY==0x00)
//                {
//                    state=1;
//                }break;
//                
//    }
//    return returnvalue;
//}


