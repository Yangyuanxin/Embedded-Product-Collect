#define _FORM_SOE_REPORT_C
#include "user_head.h"
/*
模块：<事件报告>
----------------
在此菜单下的所有子菜单

   事件报告	|---事件浏览
			|
			|
			|---事件清除


对显示方式及如何处理作如下说明：
	1.显示字串格式
		每个SOE分两行显示，	第一行显示SOE类型 + 	动作(合)\复归(分)
							动作值+单位 + 时间
			----------------------------------
	例：	断路器合位继	合
			2007-10-18 21:59:59:369
			过流一段I时限	动作
			2007-10-18 21:59:59:369		2.01A
			----------------------------------
	按规约规定：
		

	得到的事件数据块的数据结构如下：
	0	事件
	1	SOE事件数
	以下每14个字节为一组，组定义请参见规约
*/


void DispSoeReport(struct _rq_t *item)
{	
	UINT8 max_line;
	UINT8 temp_need_pluse;	//需要更改显示

	UINT16 bak_key_value;

	UINT8 soe_num;	
	//召唤SOE数据
	SetProRunStatus(STA_SEQ_EVENT_CALL_FLAG);
	ResetParaStatus(ENABLE_DATA_FLAG);
	if(!( ExeCallForm((UINT8 *)"SOE事件召唤中,请稍候",0)))
	{
		ResetProRunStatus(STA_SEQ_EVENT_CALL_FLAG);
		item->ct = item->cancel;
		return;
	}
	//显示SOE
	soe_num = share_buf[1];

	if ((share_buf[0] != FRAME_TYPE_SEQ_EVENT))	//非法检查
	{
		GUI_PutString16_16(0,0,(INT8*)"错误!错误!非法事件块,请检查",0);
		REFURBISH_LCD();				//刷新显示屏
		//此处开始等待用户按键返回
		WaitHitKeyReturn(item,0);
		return;
	}
	if (0 == soe_num)
	{
		GUI_PutString16_16(0,0,(INT8*)"无事件记录",0);
		REFURBISH_LCD();				//刷新显示屏
		//此处开始等待用户按键返回
		WaitHitKeyReturn(item,0);
		return;
	}
	
	CurHideCur();		

	max_line = soe_num * 2;		//每个SOE显示两行
	item->df_line = 0;
	item->cur_line = 0;
	temp_need_pluse = 1;
		
	while (1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/20);
	
		if (temp_need_pluse)
		{
			UINT8 *p,i;
			UINT16 evt_no;		//对象号	
						
			INT8 temp_str[48];

			temp_need_pluse = 0;
			CLR_SCREEN();
			//开始显示事件，每屏两个
			for (i=item->df_line; (i<item->df_line+4)&&(i<max_line); i++)
			{
				p = &share_buf[2+(i/2)*14];		//指向SOE事件缓冲区块
				evt_no = (*(p+1)<<8) + *p;		//得到对象号
				
				
				if (evt_no == 0xFF)
				{					
					GUI_PutString16_16(32,(i-item->df_line)*2*8,"SOE对象号未对应类型",0);
					i++;
					sprintf(temp_str,"EVT: %04x",evt_no);
					GUI_PutString16_16(32,(i-item->df_line)*2*8,temp_str,0);
				}
				else
				{
					//开始显示SOE				
					GUI_PutString16_16(0,(	i-item->df_line)*2*8,
											(INT8*)soe_type_list[evt_no],
											0);
					//显示动作或复归等
					if (*(p+11) & 0x08)		//合分性质
					{
						if (*(p+11) & 0x10)
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"合",
												0);
						}
						else
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"分",
												0);
						}
					}
					else					//动作/复归性质 
					{
						if (*(p+11) & 0x10)
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"动作",
												0);
						}
						else
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"复归",
												0);
						}
					}
					//第一行显示完毕，以下显示需换行
					i++;
					//显示时间标					
					sprintf(temp_str,"%04d-%02d-%02d",(*(p+9)<<8) + (*(p+8))+2000,*(p+7),*(p+6));
					GUI_PutString8_8(0,(i-item->df_line)*2*8,temp_str);
					sprintf(temp_str,"%02d:%02d:%02d.%03d",
									*(p+5),
									*(p+4),
									((*(p+3)<<8) + (*(p+2)))/1000,
									((*(p+3)<<8) + (*(p+2)))%1000);
					GUI_PutString8_8(0,(i-item->df_line)*2*8+8,temp_str);
					//显示动作值
					if(*(p+11) & 0x80)	//动作值有效
					{						
						UINT32 value;
						UINT8 dot;						
						
						value = ((UINT32)*(p+13)<<8) + (UINT32)(*(p+12));								
						
						//value1中是结果,转换成字串
						sprintf(temp_str,"[%05d",value & 0xffff);
						//小数点处理
						dot = (*(p+10) & 0xE0)>>5;	
						if (dot)
						{
							UINT8 temp1,temp2;
				  			//			  			
				  			temp1 = strlen(temp_str);
				  			temp_str[temp1+1] = 0;
				  			
				  			for (temp2=0; temp2<dot; temp2++)
				  			{
				  				temp_str[temp1] = temp_str[temp1-1];
				  				temp1--;
				  			}  		
				  			temp_str[temp1] = '.';
						}
						//显示出来动作值
						GUI_PutString16_16(	96,(i-item->df_line)*2*8,temp_str,0);
						//单位显示			
						GUI_PutString16_16(	96+strlen(temp_str)*8,
											(i-item->df_line)*2*8,
											(int8*)soe_unit[*(p+10)&0x1F],
											0);
					}
					

				}					
			}
			//-------------------------------------------------------------
			//竖滚动条
			if (item->df_line != 0)
			{
				GUI_PutString16_16(GUI_LCM_XMAX-16,0,"∧",1);
			}
			else
			{
			//	GUI_PutString16_16(GUI_LCM_XMAX-16,0,"  ",0);
			}
			if ((item->df_line+4) < max_line)
			{
				GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"∨",1);
			}
			else
			{
			//	GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"  ",0);
			}
			//-------------------------------------------------------------
		}
		REFURBISH_LCD();
		//处理按键
		//进行按键处理
		while (0 == cur_key_value)
		{
			OSTimeDly(OS_TICKS_PER_SEC/20);			
		}
		//
	    OS_ENTER_CRITICAL();
	    
	    bak_key_value = cur_key_value;		
		cur_key_value = 0;	
		
		OS_EXIT_CRITICAL();
		//
		switch (bak_key_value)
		{
			case KEY_ESC:
				ResetProRunStatus(STA_SEQ_EVENT_CALL_FLAG);
				item->ct = item->cancel;
				return;
			case KEY_DOWN:					
				if ((item->df_line+4) < max_line)
				{					
					//item->df_line++;
					item->df_line+=4;
					
					temp_need_pluse = 1;
					//GUI_RectangleFill(32,0,192,64,0);
				}
				break;
			case KEY_UP:
				if (4 <= item->df_line)
				{					
					item->df_line-=4;
					
					temp_need_pluse = 1;
					//GUI_RectangleFill(32,0,192,64,0);
				}
				break;
		}		
	}
}

void ClrSoeReport(struct _rq_t *item)
{
	CLR_SCREEN();
	//提示不支持的事件显示
	GUI_PutString16_16(16,16,(INT8*)"SORRY,SOE CLR DISABLE!",0);	
	GUI_PutString8_8(0,32,(INT8*)"FormSoeReport.c");
	GUI_PutString8_8(0,48,(INT8*)"ClrSoeReport");
	REFURBISH_LCD();
	//此处开始等待用户按键返回
	WaitHitKeyReturn(item,0);
	return;
}


