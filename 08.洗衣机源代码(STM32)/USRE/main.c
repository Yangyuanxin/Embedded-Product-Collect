#include"led.h"
#include"key.h"
#include"delay.h"
#include"LCD12864.H"
#include"PWM.H"
#include"TIMER2.H"
#include<stdint.h>
#include<string.h>
#include"shui.h"
#include"MOTOR.H"
#include"HONG.H"
#include"BEEP.H"
#define mode_piaoxi

u8 i;//公共变量
u8 MODE_WORK=0;
u8 TIM2_5ms_ticks;
u8 TIM2_1s_ticks;
u8 cuuent_mode;

struct 
{
	u8 shuiwei_youxiao:1;
	u8 shuiwei_wuxiao:1;
}SHUI_BIT;


#define LEV_youxiao SHUI_BIT.shuiwei_youxiao


u8 mode0 =0;  //定义此时程序模式。
u8 flag_MODESTOP=0;  //主函数模式位 [0:2] 6种洗涤模式 [3:4]  8种状态信息[5:8]
char *const Dis_str1[]={"标准洗涤","快速洗涤","纤物洗涤","大物洗涤","羊毛洗涤","防皱洗涤"};
char *const Dis_str2[]={"少","中","多"};
char *const Dis_str_xidi="【选择洗涤模式】";  //mode1 dis3
char *const Dis_str_dawe="【  选择档位  】";  //mode2 dis4
char *const Dis_str_jish="【正在进水....】";  //mode3 dis6
char *const Dis_str_runi="【正在洗涤....】";  //mode4 dis7
char *const Dis_str_pais="【正在排水....】";  //mode5 dis8
char *const Dis_str_tuos="【正在脱水....】";  //mode4 dis7
char *const Dis_str_piao="【正在漂洗....】";  //mode4 dis7
char *const Dis_str_susp="【洗涤暂停中..】";  //mode6 dis9
char *const Dis_str_daij="【  待机中    】";  //mode0 dis10
char *const Dis_str_star="【  是否开始? 】"; 
char *const Dis_str_xyco="【  洗衣完成  】"; 

struct  TIME_S
{
	u8 Time_jinshui_s;  //进水低档位
	u8 Time_jinshui_z;  //进水中档位
	u8 Time_jinshui_g;  //进水高档位
	u8 Time_paishui_s;  //进水低档位
	u8 Time_paishui_z;  //进水中档位
	u8 Time_paishui_g;  //进水高档位
};
const struct  TIME_S TIM_shui=
{
	.Time_jinshui_s=125-43,
	.Time_jinshui_z=150-36,
	.Time_jinshui_g=160,
	.Time_paishui_s=125-39,  
	.Time_paishui_z=150-34,
	.Time_paishui_g=160,
};

struct
{
	u8 po_tick1;
}Pointer_count;

struct
{
	u8 Nu_tick1;  //进水
	u16 nu_tick2; //洗涤
	u8 Nu_tick3;  //排水
	u16 Nu_tick5;  //漂洗
	u16 Nu_tick7;   //脱水
	u8 Nu_tick8;   //完成
	u16 Nu_tick9;
}Num_count;

char *Num_str=NULL;
	
struct
{
	u8 f_tick_1s:1;
	u8 count_enable:1;
	u8 cishu_piaoxi:1;  //=1时两次漂洗，0时为一次
	u8 tuoshui_Enable:1; //脱水模式使能
	u8 xidi_Enable:1;    //洗涤模式使能
	u8 piaoxi_Enable:1;  //漂洗使能
}SY_BOOL;


typedef void (*motor_run_sta)(void);

motor_run_sta EVENT_CB[4]={tingzhi,zhengzhuan,zhengzhuan2,fanzhuan};
motor_run_sta CUUENT_CB = tingzhi;
motor_run_sta CBB=NULL;

