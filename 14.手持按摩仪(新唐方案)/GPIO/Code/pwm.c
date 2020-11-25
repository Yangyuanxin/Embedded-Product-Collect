#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "pwm.h"
void pwm_init(void)
{
	//PWM1_P14_OUTPUT_ENABLE; //MOS;
	P14_PushPull_Mode;
	P14=0;
  PWM3_P04_OUTPUT_ENABLE;//rise voltage
	PWM5_P03_OUTPUT_ENABLE;
	PWM_IMDEPENDENT_MODE;
	PWM_CLOCK_FSYS;
	PWMPH = 0;
	PWMPL = 221;//37.3KHZ
	PWM1H=0x00; 
	PWM1L=110;//duty is 50%
	set_CLRPWM;	
}

void moto_run(void)
{
  PWM5_P03_OUTPUT_ENABLE;
	set_SFRPAGE;
	PWM5H=0x00; 
	PWM5L=128;//duty is 74%
	clr_SFRPAGE;
	pwm_start();
}
void moto_stop(void)
{
	PWM5_P03_OUTPUT_DISABLE;
	P03=0;
}
/***************************************
*first voltage
****************************************/
void voltage1(void)
{
	PWM3H=0x00; 
	PWM3L=50 ;//duty is 50%	 66
	pwm_start();
}
void voltage2(void)
{
	PWM3H=0x00; 
	PWM3L=20;//duty is 
	pwm_start();
}
void voltage3(void)
{
	PWM3H=0x00; 
	PWM3L=10;//duty is 
	pwm_start();

}
void voltage4(void)
{
	PWM3H=0x00; 
	PWM3L=0;//duty is 
	pwm_start();
    
}
void pwm_start(void)
{
	set_LOAD;
	set_PWMRUN;
}
void pwm_stop(void)
{
	clr_LOAD;
	clr_PWMRUN;
}