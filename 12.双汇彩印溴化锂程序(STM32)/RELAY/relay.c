
#define _RELAY_C
#include "head.h"

//#define DBG_RELAY		//debug 输出信息开关



#ifdef DBG_RELAY
INT8 dbg_relay_buf[128];
u32 dbg_relay_time;
u8 dbg_relay_flag;
#endif

#define OPEN_PAIWU_CONDITION    1
#define CLOSE_PAIWU_CONDITION   2

#define REMOTE_OPEN_PAIWU_FLAG       1
#define REMOTE_CLOSE_PAIWU_FLAG    2

//-------------------------------------------------------------
u8 Jiayao1_start_flag;	//。
u8 Jiayao2_start_flag;	//。
u8 Jiayao1_hand_flag;	//。
u8 Jiayao2_hand_flag;	//。


u8 LL_Jiayao3_start_flag;	//根据流量流量每次进入时要置一下位。
u8 LL_Jiayao4_start_flag;	//根据流量流量每次进入时要置一下位。
u8 Jiayao5_start_flag;	//
u8 Jiayao6_start_flag;	//
u8 Jiayao7_start_flag;	//
u8 Jiayao5_hand_flag;	//。
u8 Jiayao6_hand_flag;	//。

//计量泵1
u32 jiayao1_zhouqi;		//计量泵周期时间
u32 jiayao1_cur_time;	//计量泵当前周期
//计量泵2
u32 jiayao2_zhouqi;		//计量泵周期时间
u32 jiayao2_cur_time;	//计量泵当前周期
//计量泵3
u32 jiayao3_zhouqi;		//计量泵周期时间
u32 jiayao3_cur_time;	//计量泵当前周期
//计量泵4
u32 jiayao4_zhouqi;		//计量泵周期时间
u32 jiayao4_cur_time;	//计量泵当前周期
//计量泵5
u32 jiayao5_zhouqi;		//计量泵周期时间
u32 jiayao5_cur_time;	//计量泵当前周期
//计量泵6
u32 jiayao6_zhouqi;		//计量泵周期时间
u32 jiayao6_cur_time;	//计量泵当前周期
//计量泵7
u32 jiayao7_zhouqi;		//计量泵周期时间
u32 jiayao7_cur_time;	//计量泵当前周期

//-------------------------------------------------------------





//---------------------------------------------------------
//hyman2011-04-20
void JiaYao1(void);
void JiaYao2(void);
void JiaYao3(void);
void JiaYao4(void);

void JiaYao5(void);
void JiaYao6(void);

void JiaYao7(void);



// "3#传感器值控制1#计量泵工作加药，当值高于高限时开始加药，当值低于低限时停止加药，
// 1#加药罐液位低时不加药，
// 当压力低时不加药
// 手动运行时可以随意设定加药速度
// 加药速度的计算：如果大于高限，以最大速度加药（100次/分）
// 低于高限后，（当前值-低限值）/(高限值-低限值） * 100"




 
void Init_Relay(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	//  paiwu_counter=500;
	Jiayao1_start_flag = 0;
	Jiayao2_start_flag = 0;
	Jiayao1_hand_flag = 0;
	Jiayao2_hand_flag = 0;
	LL_Jiayao3_start_flag = 0;	
	LL_Jiayao4_start_flag = 0;

    //4个计量泵默认成自动加药模式。要想用手动的加药速度，则需要特殊权限或远程GPRS更改。
    ResetBit_Yx(B_ZJ_JIAYAO1_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO2_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO3_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO4_REMOTE);
	ResetBit_Yx(B_ZJ_JIAYAO5_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO6_REMOTE);
}



void ProLogic(void)
{ 

    JiaYaoLogic();//	 

}



