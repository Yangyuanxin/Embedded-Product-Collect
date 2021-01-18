#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h" 
#define BM_CLK PBin(0)// 旋转编码器CLK输入
#define BM_DT PBin(3)// 旋转编码器DT输入
#define BM_NULL 	0x05//编码器无事件
#define BM_up 		0x06//编码器顺时针旋转
#define BM_down 	0x07//编码器逆时针旋转
#define KEY_enter 	0x08//确定按键事件标志
#define KEY_esc		0x09//返回按键事件标志
#define KEY_home	0x0A//主界面按键事件标志
#define NUM_up 		0x0B//确定按键事件标志
#define NUM_down 	0x0C//确定按键事件标志

typedef struct // 构造按键初始化类
{
	GPIOMode_TypeDef GPIO_Mode; // 初始化按键模式
	GPIO_TypeDef* GPIOx; // 初始化按键口
	uint16_t GPIO_Pin_x; // 初始化按键引脚好
	uint32_t RCC_APB2Periph_GPIOx; // 初始化时钟
}Key_Init;

typedef enum _KEY_STATUS_LIST // 按键状态
{
	KEY_NULL = 0x00, // 无动作
	KEY_SURE = 0x01, // 确认状态
	KEY_UP   = 0x02, // 按键抬起
	KEY_DOWN = 0x03, // 按键按下
	KEY_LONG = 0x04, // 长按
}KEY_STATUS_LIST;

typedef struct _KEY_COMPONENTS // 状态机类
{
    FunctionalState KEY_SHIELD; //按键屏蔽，DISABLE(0):屏蔽，ENABLE(1):不屏蔽
	uint8_t KEY_COUNT;        	//按键长按计数
    BitAction KEY_LEVEL;        //最终按键状态，按下Bit_SET(1)，抬起Bit_RESET(0)
    BitAction KEY_DOWN_LEVEL;   //按下时，按键IO实际的电平
    KEY_STATUS_LIST KEY_STATUS;       //按键状态
    KEY_STATUS_LIST KEY_EVENT;        //按键事件
    BitAction (*READ_PIN)(Key_Init Key);//读IO电平函数
}KEY_COMPONENTS;


typedef struct // 按键类
{
	Key_Init Key; // 继承初始化父类
	KEY_COMPONENTS Status; // 继承状态机父类
}Key_Config;


typedef enum // 按键注册表
{
	BM_SW,
	KEY_NUM, // 必须要有的记录按钮数量，必须在最后
}KEY_LIST;

extern volatile u16 menuEvent[4];

void KEY_Init(void);//IO初始化
void Creat_Key(Key_Init* Init); // 初始化按钮函数
void ReadKeyStatus(void); // 状态机函数
void TIM3_Init(u16 arr,u16 psc);
void encoder_Init(void);//IO初始化
#endif
