#define _LCD_INTERFACE_C

#include "head.h"

UINT16 bak_key_value;	

void SetAddr(void);
void LiuliangStatus(void);
void JiaYaoStatus(void);

void DevInf(void);

/*
液晶人机交互界面显示处理任务
*/
void TaskLcdInf(void *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	

	
	GUI_Initialize();
	GUI_SetColor(1,0);	
    
    //如果参数无效，提示用户输入参数。
    //如果参数无效，提示用户输入参数。
    if (para_enable_flag == 0)
    {
        GUI_PutString16_16(0, 0, "请设置地址",0);

        GUI_PutString8_8(0, 20, VER);
		GUI_PutString8_8(0, 30, VER_DATA);
        GUI_PutString8_8(0, 38, "----------------------------------");
		GUI_PutString16_16(0, 48, PRO_NAME,0);

    }  
    else
    {
        GUI_PutString8_8(0, 0, "----------------------------------");
		GUI_PutString8_8(0, 10, "START ...");
		GUI_PutString8_8(0, 20, VER);
		GUI_PutString8_8(0, 30, VER_DATA);
        GUI_PutString8_8(0, 38, "----------------------------------");
		GUI_PutString16_16(0, 48, PRO_NAME,0);  
    }  
    
	
	REFURBISH_LCD();

    OSTimeDly(OS_TICKS_PER_SEC);
    OSTimeDly(OS_TICKS_PER_SEC);
    OSTimeDly(OS_TICKS_PER_SEC);
    
	CLR_SCREEN();
	while (1)
	{		
		// 2011-02-10.HYMAN
		// 更改成简单的人机界面
		MainView();        
		
		WaitKey(2,0xffff);
		
		//设置地址
		if (bak_key_value == KEY_SET)
		{
			SetAddr();
		}
		//显示当前温度
		else if (bak_key_value == KEY_DEC)
		{
			LiuliangStatus();			
		}
		//显示YX区数据
		else if (bak_key_value == KEY_ADD)
		{
            JiaYaoStatus();			
		}
		//显示YX区数据
		else if (bak_key_value == KEY_ESC)
		{
            DevInf();			
		}
		
		//		
		// 		tmpitem = Logo(tmpitem);
		// 		tmpitem->Execute(tmpitem);
		// 		tmpitem = tmpitem->ct;
		
		//可以在里面加上一个计时函数进行返回首页面。
		//实现的方法是： 	1\tmpitem = 0;
		//					2\cur_key_value= KEY_ESC
	}
}

void SetAddr(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	
	
    u8 temp_addr;
    
	char str_buf[10];
	
    CLR_SCREEN();	
    GUI_PutStringTier(0,0,(INT8*)"修改地址",1);
    
    GUI_PutString16_16(20, 10, "当前地址",0);
	
	sprintf(str_buf,"%03d",para.value.addr);
	
	GUI_PutString16_16(20+(strlen("当前地址")+1)*8, 10, str_buf,0);
	
    GUI_PutString16_16(20, 36, "更改地址",0);
    REFURBISH_LCD();
    
    temp_addr = para.value.addr;
	
	while (1)
	{      
		OSTimeDly(OS_TICKS_PER_SEC/50);		
		
		
		sprintf(str_buf,"%03d",temp_addr);		
		GUI_PutString16_16(20+(strlen("更改地址")+1)*8, 36, str_buf,1);
		REFURBISH_LCD();
		
		
		WaitKey(0,0xffff);		
        
        switch (bak_key_value)
        {
		case KEY_SET:
			//保存，并且返回
			para.value.addr = temp_addr;
			
			SavePara();
			CLR_SCREEN();
			GUI_PutString16_16(25, 16, "修改完成!",0);
            REFURBISH_LCD();
			
			WaitKey(500,0xffff);
           	
			return;
		case KEY_ESC:
			//保存，并且返回
			CLR_SCREEN();
			GUI_PutString16_16(20, 16, "放弃修改!",0);
            REFURBISH_LCD();
			
            WaitKey(500,0xffff);	
			return;
		case KEY_ADD:
			temp_addr++;
			break;
		case KEY_DEC:
			temp_addr--;
			break;
            
        }	
		
	}
}