void SetBit_Yx(u8 bit)
{
	u8 yx_num;
	u8 yx_bit;
	
	yx_num = bit/16;
	yx_bit = bit%16;
	
	yx_buf[yx_num] |= (1<<yx_bit);
	
}
void ResetBit_Yx(u8 bit)
{
	u8 yx_num;
	u8 yx_bit;
	yx_num = bit/16;
	yx_bit = bit%16;
	yx_buf[yx_num] &= (~(1<<yx_bit));
}

u8 GetBit_Yx(u8 bit)
{
	if (yx_buf[bit/16] & (1<<(bit%16)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// 继电器输出
void OutJdq(void)
{
	//1
	if (GetBit_Yx(B_ZJ_JDQ_1))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	}
	//2
	if (GetBit_Yx(B_ZJ_JDQ_2))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	}
	//3
	if (GetBit_Yx(B_ZJ_JDQ_3))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	//4
	if (GetBit_Yx(B_ZJ_JDQ_4))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
	//5
	if (GetBit_Yx(B_ZJ_JDQ_5))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	}
	//6
	if (GetBit_Yx(B_ZJ_JDQ_6))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_13);
	}
	//7
	if (GetBit_Yx(B_ZJ_JDQ_7))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_15);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	}
	//8
	if (GetBit_Yx(B_ZJ_JDQ_8))
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_3);
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	}
	//9
	if (GetBit_Yx(B_ZJ_JDQ_9))
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_4);
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_4);
	}
	//10
	if (GetBit_Yx(B_ZJ_JDQ_10))
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_5);
	}
}


void JiaYao1(void)
{
	u32 temp_k;
	u16 temp2;

	


	//for debug
	//yc_buf[ZJ_AI_MST_3_VAL] = 1100;
	
	//控制1#加药泵
	if (!(GetBit_Yx(B_ZJ_JIAYAO1_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI)
			|| (yc_buf[ZJ_AI_MST_3_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_3])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//加药停止时，要把为了LOAD SYS_TICK 的启动加药位清0.
			Jiayao1_start_flag = 0;
			
			// 等于0时，就是停止计量泵工作。此处需要把泵停止掉
			yc_buf[ZJ_AI_JIAYAO_SUDU_1] = 0;
			
		}
		//------------------------------------------------------------------------
		//开始加药处理
		//高时，启动计量泵，泵速根据差值进行。
		else if (yc_buf[ZJ_AI_MST_3_VAL] > yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_3])
		{
			if (Jiayao1_start_flag == 0)
			{	
				Jiayao1_start_flag = 1;
				
				jiayao1_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_3]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_MST_3_VAL];
		}

		Jiayao1_hand_flag = 0;
			
		temp_k = 0;
		//如果处在加药状态，则开始计算加药速度
		if (1 == Jiayao1_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_3]) * 100/(yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_3] - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_3]);
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_1] = temp_k;
		
		
		
	}
	else	// 如果是手动状态下，要使用后台设定的加药速度
	{	
		if (Jiayao1_hand_flag == 0)
		{
			Jiayao1_hand_flag = 1;
			
			jiayao1_cur_time = sys_tick;
		}

		Jiayao1_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_1] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_1];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//下面开始根据每分钟需要加多少次进行相应换算
	// 60秒，60000MS,12000(5MS).每半周期是6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_1])
	{
		jiayao1_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_1];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao1_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//1#泵
	if (jiayao1_zhouqi)
	{
		if ((sys_tick - jiayao1_cur_time ) >= jiayao1_zhouqi)
		{
			jiayao1_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO1_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_1))
				{
					//如果 有泵工作了，就记录输出药液量
					float *p;
					u16 temp;
					
					jlb1_need_times += 0.48;
					
					//同时更新到YC里去。hyman2011-12-21
					p = (float*)&yc_buf[52];
					*p = jlb1_need_times;
					
					temp = yc_buf[53] ;
					yc_buf[53] = yc_buf[52];
					yc_buf[52] = temp;						

					ResetBit_Yx(B_ZJ_JDQ_1);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_1);

				}
			}			
		}		
	}
	
	//急停处理
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI) 								 
		)		
	{
		ResetBit_Yx(B_ZJ_JDQ_1);
		
		ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_1] = 0;
	}
	else
	{
		if (jiayao1_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);	
		}
	}	
	
}




