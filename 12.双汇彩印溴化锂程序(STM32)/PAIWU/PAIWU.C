
#define _PAIWU_C
#include "head.h"





// "根据1#主传感器值排污：当值高于高限时开始排污，排一个可设置的固定时间然后停止排污
// 停止排污后，在一个可设置的固定时间内不允许自动开始再排污
// 当值低于低限时停止排污
// 当蒸发冷液位低时停止排污，当压力低时不加药？？？？
// 急停时，停止排污，如果正在排污，会及时关闭排污阀
// 手动运行时，可以开关排污，不受急停影响，不受设定的排污时间限制"

void Open_PaiWu(void);
void Open_PaiWu_ING(void);
void Open_PaiWu_OPENED(void);
void CLOSE_PaiWu_ING(void);
void CLOSE_PaiWu_DELAY(void);

void TaskPAIWU(void *pdata)
{

// #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
//     OS_CPU_SR  cpu_sr = 0;
// #endif



	yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_OPEN_EN;

    paiwu_num = 0;
    
	while (1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/20 );



		if (yc_buf[ZJ_AI_PAIWU_STATE] == STATE_PAIWU_OPEN_EN)
		{
			Open_PaiWu();
		}
		else if (yc_buf[ZJ_AI_PAIWU_STATE] == STATE_PAIWU_OPEN_ING)
		{
			Open_PaiWu_ING();
		}
		else if (yc_buf[ZJ_AI_PAIWU_STATE] == STATE_PAIWU_OPENED)
		{
			Open_PaiWu_OPENED();
		}
		else if (yc_buf[ZJ_AI_PAIWU_STATE] == STATE_PAIWU_CLOSE_ING)
		{
			CLOSE_PaiWu_ING();
		}
		else if (yc_buf[ZJ_AI_PAIWU_STATE] == STATE_PAIWU_DELAY)
		{
			CLOSE_PaiWu_DELAY();
		}
		else
		{
			Open_PaiWu();
		}

		
		
	}

}


// 当排污阀允许打开时，调用此函数
// 此函数判断是否需要打开排污阀
// 打开排污阀的条件：
// 1、自动时，压力不低且值高于高限，
void Open_PaiWu(void)
{
    //如果排污阀处在打开位，则直接跳转到允许关闭位置。 
    if (GetBit_Yx(B_ZJ_PAIWU_OPENED))    
    {
        yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_OPENED;	//转换到已经打开状态			
	    process_delay = sys_tick;
        
        return;
    }
  
	//自动状态
	if (yc_buf[ZJ_MODBUS_YC_AUTO_MODE])	
	{
		//压力低时，不允许打开排污
		if (    //GetBit_Yx(B_ZJ_YALI_L)
			GetBit_Yx(B_ZJ_STOP)
              || GetBit_Yx(B_ZJ_YW_L)
			)
		{
			return;
		}
		
		// for debug
	//	yc_buf[ZJ_AI_MST_1_VAL]  = 500;

		//当值高于高限时开始排污
		if (yc_buf[ZJ_AI_MST_1_VAL] > yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_1] )
        //if (1)  //for test
		{
			SetBit_Yx(B_ZJ_JDQ_7);
			
			yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_OPEN_ING;
			
			process_delay = sys_tick;
            
            paiwu_num++;
		}
	}
	else	//远程状态
	{
		if (GetBit_Yx(B_ZJ_PAIWU_OPEN))
		{
            if (GetBit_Yx(B_ZJ_STOP))
            {
                ResetBit_Yx(B_ZJ_PAIWU_OPEN);
                
            }
            else
            {
               //Reset_Yx(B_ZJ_PAIWU_OPEN);

                SetBit_Yx(B_ZJ_JDQ_7);
                
                yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_OPEN_ING;
                
                process_delay = sys_tick; 
            }
			
		}
	}

	
}