// 在一定时间内等待按键,如果时间为0，则是一直等待
// 如果KEY值为0，则为清除上次按键，
// 0xffff为可响应所有按键，并不清除。
// 其余键值则是只等待那个键
void WaitKey(s16 time_long,u16 key)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	
	
    
    if (time_long)
    {
		while (1)
		{
			OSTimeDly(OS_TICKS_PER_SEC/50);				
			
			
            
			if ((--time_long <= 0) || (cur_key_value))
			{
				if (key == 0)
				{
					OS_ENTER_CRITICAL();        
					
					cur_key_value = 0;	
					bak_key_value = 0;
					
					OS_EXIT_CRITICAL();
					
					break;
				}
				
				else if ((key == 0xffff) || (key == cur_key_value))
				{
					OS_ENTER_CRITICAL();        
					
					
					bak_key_value = cur_key_value;
					cur_key_value = 0;	
					
					OS_EXIT_CRITICAL();
					
					break;
				}
			}
		}
    }
    else 
	{
		while (1)
		{
          LED_Run();
          
          OSTimeDly(OS_TICKS_PER_SEC/50);
          
			if (cur_key_value)
				
				if (key == 0)
				{
					OS_ENTER_CRITICAL();        
					
					cur_key_value = 0;	
					bak_key_value = 0;
					
					OS_EXIT_CRITICAL();
					
					break;
				}
				
				else if ((key == 0xffff) || (key == cur_key_value))
				{
					OS_ENTER_CRITICAL();        
					
					
					bak_key_value = cur_key_value;
					cur_key_value = 0;	
					
					OS_EXIT_CRITICAL();
					
					break;
				}
		}
	}
	
}






// 温度状态，显示所有的逻辑相关事件


void LiuliangStatus(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	   
    
	char str_buf[32];
	
    CLR_SCREEN();	
	
	
    GUI_PutStringTier(0,0,(INT8*)"流量功能",1);
       
	// 	
	
	// 	
    
    //REFURBISH_LCD();
    
    
	
	while (1)
	{      
		OSTimeDly(OS_TICKS_PER_SEC/50);			
			
		sprintf(str_buf,"Pulse : %d",liuliangjiMaster_all_times);
		GUI_PutString(17, 0, str_buf);

		//
        sprintf(str_buf,"#3 single: %fML",ll_single_jiayao3_value);
		GUI_PutString(17, 8, str_buf);
		
		sprintf(str_buf,"#3 NEED: %.2fML",jlb3_all_times);
		GUI_PutString(17, 16, str_buf);	

		sprintf(str_buf,"#3 OUT : %.2fML",jlb3_need_times);
		GUI_PutString(17, 24, str_buf);	

		sprintf(str_buf,"#4 single: %fML",ll_single_jiayao4_value);
		GUI_PutString(17, 32, str_buf);
		
		sprintf(str_buf,"#4 NEED: %.2fML",jlb4_all_times);
		GUI_PutString(17, 40, str_buf);	
		
		sprintf(str_buf,"#4 OUT : %.2fML",jlb4_need_times);
		GUI_PutString(17, 48, str_buf);	

		sprintf(str_buf,"CurLL: %05d",yc_buf[ZJ_AI_LL] );//ZJ_AI_LL
		Adj_xiaoshudian(str_buf,3);
		GUI_PutString(17, 56, str_buf);
		GUI_PutString(100, 56, "L/M");
		

		REFURBISH_LCD();


		
		WaitKey(50,0xffff);		
        
        switch (bak_key_value)
        {
		
		case KEY_ESC:	
			return;
            
        }	
		
	}
}