void JiaYao2(void)
{
	u32 temp_k;
	u16 temp2;

	


	//for debug
	//yc_buf[ZJ_AI_MST_2_VAL] = 1100;
	
	//控制1#加药泵
	if (!(GetBit_Yx(B_ZJ_JIAYAO2_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI)
			|| (yc_buf[ZJ_AI_MST_2_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_2])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//加药停止时，要把为了LOAD SYS_TICK 的启动加药位清0.
			Jiayao2_start_flag = 0;
			
			// 等于0时，就是停止计量泵工作。此处需要把泵停止掉
			yc_buf[ZJ_AI_JIAYAO_SUDU_2] = 0;
			
		}
		//------------------------------------------------------------------------
		//开始加药处理
		//高时，启动计量泵，泵速根据差值进行。
		else if (yc_buf[ZJ_AI_MST_2_VAL] > yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_2])
		{
			if (Jiayao2_start_flag == 0)
			{	
				Jiayao2_start_flag = 1;
				
				jiayao2_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_2]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_MST_2_VAL];
		}

		Jiayao2_hand_flag = 0;
			
		temp_k = 0;
		//如果处在加药状态，则开始计算加药速度
		if (1 == Jiayao2_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_2]) * 100/(yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_2] - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_2]);
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_2] = temp_k;
		
		
		
	}
	else	// 如果是手动状态下，要使用后台设定的加药速度
	{	
		if (Jiayao2_hand_flag == 0)
		{
			Jiayao2_hand_flag = 1;
			
			jiayao2_cur_time = sys_tick;
		}

		Jiayao2_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_2] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_2];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//下面开始根据每分钟需要加多少次进行相应换算
	// 60秒，60000MS,12000(5MS).每半周期是6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_2])
	{
		jiayao2_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_2];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao2_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//1#泵
	if (jiayao2_zhouqi)
	{
		if ((sys_tick - jiayao2_cur_time ) >= jiayao2_zhouqi)
		{
			jiayao2_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO2_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_2))
				{
					//如果 有泵工作了，就记录输出药液量
					float *p;
					u16 temp;
					
					jlb2_need_times += 0.48;
					
					//同时更新到YC里去。hyman2011-12-21
					p = (float*)&yc_buf[54];
					*p = jlb2_need_times;
					
					temp = yc_buf[55] ;
					yc_buf[55] = yc_buf[54];
					yc_buf[54] = temp;

					ResetBit_Yx(B_ZJ_JDQ_2);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_2);
				}
			}			
		}		
	}
	
	//急停处理
	//急停处理
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI) 								
		)
	{
		ResetBit_Yx(B_ZJ_JDQ_2);
		
		ResetBit_Yx(B_ZJ_JIAYAO2_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_2] = 0;
	}
	else
	{
		if (jiayao2_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO2_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO2_CONTROL);	
		}
	}
	
	
	
}



// 根据补水量控制3号计量泵实时加药
// 按照每吨水加药***，进行实时处理加药，同步控制3#计量泵，相应罐液位低时不加药