typedef struct
{
	u8 ticks;
	u8 cuu_sta;
	u8 state_stop_time[4];  //秒数
	u8 stop_time;
	u8 flag_stop;
	u8 motor_state[4];
}WORD_STATE;

WORD_STATE WORK_xidi={.cuu_sta=0,.motor_state={1,0,3,0},.stop_time=28,.state_stop_time={10,14,24,28}};
WORD_STATE WORK_pixi={.cuu_sta=0,.motor_state={1,0,3,0},.stop_time=56,.state_stop_time={20,28,48,56}};
WORD_STATE WORK_tuos={.cuu_sta=0,.motor_state={2},.stop_time=15,.state_stop_time={15,15,15,15}};

void Shui_jiance()
{
	switch(mode0)
	{
		case 4:
		if(((flag_MODESTOP&0x18)>>3)==0)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao))   //SHUI_A 口和 SHUI_B口有水时
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==1)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)) //当A,B,C水位都检测到水时，说明水已注。
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				  Jinshui();
					LED_Jin=0;
					LED_Pai=1;
					LED_Tuo=1;
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==2)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)&&(SHUI_D==LEV_youxiao)) 
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				Jinshui();
					LED_Jin=0;
					LED_Pai=1;
					LED_Tuo=1;
			}
		}
		break;
		case 6:
			if(SHUI_A!=LEV_youxiao)//1
			{
				Tingshui();
				LED_Jin=1;
				LED_Pai=1;
				LED_Tuo=1;				
			}
			else
			{
				Chushui();
				LED_Jin=1;
				LED_Pai=0;
				LED_Tuo=1;				
			}
			break;
		case 7:
		if(((flag_MODESTOP&0x18)>>3)==0)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)) 
			{
				Tingshui();
				LED_Jin=1;
				LED_Pai=1;
				LED_Tuo=1;				
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;				
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==1)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)) 
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;				
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==2)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)&&(SHUI_D==LEV_youxiao)) 
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;				
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;			
			}
		}
		break;	
		case 9:
			LED_Tuo=0;
			LED_Jin=1;
			LED_Pai=1;
			Chushui();
			break;	
		case 10:
			if(SHUI_A!=LEV_youxiao)
			{
				Tingshui();
				LED_Jin=1;
				LED_Pai=1;
				LED_Tuo=1;			
			}
			else
			{
				Chushui();
				LED_Jin=1;
				LED_Pai=0;
				LED_Tuo=1;				
			}
			break;	
		default:
			Tingshui();
			LED_Jin=1;
			LED_Pai=1;
			LED_Tuo=1;	
			break;
	}
}


void Inlet_Init()  //进水
{
	switch((flag_MODESTOP&0x18)>>3)
	{
		case 0:Num_count.Nu_tick1=TIM_shui.Time_jinshui_s;break;
		case 1:Num_count.Nu_tick1=TIM_shui.Time_jinshui_z;break;
		case 2:Num_count.Nu_tick1=TIM_shui.Time_jinshui_g;break;
		default:break;
	}	
}

void drain_Init()  //排水相关初始化
{
	switch((flag_MODESTOP&0x18)>>3)
	{
		case 0:Num_count.Nu_tick3=TIM_shui.Time_paishui_s;break;
		case 1:Num_count.Nu_tick3=TIM_shui.Time_paishui_z;break;
		case 2:Num_count.Nu_tick3=TIM_shui.Time_paishui_g;break;
		default:break;
	}	
}

void Wash_Init() //洗涤相关初始化
{
	switch((flag_MODESTOP&0x07))
	{
		case 0:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=12*60; break;
		case 1:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=1.5*60; break;
		case 2:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=10*60; break;
		case 3:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=15*60; break;
		case 4:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=15*60; break;		
		case 5:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=8*60; break;
		default:break;
	}
	WORK_xidi.ticks=0;	//刷新ticks
	WORK_xidi.cuu_sta=0;
}
void Wash_Stop()
{
	SY_BOOL.xidi_Enable=0;
	WORK_xidi.ticks=0;       //刷新ticks
}
void Rinse_Init()
{
	switch((flag_MODESTOP&0x07))
	{
		case 0:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 1:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 2:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 3:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 4:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 5:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		default:break;
	}
	WORK_pixi.ticks=0;	//刷新ticks
	WORK_pixi.cuu_sta=0;
}

