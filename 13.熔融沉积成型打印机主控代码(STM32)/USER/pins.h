#ifndef __PINS_H
#define __PINS_H


#define PS_ON_PIN     PFout(12) //目前该引脚是乱分配的，后面要根据实际电路分配引脚

#define X_ENABLE_PIN   PFout(10)
#define X_DIR_PIN      PFout(8)
#define X_STEP_PIN     PFout(9) 

#define Y_ENABLE_PIN   PFout(7)  
#define Y_DIR_PIN      PFout(5) 
#define Y_STEP_PIN     PFout(6)     

#define Z_ENABLE_PIN   PFout(4)
#define Z_DIR_PIN      PFout(2) 
#define Z_STEP_PIN     PFout(3) 

#define E0_ENABLE_PIN  PFout(1) 
#define E0_DIR_PIN     PEout(6) 
#define E0_STEP_PIN    PFout(0)         
    
#define E1_ENABLE_PIN  PEout(5)
#define E1_DIR_PIN	   PEout(3) 
#define E1_STEP_PIN    PEout(4)	 


#define X_MAX_PIN      PAin(4)//4
#define X_MIN_PIN      PAin(5)//5
#define Y_MAX_PIN      PAin(6)//6
#define Y_MIN_PIN      PAin(7)//7
#define Z_MAX_PIN      PCin(4)//4
#define Z_MIN_PIN      PCin(5)//5 

//软件模拟PWM，因此用数字引脚即可
#define  HEATER_0_PIN   PBout(0)	  //E0_PWM  
//#define HEATER_1_PIN  PFout(11)	  //E1_PWM 
#define  HEATER_BED_PIN PFout(13)	  //BED_PWM
#define  FAN_0_PIN      PFout(11)//PBout(1)   	//E0_FAN  //加热头散热片上的风扇
#define  FAN_1_PIN      PFout(12)   //E1_FAN  
#define  FAN_BED_PIN    PFout(14)	  //BED_FAN
#define  FAN_PIN        PBout(1)     //模型散热的风扇，有利于提高打印质量，通过切片软件控制M106 S255

//2016/12/26
//Arduino的ADC模块的分辨率为10位精度而STM32的ADC模块的分辨率为12位精度。目前采用的温度查表仍是
//原固件中的温度查表，故此处需要将精度降低两位(右移2位或除以4)。后面可用python重新生成一下温度查表，
//改成12位精度的值该处就不用右移2了。
//2017/1/12
//通过修改生成温度查表的.py文件，重新生成了适合于stm32应用的温度查表，故将下面定义温度引脚时的右移两位去除掉了。
#define TEMP_0_PIN	   ((Get_Adc(ADC_Channel_10)))   						// PC0 ADC3_10	E0_TEMP
//#define TEMP_1_PIN	 ((Get_Adc(ADC_Channel_11)))   						// PC1 ADC3_11	E1_TEMP
#define TEMP_BED_PIN   ((Get_Adc(ADC_Channel_12)))   						// PC2 ADC3_12  BED_TEMP
#define TEMP_ROOM_PIN  ((Get_Adc(ADC_Channel_13)))               // PC3 ADC3_13  BED_TEMP


#endif //__PINS_H