void JiaYao3(void)
{
	//远程控制时，响应远程设定的速度
	if (GetBit_Yx(B_ZJ_JIAYAO3_REMOTE))
	{
		//----------------------------------------------------------------------
			
		if (yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3])
		{
			jiayao3_zhouqi = 6000 / yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3];

			SetBit_Yx(B_ZJ_JIAYAO3_CONTROL);

			if (LL_Jiayao3_start_flag == 0)
			{
				LL_Jiayao3_start_flag = 1;
				jiayao3_cur_time = sys_tick;
			}
            
            yc_buf[ZJ_AI_JIAYAO_SUDU_3] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3];
		}
		else
		{
			jiayao3_zhouqi = 0;
			LL_Jiayao3_start_flag = 0;

			ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);
		}		
		
		//3#泵
		if (jiayao3_zhouqi)
		{
			if ((sys_tick - jiayao3_cur_time ) >= jiayao3_zhouqi)
			{
				jiayao3_cur_time = sys_tick;		
				
				//
				if (GetBit_Yx(B_ZJ_JIAYAO3_CONTROL))
				{
					if (GetBit_Yx(B_ZJ_JDQ_3))
					{
						//如果 有泵工作了，就记录输出药液量
						float *p;
						u16 temp;
						
						jlb3_need_times += 0.48;
						
						//同时更新到YC里去。hyman2011-12-21
						p = (float*)&yc_buf[0];
						*p = jlb3_need_times;
						
						temp = yc_buf[1] ;
						yc_buf[1] = yc_buf[0];
						yc_buf[0] = temp;

						//和技术手册保持同步，为了和以前兼容，故再放到一个地方。
						yc_buf[56] = yc_buf[0];
						yc_buf[57] = yc_buf[1];
                        
						ResetBit_Yx(B_ZJ_JDQ_3);
                         
					}
					else
					{
						SetBit_Yx(B_ZJ_JDQ_3);
					}
				}			
			}		
		}
	}
	else	//自动时，根据流量的量加。
	{
		if (LL_Jiayao3_start_flag == 1)
		{
			//首先把上次的加药过程结束
			if ((sys_tick - jiayao3_cur_time ) >= 50)
			{
				jiayao3_cur_time = sys_tick;
				
				if (GetBit_Yx(B_ZJ_JDQ_3))
				{
					ResetBit_Yx(B_ZJ_JDQ_3);	
				}
				else
				{
					LL_Jiayao3_start_flag = 0;
				}
			}	
		}
		else if ((jlb3_all_times - jlb3_need_times) >= 0.48)
		{
			//开始加一次药
			//开始置位工作状态
			LL_Jiayao3_start_flag = 1;
			
			jiayao3_cur_time = sys_tick;
			
			// 相应罐液位低时不加药
			if (!(GetBit_Yx(B_ZJ_JIAYAO3_L)))
			{
				SetBit_Yx(B_ZJ_JDQ_3);	//置高计量泵脉冲
				SetBit_Yx(B_ZJ_JIAYAO3_CONTROL);
			}					
			
			//如果 有泵工作了，就记录输出药液量
			if (GetBit_Yx(B_ZJ_JDQ_3))
			{ 
                float *p;
                u16 temp;
              
				jlb3_need_times += 0.48;
                
                //同时更新到YC里去。hyman2011-12-21
                p = (float*)&yc_buf[0];
                *p = jlb3_need_times;
                
                temp = yc_buf[1] ;
                yc_buf[1] = yc_buf[0];
                yc_buf[0] = temp;

				//和技术手册保持同步，为了和以前兼容，故再放到一个地方。
				yc_buf[56] = yc_buf[0];
				yc_buf[57] = yc_buf[1];
			}		
		}
		else
		{
			//置
			ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);		
		}
	}	

	//急停处理
	if (GetBit_Yx(B_ZJ_STOP))
	{
		ResetBit_Yx(B_ZJ_JDQ_3);
		
		ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_3] = 0;
	}
	else
	{
		if (LL_Jiayao3_start_flag)
		{
			SetBit_Yx(B_ZJ_JIAYAO3_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);	
		}
	}
}


// 根据补水量控制4号计量泵实时加药
// 按照每吨水加药***，进行实时处理加药，同步控制3#、4#计量泵，相应罐液位低时不加药