void Rinse_Init_2()
{
	switch((flag_MODESTOP&0x07))
	{
		case 0:SY_BOOL.cishu_piaoxi=1;break;
		case 1:SY_BOOL.cishu_piaoxi=0;break;
		case 2:SY_BOOL.cishu_piaoxi=1;break;
		case 3:SY_BOOL.cishu_piaoxi=1;break;	
		case 4:SY_BOOL.cishu_piaoxi=1;break;
		case 5:SY_BOOL.cishu_piaoxi=1;break;
		default:break;
	}	
}

void Rinse_Stop()
{
	SY_BOOL.piaoxi_Enable=0;
}
void Tuoshui_Init()  //脱水相关初始化
{
	switch(flag_MODESTOP&0x07)
	{
		case 0:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=6*60; break;
		case 1:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=3*60; break;
		case 2:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=3*60; break;
		case 3:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=7*60; break;
		case 4:SY_BOOL.tuoshui_Enable=0; Num_count.Nu_tick7=0*60; break;		
		case 5:SY_BOOL.tuoshui_Enable=0; Num_count.Nu_tick7=0*60; break;
		default:break;
	}
	WORK_tuos.ticks=0;	//刷新ticks
	WORK_tuos.cuu_sta=0;
}

void Tuoshui_Stop()
{
	SY_BOOL.tuoshui_Enable=0;  //禁止洗涤
}

void display0()  // await 
{
	Lcd12864_WriAdd(0,1);
	Lcd12864_PriStr("STM32");
	Lcd12864_WriAdd(0,4);
	Lcd12864_PriStr("洗衣机");
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr("                ");
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_daij);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            开始");
	Lcd12864_SetWhite(3,6,7);
}

void display1()  //wash mode seclection
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr(" [");
	Lcd12864_WriAdd(1,5);
	Lcd12864_PriStr("] ");
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_xidi);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("模式  档位  确定");
	Lcd12864_SetWhite(3,0,1);
	Lcd12864_SetWhite(3,6,7);
}

void display2()  //water level mode seclection
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr("  ");
	Lcd12864_WriAdd(1,5);
	Lcd12864_PriStr(" [");
	Lcd12864_WriAdd(1,7);
	Lcd12864_PriStr("] ");
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_dawe);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("模式  档位  确定");
	Lcd12864_SetWhite(3,6,7);
	Lcd12864_SetWhite(3,3,4);
}
 

void display3()  //isstart interface
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr("  ");
	Lcd12864_WriAdd(1,5);
	Lcd12864_PriStr("  ");
	Lcd12864_WriAdd(1,7);
	Lcd12864_PriStr("  ");
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_star);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("模式  档位  开始");
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_SetWhite(3,6,7);
}

void display4()  //进水界面
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_jish);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("                ");
}

void display5()  //runing interface
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_runi);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            暂停");
	Lcd12864_SetWhite(3,6,7);
}

void display6()  //排水界面
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_pais);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("                ");
}

void display7()  //rinse 
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_piao);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            暂停");
	Lcd12864_SetWhite(3,6,7);
}

void display8()  //tuoshui
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_tuos);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            暂停");
	Lcd12864_SetWhite(3,6,7);
}

void display9()  //wash complish
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_xyco);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            确定");
	Lcd12864_SetWhite(3,6,7);
}

void display10()  //suspend
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_susp);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("        重新开始");
	Lcd12864_SetWhite(3,4,7);
}