// 打开排污阀的10S内或者是开到位信号到来之前，不允许返回开排污继电器。
void Open_PaiWu_ING(void)
{
	u8 temp_flag;	//作为记录是否有开到位信号

	temp_flag = 0;	

	while ((sys_tick - process_delay) < 7000)	//<35s
	{
		OSTimeDly(OS_TICKS_PER_SEC/50 );

		//判开到位信号
		if (GetBit_Yx(B_ZJ_PAIWU_OPENED))
		{
			temp_flag = 1;
			//break;
		}
	}

	ResetBit_Yx(B_ZJ_JDQ_7);
			
	yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_OPENED;	//转换到已经打开状态
			
	process_delay = sys_tick;

	if (temp_flag == 0)
	{
		//告警
		SetBit_Yx(B_ZJ_PAIWU_OPEN_FAILT);
	}
}

// 排污阀已经打开后，
// 1\排污一段时间就关掉，(现在改成直到排成液位低才停止排污)
// 2\值下降后也关掉
// 急停时也关闭排污
void Open_PaiWu_OPENED(void)
{
	u32 temp_val;

	// 如果是远程打开的排污阀，则不自动关闭
	if (yc_buf[ZJ_MODBUS_YC_AUTO_MODE] == 0)
	{
		ResetBit_Yx(B_ZJ_PAIWU_OPEN);

		//等待对方手动关闭排污阀
		while (1)
		{
			OSTimeDly(OS_TICKS_PER_SEC/20 );

			if (GetBit_Yx(B_ZJ_PAIWU_CLOSE) || GetBit_Yx(B_ZJ_STOP))
			{
				break;
			}


			//如果在开的中间，排污阀开信号告警，则在此时间区间，再次检查是否得到 全开信号，如果得到，则复位告警。
			//判开到位信号
			if (GetBit_Yx(B_ZJ_PAIWU_OPENED))
			{
				ResetBit_Yx(B_ZJ_PAIWU_OPEN_FAILT);				
			}
		}
	}
	else
	{
// 		temp_val = yc_buf[ZJ_MODBUS_YC_PAIWU_TIME] * 200;
		
// 		while ((sys_tick - process_delay) < temp_val)	//
// 		{
// 			OSTimeDly(OS_TICKS_PER_SEC/20 );
// 
// 			//如果在开的中间，排污阀开信号告警，则在此时间区间，再次检查是否得到 全开信号，如果得到，则复位告警。
// 			//判开到位信号
// 			if (GetBit_Yx(B_ZJ_PAIWU_OPENED))
// 			{
// 				ResetBit_Yx(B_ZJ_PAIWU_OPEN_FAILT);				
// 			}
// 			
// 			//已经逝去的时间
// 			yc_buf[ZJ_AI_PAIWUING_TIME] = (sys_tick-process_delay)/200;
// 
// 			//如果值低于下限 或 压力低，则停止排污
// 			if (GetBit_Yx(B_ZJ_YALI_L) || ((yc_buf[ZJ_AI_MST_1_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_1] )))
// 			{
// 				break;
// 			}
// 		}
		
		//hyman 2011-04-25
		// 程序改变成直到排到低限或者是液位低才关闭排污，急停按钮按下后也关闭排污。
        temp_val = yc_buf[ZJ_MODBUS_YC_PAIWU_TIME] * 200;
        
        
		while ((sys_tick - process_delay) < temp_val)
		{
			OSTimeDly(OS_TICKS_PER_SEC/20 );

			//已经逝去的时间
			yc_buf[ZJ_AI_PAIWUING_TIME] = (sys_tick-process_delay)/200;

			//如果在开的中间，排污阀开信号告警，则在此时间区间，再次检查是否得到 全开信号，如果得到，则复位告警。
			////判开到位信号
 			if (GetBit_Yx(B_ZJ_PAIWU_OPENED))
 			{
 				ResetBit_Yx(B_ZJ_PAIWU_OPEN_FAILT);				
 			}
            
            if (yc_buf[ZJ_MODBUS_YC_AUTO_MODE] == 0)
            {
                break;
            }

			//如果值低于下限 或 压力低 或急停，则停止排污
			if (    //GetBit_Yx(B_ZJ_YALI_L) 
				((yc_buf[ZJ_AI_MST_1_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_1] ))
				|| GetBit_Yx(B_ZJ_STOP) 
                || GetBit_Yx(B_ZJ_YW_L) 
                  
				)
			{
				break;
 			}
		}
		
        
//         while (1)
// 		{
// 			OSTimeDly(OS_TICKS_PER_SEC/20 );
// 
// 			//如果在开的中间，排污阀开信号告警，则在此时间区间，再次检查是否得到 全开信号，如果得到，则复位告警。
// 			////判开到位信号
//  			if (GetBit_Yx(B_ZJ_PAIWU_OPENED))
//  			{
//  				ResetBit_Yx(B_ZJ_PAIWU_OPEN_FAILT);				
//  			}
//             
//             if (yc_buf[ZJ_MODBUS_YC_AUTO_MODE] == 0)
//             {
//                 break;
//             }
// 
// 			//如果值低于下限 或 压力低 或急停，则停止排污
// 			if (GetBit_Yx(B_ZJ_YALI_L) 
// 				|| ((yc_buf[ZJ_AI_MST_1_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_1] ))
// 				|| GetBit_Yx(B_ZJ_STOP) 
//                 || GetBit_Yx(B_ZJ_YW_L) 
//                   
// 				)
// 			{
// 				break;
//  			}
// 		}
	}

	SetBit_Yx(B_ZJ_JDQ_8);	//关闭排污阀
	
	yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_CLOSE_ING;	//转换到正在关闭状态
	
	process_delay = sys_tick;
}