void JiaYao4(void)
{
    //for test
    //jlb4_need_times += 0.48;
  
	//远程控制时，响应远程设定的速度
	if (GetBit_Yx(B_ZJ_JIAYAO4_REMOTE))
	{
		//----------------------------------------------------------------------
		
		if (yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4])
		{
			jiayao4_zhouqi = 6000 / yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4];
			
			SetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
			
			if (LL_Jiayao4_start_flag == 0)
			{
				LL_Jiayao4_start_flag = 1;
				jiayao4_cur_time = sys_tick;
			}
            
            yc_buf[ZJ_AI_JIAYAO_SUDU_4] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4];
		}
		else
		{
			jiayao4_zhouqi = 0;
			LL_Jiayao4_start_flag = 0;
			
			ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
		}		
		
		//4#泵
		if (jiayao4_zhouqi)
		{
			if ((sys_tick - jiayao4_cur_time ) >= jiayao4_zhouqi)
			{
				jiayao4_cur_time = sys_tick;		
				
				//
				if (GetBit_Yx(B_ZJ_JIAYAO4_CONTROL))
				{
					if (GetBit_Yx(B_ZJ_JDQ_4))
					{
						//如果 有泵工作了，就记录输出药液量						
						float *p;
						u16 temp;
					  
						jlb4_need_times += 0.48;
						
						//同时更新到YC里去。hyman2011-12-21
						p = (float*)&yc_buf[2];
						*p = jlb4_need_times;
						
						temp = yc_buf[3] ;
						yc_buf[3] = yc_buf[2];
						yc_buf[2] = temp;

						//和技术手册保持同步，为了和以前兼容，故再放到一个地方。
						yc_buf[58] = yc_buf[2];
						yc_buf[59] = yc_buf[3];
						
						ResetBit_Yx(B_ZJ_JDQ_4);
					}
					else
					{
						SetBit_Yx(B_ZJ_JDQ_4);
					}
				}			
			}		
		}
	}
	else	//自动时，根据流量的量加。
	{
		
		if (LL_Jiayao4_start_flag == 1)
		{
			//首先把上次的加药过程结束
			if ((sys_tick - jiayao4_cur_time ) >= 50)
			{
				jiayao4_cur_time = sys_tick;
				
				if (GetBit_Yx(B_ZJ_JDQ_4))
				{
					ResetBit_Yx(B_ZJ_JDQ_4);	
				}
				else
				{
					LL_Jiayao4_start_flag = 0;
				}
			}	
		}
		else if ((jlb4_all_times - jlb4_need_times) >= 0.48)
		{
			//开始加一次药
			//开始置位工作状态
			LL_Jiayao4_start_flag = 1;
			
			jiayao4_cur_time = sys_tick;
			
			// 相应罐液位低时不加药
			if (!(GetBit_Yx(B_ZJ_JIAYAO4_L)))
			{
				SetBit_Yx(B_ZJ_JDQ_4);	//置高计量泵脉冲
				SetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
			}				
			
			//如果 有泵工作了，就记录输出药液量
			if (GetBit_Yx(B_ZJ_JDQ_4))
			{
				float *p;
                u16 temp;
              
				jlb4_need_times += 0.48;
                
                //同时更新到YC里去。hyman2011-12-21
                p = (float*)&yc_buf[2];
                *p = jlb4_need_times;
                
                temp = yc_buf[3] ;
                yc_buf[3] = yc_buf[2];
                yc_buf[2] = temp;
			}		
		}
		else
		{
			//置		
			ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
		}
	}
    
    

	//急停处理
	if (GetBit_Yx(B_ZJ_STOP))
	{
		ResetBit_Yx(B_ZJ_JDQ_4);
		
		ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_4] = 0;
	}
	else
	{
		if (LL_Jiayao4_start_flag)
		{
			SetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);	
		}
	}
	
}