void display11()
{
	Lcd12864_WriAdd(2,0);	
	Lcd12864_PriStr("【  异常！！  】"); 
	Lcd12864_WriAdd(3,6);	
	Lcd12864_PriStr("开始");
	Lcd12864_SetWhite(3,6,7);
}

void Calc_tick9()
{
	Num_count.Nu_tick9=0;
	switch((flag_MODESTOP&0x07))
	{
		case 0:Num_count.Nu_tick9+=(12*60+56*2+6*60+8); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*2;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*2;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*2;break;
							default:break;
						}	
		break;
		case 1:Num_count.Nu_tick9+=(1.5*60+56+3*60+5); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*2+TIM_shui.Time_paishui_s*1;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*2+TIM_shui.Time_paishui_z*1;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*2+TIM_shui.Time_paishui_g*1;break;
							default:break;
						}	break;
		case 2:Num_count.Nu_tick9+=(10*60+56*2+3*60+8); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*2;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*2;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*2;break;
							default:break;
						}	break;
		case 3:Num_count.Nu_tick9+=(15*60+56*2+7*60+8); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*2;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*2;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*2;break;
							default:break;
						}	break;					
		case 4:Num_count.Nu_tick9+=(15*60+56*2+8);
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*3;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*3;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*3;break;
							default:break;
						}	break;
		case 5:Num_count.Nu_tick9+=(8*60+56*2+8);
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*3;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*3;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*3;break;
							default:break;
						}	break;
		default:break;
	}
}

void key1_Hander(void)
{
	u8 temp=0;
	switch(mode0)
	{
		case 1:
			{
				temp=flag_MODESTOP&0x07;
				temp++;
				if(temp>5)
				{
					temp=0;
				}
				flag_MODESTOP=(flag_MODESTOP&0xf8)|(temp&0x07);
			}
			break; //洗涤模式选择界面
		default:break;
	}
}

void key2_Hander(void)
{
	u8 temp=0;
	switch(mode0)
	{
		case 1:break; //洗涤界面
		case 2:
		{
			temp=(flag_MODESTOP&0x18)>>3;
			temp++;
			if(temp>2)
				temp=0;
			flag_MODESTOP=(flag_MODESTOP&0xe7)|((temp<<3)&0x18);
		}break; //档位选择界面
		case 5:Num_count.nu_tick2=Num_count.nu_tick2-40;Num_count.Nu_tick9=Num_count.Nu_tick9-40;break;
		case 8:Num_count.Nu_tick5=Num_count.Nu_tick5-40;Num_count.Nu_tick9=Num_count.Nu_tick9-40;break;
		case 9:Num_count.Nu_tick7=Num_count.Nu_tick7-40;Num_count.Nu_tick9=Num_count.Nu_tick9-40;break;
		default:break;
	}
}
void key3_Hander(void)
{
	switch(mode0)
	{
		case 0:mode0=1;display1();break; //待机界面
		case 1:mode0=2;display2();break; //洗涤界面
		case 2:mode0=3;display3();break; //模式选择界面
		case 3:
			mode0=4;
			Rinse_Init_2();
			Inlet_Init();
			Calc_tick9();//计算总计时长短。
			SY_BOOL.count_enable=1;
			display4();break; //将要进入进水界面
		case 4:break;
		case 5: //洗涤界面->暂停界面
			SY_BOOL.count_enable=0;
			mode0=12;
			display10();
			break;
		case 6:break; //排水界面
		case 7:break; //进水界面
		case 8:       //漂洗界面->暂停界面
			mode0=13;
			SY_BOOL.count_enable=0;
			display10();
			break; 
		case 9:break; //排水界面
		case 10:  		//脱水界面->暂停界面3
			mode0=14;
			display10();
			SY_BOOL.count_enable=0;
			break; 
		case 11:mode0=0;display0();break;  //完成界面->待机界面
		case 12:       //暂停界面1->洗涤界面
			SY_BOOL.count_enable=1;
			mode0=5;
			display5();
			break;  
		case 13:       //暂停界面2->漂洗界面
			SY_BOOL.count_enable=1;
			mode0=8;
			display7();
			break;  
		case 14:       //暂停界面3->脱水界面
			SY_BOOL.count_enable=1;
			mode0=10;
			display8();
			break;  
		case 15:
			if(Gaizi()==0)  //盖子合上
			{
				mode0=cuuent_mode;
				SY_BOOL.count_enable=1;
				beep=1;
				switch(mode0)
				{
					case 5:display5();break;
					case 8:display7();break;
					case 9:display8();break;
					case 12:
					case 13:
					case 14:display10();break;
					default:break;
				}
			}break;
		default:break;
	}
}