void JiaYaoStatus(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	   
    

	
    CLR_SCREEN();	
	
	
    //GUI_PutStringTier(0,0,(INT8*)"液位",1);
    
      
    //REFURBISH_LCD();
    
    
	
	while (1)
	{      
		OSTimeDly(OS_TICKS_PER_SEC/50);		
		
        if (GetBit_Yx(B_ZJ_YW_L))
        {
		  GUI_PutString16_16(0, 0, "液位低",1);
        }
        else
        {
          GUI_PutString16_16(0, 0, "液位低",0);
        }
        
        //
        if (GetBit_Yx(B_ZJ_YALI_L))
        {
		  GUI_PutString16_16(0, 16, "压力低",1);
        }
        else
        {
          GUI_PutString16_16(0, 16, "压力低",0);
        }
        
        //
        if (GetBit_Yx(B_ZJ_PAIWU_OPENED))
        {
		  GUI_PutString16_16(0, 32, "阀全开",1);
        }
        else
        {
          GUI_PutString16_16(0, 32, "阀全开",0);
        }
        
        //
        if (GetBit_Yx(B_ZJ_PAIWU_CLOSED))
        {
		  GUI_PutString16_16(0, 48, "阀全关",1);
        }
        else
        {
          GUI_PutString16_16(0, 48, "阀全关",0);
        }
        
        //
        if (GetBit_Yx(B_ZJ_JIAYAO1_L))
        {
		  GUI_PutString16_16(48, 0, "1罐低",1);
        }
        else
        {
          GUI_PutString16_16(48, 0, "1罐低",0);
        }
        
         //
        if (GetBit_Yx(B_ZJ_JIAYAO2_L))
        {
		  GUI_PutString16_16(48, 16, "2罐低",1);
        }
        else
        {
          GUI_PutString16_16(48, 16, "2罐低",0);
        }
        
         //
        if (GetBit_Yx(B_ZJ_JIAYAO3_L))
        {
		  GUI_PutString16_16(48, 32, "3罐低",1);
        }
        else
        {
          GUI_PutString16_16(48, 32, "3罐低",0);
        }
        
         //
        if (GetBit_Yx(B_ZJ_JIAYAO4_L))
        {
		  GUI_PutString16_16(48, 48, "4罐低",1);
        }
        else
        {
          GUI_PutString16_16(48, 48, "4罐低",0);
        }
        
         //
        if (yc_buf[ZJ_MODBUS_YC_AUTO_MODE])
        {
		  GUI_PutString16_16(88, 0, "自动",1);
        }
        else
        {
          GUI_PutString16_16(88, 0, "自动",0);
        }
        
         //
        if (GetBit_Yx(B_ZJ_STOP))
        {
		  GUI_PutString16_16(88, 16, "急停",1);
        }
        else
        {
          GUI_PutString16_16(88, 16, "急停",0);
        }
        
        
        if (GetBit_Yx(B_ZJ_JIAYAO1_REMOTE))
        {
		  GUI_PutString16_16(88, 32, "1",1);
        }
        else
        {
          GUI_PutString16_16(88, 32, "1",0);
        }
        //
        if (GetBit_Yx(B_ZJ_JIAYAO2_REMOTE))
        {
		  GUI_PutString16_16(104, 32, "2",1);
        }
        else
        {
          GUI_PutString16_16(104, 32, "2",0);
        }
        //
        if (GetBit_Yx(B_ZJ_JIAYAO3_REMOTE))
        {
		  GUI_PutString16_16(88, 48, "3",1);
        }
        else
        {
          GUI_PutString16_16(88, 48, "3",0);
        }
        //
        if (GetBit_Yx(B_ZJ_JIAYAO4_REMOTE))
        {
		  GUI_PutString16_16(104, 48, "4",1);
        }
        else
        {
          GUI_PutString16_16(104, 48, "4",0);
        }
        
       
        
		REFURBISH_LCD();


		
		WaitKey(50,0xffff);		
        
        switch (bak_key_value)
        {
		
		case KEY_ESC:	
			return;
            
        }	
		
	}
}


void DevInf(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	   
    
	char str_buf[32];
	
    CLR_SCREEN();	
	
	
    GUI_PutStringTier(0,0,(INT8*)"排污界面",1);
	
	// 	
	
	// 	
    
    REFURBISH_LCD();
    
    
	
	while (1)
	{      
		OSTimeDly(OS_TICKS_PER_SEC/50);			
		//1
/*		sprintf(str_buf,"ZHU 1# :%d",yc_buf[ZJ_AI_MST_1_VAL]);
		GUI_PutString(17, 0, str_buf);

		sprintf(str_buf,"Rx:%d,Tx:%d",mst_modbus_state[0].rece_byte_num,mst_modbus_state[0].send_byte_num);
		GUI_PutString(17, 8, str_buf);
		//2
		sprintf(str_buf,"CONG 1# :");
		GUI_PutString(17, 16, str_buf);
		
		sprintf(str_buf,"Rx:%d,Tx:%d",mst_modbus_state[1].rece_byte_num,mst_modbus_state[1].send_byte_num);
		GUI_PutString(17, 24, str_buf);

		//3
		sprintf(str_buf,"ZHU 2# :%d",yc_buf[ZJ_AI_MST_2_VAL]);
		GUI_PutString(17, 32, str_buf);
		
		sprintf(str_buf,"Rx:%d,Tx:%d",mst_modbus_state[2].rece_byte_num,mst_modbus_state[2].send_byte_num);
		GUI_PutString(17, 40, str_buf);
		//4
		sprintf(str_buf,"CONG 2# :");
		GUI_PutString(17, 48, str_buf);
		
		sprintf(str_buf,"Rx:%d,Tx:%d",mst_modbus_state[3].rece_byte_num,mst_modbus_state[3].send_byte_num);
		GUI_PutString(17, 56, str_buf);
	*/	
		
        sprintf(str_buf,"STATUS# :%d",yc_buf[ZJ_AI_PAIWU_STATE]);
		GUI_PutString(17, 0, str_buf);

		sprintf(str_buf,"paiwu_num:%d",paiwu_num);
		GUI_PutString(17, 8, str_buf);
        
        sprintf(str_buf,"sys_tick:%d",sys_tick);
		GUI_PutString(17, 24, str_buf);

		//3
		sprintf(str_buf,"delay:%d",process_delay);
		GUI_PutString(17, 32, str_buf);
		
		REFURBISH_LCD();
		
		
		
		WaitKey(10,0xffff);		
        
        switch (bak_key_value)
        {
			
		case KEY_ESC:	
			return;
            
        }	
		
	}
}