// 5#计量泵受控于冷水的3#传感器，，冷水压力低时停止加药
void JiaYao5(void)
{
	u32 temp_k;
	u16 temp2;

	//for debug
	//yc_buf[ZJ_AI_MST_3_VAL] = 1100;
	
	//控制5#加药泵
	if (!(GetBit_Yx(B_ZJ_JIAYAO5_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGSHUI)			
			|| (yc_buf[ZJ_AI_SLV2_3_VAL] < yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_3])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//加药停止时，要把为了LOAD SYS_TICK 的启动加药位清0.
			Jiayao5_start_flag = 0;
			
			// 等于0时，就是停止计量泵工作。此处需要把泵停止掉
			yc_buf[ZJ_AI_JIAYAO_SUDU_5] = 0;
			
		}
		//------------------------------------------------------------------------
		//开始加药处理
		//高时，启动计量泵，泵速根据差值进行。
		else if (yc_buf[ZJ_AI_SLV2_3_VAL] > yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_3])
		{
			if (Jiayao5_start_flag == 0)
			{	
				Jiayao5_start_flag = 1;
				
				jiayao5_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_3]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_SLV2_3_VAL];
		}

		Jiayao5_hand_flag = 0;
			
		temp_k = 0;
		//如果处在加药状态，则开始计算加药速度
		if (1 == Jiayao5_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_3]) * 100/(yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_3] - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_3]);
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_5] = temp_k;
		
		
		
	}
	else	// 如果是手动状态下，要使用后台设定的加药速度
	{	
		if (Jiayao5_hand_flag == 0)
		{
			Jiayao5_hand_flag = 1;
			
			jiayao5_cur_time = sys_tick;
		}

		Jiayao5_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_5] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_5];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//下面开始根据每分钟需要加多少次进行相应换算
	// 60秒，60000MS,12000(5MS).每半周期是6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_5])
	{
		jiayao5_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_5];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao5_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//5#泵
	if (jiayao5_zhouqi)
	{
		if ((sys_tick - jiayao5_cur_time ) >= jiayao5_zhouqi)
		{
			jiayao5_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO5_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_5))
				{
					//如果 有泵工作了，就记录输出药液量
					float *p;
					u16 temp;
					
					jlb5_need_times += 0.48;
					
					//同时更新到YC里去。hyman2011-12-21
					p = (float*)&yc_buf[60];
					*p = jlb5_need_times;
					
					temp = yc_buf[61] ;
					yc_buf[61] = yc_buf[60];
					yc_buf[60] = temp;

					ResetBit_Yx(B_ZJ_JDQ_5);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_5);
				}
			}			
		}		
	}
	

	//急停处理
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGSHUI) 
		
		)
	
	{
		ResetBit_Yx(B_ZJ_JDQ_5);
		
		ResetBit_Yx(B_ZJ_JIAYAO5_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_5] = 0;
	}
	else
	{
		if (jiayao5_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO5_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO5_CONTROL);	
		}
	}	
	
}