void display()
{
	switch(mode0)
	{
		case 0: display0(); break;
		case 1: display1(); break;
		case 2: display2(); break;
		case 3: display3(); break;
		case 4: display4(); break;
		case 5: display5(); break;
		case 6: display6(); break;
		case 7: display4(); break;
		case 8: display7();break;
		case 9: display6();break;
		case 10:display8();break;
		case 11:display9();break;
		case 12:display10();break;
		case 13:display10();break;
		case 14:display10();break;
	}
}

void Motor_Run()
{
	if(CBB!=CUUENT_CB)
	{
		CBB=CUUENT_CB;
		if(*CBB==zhengzhuan)
			zhengzhuan();
		else if (*CBB==fanzhuan)
			fanzhuan();
		else if (*CBB==tingzhi)
			tingzhi();
		else if(*CBB==zhengzhuan2)
			zhengzhuan2();
	}
}

void Active_action()
{
	switch(mode0)
	{
		case 5:
			if(SY_BOOL.xidi_Enable==0)//判断洗涤时间是否结束,0=结束
			{
				
			}
			else  //没有则执行洗涤操作
			{
				if((WORK_xidi.ticks)>=WORK_xidi.stop_time)  //当前模式结束
				{
					WORK_xidi.flag_stop=1; 	//激活当前状态停止标志
					WORK_xidi.ticks=0;       //刷新ticks
					WORK_xidi.cuu_sta=0;     //工作状态初始化为0
				}else if((WORK_xidi.ticks)>=WORK_xidi.state_stop_time[WORK_xidi.cuu_sta])   //当前状态结束
				{
					WORK_xidi.flag_stop=0; 	//激活当前状态停止标zhi
					WORK_xidi.cuu_sta++;     //工作状态+1
					CUUENT_CB=EVENT_CB[WORK_xidi.motor_state[WORK_xidi.cuu_sta]];
				}else                /*没有达到到改变状态的时间*/
				{
					WORK_xidi.flag_stop=0; 	//激活当前状态停止标zhi
					 CUUENT_CB=EVENT_CB[WORK_xidi.motor_state[WORK_xidi.cuu_sta]];
				}
				WORK_xidi.ticks++;
			}break;
			case 8:  //漂洗  
					if(SY_BOOL.piaoxi_Enable==0) //判断是否可以进入下一个状态
					{

					}
					else
					{
						if(((WORK_pixi.ticks)++)>=WORK_pixi.stop_time)  //当前模式结束
						{
							WORK_pixi.flag_stop=1; 	//激活当前状态停止标志
							WORK_pixi.ticks=0;       //刷新ticks
							WORK_pixi.cuu_sta=0;     //工作状态初始化为0
						}else if((WORK_pixi.ticks)>=WORK_pixi.state_stop_time[WORK_pixi.cuu_sta])   //当前状态结束
						{
							WORK_pixi.flag_stop=0; 	//激活当前状态停止标zhi
							WORK_pixi.cuu_sta++;     //工作状态+1
							CUUENT_CB=EVENT_CB[WORK_pixi.motor_state[WORK_pixi.cuu_sta]];
						}else                /*没有达到到改变状态的时间*/
						{
							WORK_pixi.flag_stop=0; 	//激活当前状态停止标zhi
							CUUENT_CB=EVENT_CB[WORK_pixi.motor_state[WORK_pixi.cuu_sta]];
						}
					}break;
			case 9:	 //脱水 
				if(SY_BOOL.tuoshui_Enable==0) //判断是否可以进入下一个环节
				{

				}
				else
				{
					if((WORK_tuos.ticks)>=WORK_tuos.stop_time)  //当前模式结束
					{
						WORK_tuos.flag_stop=1; 	//激活当前状态停止标志
						WORK_tuos.ticks=0;       //刷新ticks
						WORK_tuos.cuu_sta=0;     //工作状态初始化为0
					}else if((WORK_pixi.ticks)>=WORK_pixi.state_stop_time[WORK_tuos.cuu_sta])   //当前状态结束
					{
						WORK_tuos.flag_stop=0; 	//激活当前状态停止标zhi
						WORK_tuos.cuu_sta++;     //工作状态+1
					}else                /*没有达到到改变状态的时间*/
					{
						WORK_tuos.flag_stop=0; 	//激活当前状态停止标zhi
						CUUENT_CB=EVENT_CB[WORK_tuos.motor_state[WORK_tuos.cuu_sta]];
					}
					WORK_tuos.ticks++;
				}break;
			default:CUUENT_CB=EVENT_CB[0];break;
			}
		}