// 关闭排污阀的10S内或者是关到位信号到来之前，不允许返回开排污继电器。
void CLOSE_PaiWu_ING(void)
{
	u8 temp_flag;	//作为记录是否有关到位信号
	
	temp_flag = 0;	
	
	while ((sys_tick - process_delay) < 7000)	//<35s
	{
		OSTimeDly(OS_TICKS_PER_SEC/20 );
		
		//判关到位信号
		if (GetBit_Yx(B_ZJ_PAIWU_CLOSED))
		{			
			temp_flag = 1;
		}
	}

	ResetBit_Yx(B_ZJ_JDQ_8);
			
	yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_DELAY;	//转换到已经打开状态
			
	process_delay = sys_tick;

	if (temp_flag == 0)
	{
		//告警
		SetBit_Yx(B_ZJ_PAIWU_CLOSE_FAILT);
	}
}

// 排污阀关闭后，延时到下个周期，才能允许再次排污。转换到远程排污时，该进程自动返回。
void CLOSE_PaiWu_DELAY(void)
{
	u32 temp_val;
	
	//temp_val = 3600 * 200;//yc_buf[ZJ_MODBUS_YC_PAIWU_LOOP] * 200;
    temp_val = yc_buf[ZJ_MODBUS_YC_PAIWU_LOOP] * 200;
	
	while ((sys_tick - process_delay) < temp_val)	//
	{
		OSTimeDly(OS_TICKS_PER_SEC/20 );

		//如果在关的中间，排污阀关信号告警，则在此时间区间，再次检查是否得到 全关信号，如果得到，则复位告警。
		//判关到位信号
		if (GetBit_Yx(B_ZJ_PAIWU_CLOSED))
		{
			ResetBit_Yx(B_ZJ_PAIWU_CLOSE_FAILT);				
		}
		
		//已经逝去的时间
		yc_buf[ZJ_AI_PAIWUED_TIME] = (sys_tick-process_delay)/200;

		//如果是远程遥控状态，则返回
		if (yc_buf[ZJ_MODBUS_YC_AUTO_MODE] == 0)
		{
			ResetBit_Yx(B_ZJ_PAIWU_CLOSE);

			break;
		}
	}
	
	yc_buf[ZJ_AI_PAIWU_STATE] = STATE_PAIWU_OPEN_EN;	//转换到允许打开排污状态
		
}
