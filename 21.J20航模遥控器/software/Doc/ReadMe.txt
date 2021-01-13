1.关于兼容1.0版本PCB的说明

	基础版遥控器mini板PCB 1.0版本的CSN/CE/IRQ连线与之后的版本不同，本工程默认适配2.0版本PCB。
	如果您使用的1.0版本PCB，请在Keil5中打开工程后，点击魔法棒(Options for Target...)
	点击选项栏 C/C++ >> Preprocessor Symbols 的define文本框中删除 NRF_PIN 即可。

	mini板PCB 1.0版本 无线模块与STM32的连接：
	NRF24L01模块：
		GND   	电源地
		VCC	接3.3V电源
		CSN	接PA8
		SCK	接PB13
		MISO	接PB14
		MOSI	接PB15
		CE	接PA9
		IRQ	接PB12
		
	mini板PCB 2.0版本 无线模块与STM32的连接（调换了CSN/CE/IRQ）：
	NRF24L01模块：
		GND   	电源地
		VCC	接3.3V电源
		CSN	接PB12
		SCK	接PB13
		MISO	接PB14
		MOSI	接PB15
		CE	接PA8
		IRQ	接PA9

2.关于EC11旋转编码器版本、按键版本之间的兼容

	本工程默认适配EC11旋转编码器版本，如果您想使用按键版本：
	请在Keil5中打开工程后，点击魔法棒(Options for Target...)
	点击选项栏 C/C++ >> Preprocessor Symbols 的define文本框中删除 EC11_VERSION 即可。
	
	===================EC11旋转编码器版本===================
	六个按键：
	1	CH1Left		接PB5	【home】
	2	CH1Right		接PB4
	3	CH2Down		接PB3	【确定】
	4	CH2Up		接PA15	【返回】
	5	CH4Left		接PA12	【数值-】
	6	CH4Right		接PA11	【数值+】
	7 	旋转编码器模块：
		GND	电源地
		VCC	接3.3V电源
		SW	接PB11
		DT	接PB10
		CLK	接PB1
	短按旋转编码器进入菜单，长按旋转编码器/短按CH1Left键退出菜单。

	按键布局建议：
                                                    4
                                                    |
                                                    3                        
                5-------6                           1-------2
	 
                                   -----------------
                               4 |        屏幕          |  7
                                   -----------------

	========================按键版本========================
	七个按键定义：
	1	CH1Left    接PB5	【确定】
	2	CH1Right  接PB4	【返回】
	3	CH2Down 接PB3	【菜单向下】
	4	CH2Up      接PA15	【菜单向上】
	5	CH4Left    接PA12	【数值-】
	6	CH4Right  接PA11	【数值+】
	7	MENU      接PB11	【菜单】
	短按MENU键进入/退出菜单，CH2Down键和CH2Up键轮询菜单。

	按键布局建议：

                       4                        1
	5---+---6                   2
                       3                         7

3.其他引脚说明
	NRF24L01模块：
		GND	电源地
		VCC	接3.3V电源
		CSN	接PB12
		SCK	接PB13
		MISO	接PB14
		MOSI	接PB15
		CE	接PA8
		IRQ	接PA9
	ADC采样：PA0-7
	电池电压检测：PB0
	蜂鸣器：PA10
	OLED显示屏：
		GND   	电源地
		VCC   	接3.3V电源
		SCL   	接PB8（SCL）
		SDA   	接PB9（SDA）
	串口USB-TTL接法：	
		GND   	电源地
		3V3   	接3.3V
		TXD   	接PB7
		RXD   	接PB6
	ST-LINK V2接法：
		GND   	电源地
		3V3   	接3.3V
		SWCLK 	接DCLK
		SWDIO 	接DIO