void key_scan(void)
{
	static u8 key_state=1;
	u8 keytemp=0;
	if(KEY1==0)
		keytemp|=0x01;
	else if(KEY2==0)
		keytemp|=0x02;
	else if (KEY3==0)
		keytemp|=0x04;
	switch(key_state)
	{
		case 1: if(keytemp!=0x00) key_state=2;break;
		case 2: if(keytemp==0x00) key_state=1;
						else 
						{
							switch(keytemp)
							{
								case 0x01:key1_Hander();break;
								case 0x02:key2_Hander();break;
								case 0x04:key3_Hander();break;
								default:break;
							}
							key_state=3;
						}break;
		case 3: if(keytemp==0x00) key_state=1;break;
	}
}

void pointer_action()
{
	SY_BOOL.f_tick_1s=0;
	switch(mode0)
	{
		case 0:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
				
		case 4:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 5:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
 				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 6:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 7:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 8:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 9:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 10:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		default:break;
	}
}

char* Ass_itos(u16 num)  //将传入的数字变成字符型
{
	char *num_str=NULL;
	if(num>60)
	{
		num_str[0]=(num/60)/10+0x30;
		num_str[1]=(num/60)%10+0x30;
		num_str[2]='m';
		num_str[3]='\0';
	}
	else
	{
		num_str[0]=num/10+0x30;
		num_str[1]=num%10+0x30;
		num_str[2]='s';
		num_str[3]='\0';
	}
	return num_str;	
}