// 6#计量泵受控于冷水的2#传感器，
void JiaYao6(void)
{
	u32 temp_k;
	u16 temp2;

	//for debug
	//yc_buf[ZJ_AI_MST_3_VAL] = 1100;
	
	//控制6#加药泵
	if (!(GetBit_Yx(B_ZJ_JIAYAO6_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGSHUI)
			|| (yc_buf[ZJ_AI_SLV2_2_VAL] < yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_2])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//加药停止时，要把为了LOAD SYS_TICK 的启动加药位清0.
			Jiayao6_start_flag = 0;
			
			// 等于0时，就是停止计量泵工作。此处需要把泵停止掉
			yc_buf[ZJ_AI_JIAYAO_SUDU_6] = 0;
			
		}
		//------------------------------------------------------------------------
		//开始加药处理
		//高时，启动计量泵，泵速根据差值进行。
		else if (yc_buf[ZJ_AI_SLV2_2_VAL] > yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_2])
		{
			if (Jiayao6_start_flag == 0)
			{	
				Jiayao6_start_flag = 1;
				
				jiayao6_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_2]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_SLV2_2_VAL];
		}

		Jiayao6_hand_flag = 0;
			
		temp_k = 0;
		//如果处在加药状态，则开始计算加药速度
		if (1 == Jiayao6_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_2]) * 100/(yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_2] - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_2]);//经处写成了L_3
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_6] = temp_k;
		
		
		
	}
	else	// 如果是手动状态下，要使用后台设定的加药速度
	{	
		if (Jiayao6_hand_flag == 0)
		{
			Jiayao6_hand_flag = 1;
			
			jiayao6_cur_time = sys_tick;
		}

		Jiayao6_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_6] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_6];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//下面开始根据每分钟需要加多少次进行相应换算
	// 60秒，60000MS,12000(5MS).每半周期是6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_6])
	{
		jiayao6_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_6];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao6_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//6#泵
	if (jiayao6_zhouqi)
	{
		if ((sys_tick - jiayao6_cur_time ) >= jiayao6_zhouqi)
		{
			jiayao6_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO6_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_6))
				{
					//如果 有泵工作了，就记录输出药液量
					float *p;
					u16 temp;
					
					jlb6_need_times += 0.48;
					
					//同时更新到YC里去。hyman2011-12-21
					p = (float*)&yc_buf[62];
					*p = jlb6_need_times;
					
					temp = yc_buf[63] ;
					yc_buf[63] = yc_buf[62];
					yc_buf[62] = temp;

					ResetBit_Yx(B_ZJ_JDQ_6);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_6);
				}
			}			
		}		
	}
	
	//急停处理
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGSHUI) )
	{
		ResetBit_Yx(B_ZJ_JDQ_6);
		
		ResetBit_Yx(B_ZJ_JIAYAO6_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_6] = 0;
	}
	else
	{
		if (jiayao6_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO6_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO6_CONTROL);	
		}
	}	
	
}

void JiaYao7(void)
{
	
		
	if (yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7])
	{
		jiayao7_zhouqi = 6000 / yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7];
		
		SetBit_Yx(B_ZJ_JIAYAO7_CONTROL);
		
		if (Jiayao7_start_flag == 0)
		{
			Jiayao7_start_flag = 1;
			jiayao7_cur_time = sys_tick;
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_7] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7];
	}
	else
	{
		jiayao7_zhouqi = 0;
		Jiayao7_start_flag = 0;
		
		ResetBit_Yx(B_ZJ_JIAYAO7_CONTROL);
	}		
	
	//7#泵
	if (jiayao7_zhouqi)
	{
		if ((sys_tick - jiayao7_cur_time ) >= jiayao7_zhouqi)
		{
			jiayao7_cur_time = sys_tick;		
			
			//
			if (GetBit_Yx(B_ZJ_JIAYAO7_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_9))
				{
					//如果 有泵工作了，就记录输出药液量
					float *p;
					u16 temp;
					
					jlb7_need_times += 0.48;
					
					//同时更新到YC里去。hyman2011-12-21
					p = (float*)&yc_buf[64];
					*p = jlb7_need_times;
					
					temp = yc_buf[65] ;
					yc_buf[65] = yc_buf[64];
					yc_buf[64] = temp;

					ResetBit_Yx(B_ZJ_JDQ_9);					
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_9);
				}
			}			
		}		
	}
	
	//急停处理
	if (GetBit_Yx(B_ZJ_STOP) || (	GetBit_Yx(B_ZJ_YALI_L_LENGSHUI) 
									&& GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI2)									
									) 
		)
	{
		ResetBit_Yx(B_ZJ_JDQ_7);
		
		ResetBit_Yx(B_ZJ_JIAYAO7_CONTROL);	
		
		//yc_buf[ZJ_AI_JIAYAO_SUDU_5] = 0;
	}
	else
	{
		if (Jiayao7_start_flag)
		{
			SetBit_Yx(B_ZJ_JIAYAO7_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO7_CONTROL);	
		}
	}
	
}



void JiaYaoLogic(void)
{    
	JiaYao1();
	JiaYao2();
	JiaYao3();    
    JiaYao4(); 
	
	JiaYao5();    
    JiaYao6(); 
	
	JiaYao7(); 
}

