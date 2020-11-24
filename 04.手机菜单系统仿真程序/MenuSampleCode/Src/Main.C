/******************************************************************************
 * Main.C - mainº¯ÊýÎÄ¼þ
 *
 *
 * DESCRIPTION: -
 *
 * modification history
 * --------------------
 * 01a, 04/sep/2007, ÁºÑ×²ý written
 * --------------------
 ******************************************************************************/
#include "CPU.H"
#include "Ks0108.H"
//24MHZ  245--->1Ms
void Delayms(U16 Ms)
{
    U8 k;
    while(Ms--){
        for(k = 245; k > 0 ; k--);
    }
}



void MenuMainLoop(void);

void main(void)

{


    //LED0 = 0x00;
    //LED1 = 0x00;

    GUI_Init();
    GUI_Clear();
    while(1){
        MenuMainLoop();
    }

    while(1);
}