void display_action()
{
	char str[10];
	char str1[6];
	char str2[16];
	switch(mode0)
	{
		case 4:  //进水
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick1>60)
			{
				str2[6]=(Num_count.Nu_tick1/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick1/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick1/10+0x30;
				str2[7]=Num_count.Nu_tick1%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			}
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 5:  //洗涤
			if(Num_count.Nu_tick9>60)
			{
				str[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str[2]='m';
				str[3]=' ';
			}
			else
			{
				str[0]=Num_count.Nu_tick9/10+0x30;
				str[1]=Num_count.Nu_tick9%10+0x30;
				str[2]='s';
				str[3]=' ';
			}
			str[4]=' ';
			str[5]=' ';
			if(Num_count.nu_tick2>60)
			{
				str[6]=(Num_count.nu_tick2/60)/10+0x30;
				str[7]=(Num_count.nu_tick2/60)%10+0x30;
				str[8]='m';
				str[9]='\0';
			}
			else
			{
				str[6]=Num_count.nu_tick2/10+0x30;
				str[7]=Num_count.nu_tick2%10+0x30;
				str[8]='s';
				str[9]='\0';
			}
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str);
			break;
		case 6: //排水
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick3>60)
			{
				str2[6]=(Num_count.Nu_tick3/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick3/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick3/10+0x30;
				str2[7]=Num_count.Nu_tick3%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 7:
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick1 >60)
			{
				str2[6]=(Num_count.Nu_tick1/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick1/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick1/10+0x30;
				str2[7]=Num_count.Nu_tick1%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			}  
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 8:
			if(Num_count.Nu_tick9>60)
			{
				str[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str[2]='m';
				str[3]=' ';
			}
			else
			{
				str[0]=Num_count.Nu_tick9/10+0x30;
				str[1]=Num_count.Nu_tick9%10+0x30;
				str[2]='s';
				str[3]=' ';
			}
			str[4]=' ';
			str[5]=' ';
			if(Num_count.Nu_tick5>60)
			{
				str[6]=(Num_count.Nu_tick5/60)/10+0x30;
				str[7]=(Num_count.Nu_tick5/60)%10+0x30;
				str[8]='m';
				str[9]='\0';
			}
			else
			{
				str[6]=Num_count.Nu_tick5/10+0x30;
				str[7]=Num_count.Nu_tick5%10+0x30;
				str[8]='s';
				str[9]='\0';
			}
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str);
			break;
		case 9:
			if(Num_count.Nu_tick9>60)
			{
				str[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str[2]='m';
				str[3]=' ';
			}
			else
			{
				str[0]=Num_count.Nu_tick9/10+0x30;
				str[1]=Num_count.Nu_tick9%10+0x30;
				str[2]='s';
				str[3]=' ';
			}
			str[4]=' ';
			str[5]=' ';
			if(Num_count.Nu_tick7>60)
			{
				str[6]=(Num_count.Nu_tick7/60)/10+0x30;
				str[7]=(Num_count.Nu_tick7/60)%10+0x30;
				str[8]='m';
				str[9]='\0';
			}
			else
			{
				str[6]=Num_count.Nu_tick7/10+0x30;
				str[7]=Num_count.Nu_tick7%10+0x30;
				str[8]='s';
				str[9]='\0';
			}
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str);
			break;
		case 10:
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick3>60)
			{
				str2[6]=(Num_count.Nu_tick3/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick3/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick3/10+0x30;
				str2[7]=Num_count.Nu_tick3%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			} 
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 11:
			str1[0]=(Num_count.Nu_tick8%100)%10+0x30;
			str1[1]='s';
		  str1[2]=' ';
			str1[3]=' ';
			str1[4]=' ';
			str1[5]=' ';
			Lcd12864_WriAdd(3,3);
			Lcd12864_PriStr(str1);
			break;
		case 15:display11();break;
		default:break;
		 
	}		
}



void count_action()  //显示倒计时
{
	switch(mode0)
	{
		case 4: //进水水倒计时
			Num_count.Nu_tick1--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick1>250)
			{
				Num_count.Nu_tick1=0;
				mode0=5;
				Wash_Init();
				display5();
			}
			break;	
		case 5:  //洗涤计时
			Num_count.nu_tick2--;
			Num_count.Nu_tick9--;
			if(Num_count.nu_tick2>10000)
			{
				Wash_Stop();
				Num_count.nu_tick2=0;
				drain_Init();
				mode0=6;
				display6(); //进入排水界面
			}
			break;
		case 6: //排水计时
			Num_count.Nu_tick3--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick3>250)
			{
				Num_count.Nu_tick3=0;
				mode0=7;
				Inlet_Init();  //进水变量初始化
				display4();  //进入进水界面
			}
			break;
		case 7: //进水
			Num_count.Nu_tick1--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick1>250)
			{
				Num_count.Nu_tick1=0;
				mode0=8;
				Rinse_Init();
				display7();  //进入漂洗界面
			}
			break;
		case 8: //漂洗
			Num_count.Nu_tick5--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick5>10000)
			{
				
				Num_count.Nu_tick5=0;
				if(SY_BOOL.cishu_piaoxi)
				{		
						SY_BOOL.cishu_piaoxi=0;
						drain_Init();
						mode0=6;  //->排水
						display6();
				}
				else
				{
					Rinse_Stop();
  				Tuoshui_Init();
					if(SY_BOOL.tuoshui_Enable)
					{
						mode0=9;
						display8();// 进入脱水模式
					}
					else
					{
						mode0=10;
						drain_Init();
						display6(); //进入排水模式
					}
				}				
			}
			break;
		case 9: //脱水计时
			Num_count.Nu_tick7--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick7>1000)
			{
				Tuoshui_Stop();
				Num_count.Nu_tick7=0;
				mode0=11;
				Num_count.Nu_tick8=5;
				display9();  //洗涤完成界面
			}
			break;
		case 10: //排水计时
			Num_count.Nu_tick3--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick3>250)
			{
				Lcd12864_SetWhite_Init();
				Num_count.Nu_tick3=0;
				mode0=11;
				Num_count.Nu_tick8=5;
				display9();  //->洗涤完成界面
			}
			break;
		case 11: //完成
			Num_count.Nu_tick8--;
			if(Num_count.Nu_tick8>250)
			{
				Num_count.Nu_tick8=0;
				mode0=0;
				display0();  //->待机界面
			}
			Lcd12864_WriAdd(0,0);
			Lcd12864_PriStr("  STM32 洗衣机");
			break;
	}
}
 

void JINJI()   //如果在运行过程中盖子打开
{
	switch(mode0)
	{
			case 5:
			case 8:
			case 9:
				if(Gaizi()==0)  //盖子合上
				{
					SY_BOOL.count_enable=1;break;
				}
				else
				{
					SY_BOOL.count_enable=0;
					beep=0;//打开蜂鸣器
					display11();
					cuuent_mode=mode0;
					mode0=15;
				}
				break;			
			default:SY_BOOL.count_enable=1;break;
	}
}

int main()
{
		SHUI_BIT.shuiwei_youxiao=1;
		SHUI_BIT.shuiwei_wuxiao=0;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		Led_init();
		Shui_init();
		HONG_Init();
		SY_BOOL.count_enable=1;
		Pointer_count.po_tick1=1;
		Key_init(); 						//初始化按键
		delay_init();  					//延时函数初始化
		TIM2_IN_Init(9999,359);  //定时器2初始化
		PWM_Init();             //PWM初始化
		Lcd12864_Init();  			//12864初始化
		Lcd12864_SetWhite_Init();
		Lcd12864_WriAdd(0,0);
		Lcd12864_PriStr("  STM32 洗衣机");
		FMQ_Init();
		display();
		CBB=tingzhi;
		Motor_Init();
    while(1)
    {
			if(mode0==1)
			{
				Lcd12864_WriAdd(1,1);
				Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
			}
			if(mode0==2)
			{
				Lcd12864_WriAdd(1,6);
				Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);		
			}
			if(SY_BOOL.f_tick_1s==1)
			{
				SY_BOOL.f_tick_1s=0;
				pointer_action();
				display_action();
			}
			Motor_Run();
			Shui_jiance();
			JINJI();
    } 
}
//定时器2中断服务程序
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
			key_scan();
		}
		TIM2_5ms_ticks++;  //5MS自增
		if(TIM2_5ms_ticks>=20)
		{
			TIM2_5ms_ticks=0;
			TIM2_1s_ticks++;
			if(SY_BOOL.count_enable)
			{
				count_action();
			}
			Active_action();
			SY_BOOL.f_tick_1s=1;
		}
}



