#define _FORM_SYS_MANAGE_C
#include "head.h"
/*
模块：<系统管理>
----------------
在此菜单下的所有子菜单
此处为了节省内存，因为FLASH大，故以代码形式体现。
以后或想放到RAM中，即定义相对应的几个控件变量数组即可。
*/

static u8 comm_para_check_flag;
void R_Sys_Manage_Select_Execute(struct _rq_t *item);
void R_Comm_Manage_Select_Execute(struct _rq_t *item);
void R_JiankongOcxOptExecute(struct _rq_t *item);
void R_GprsOcxOptExecute(struct _rq_t *item);
void R_SensorOcxOptExecute(struct _rq_t *item);
//
void R_Edit_Comm_Execute(struct _rq_t *item);
void R_Edit_Addr_Execute(struct _rq_t *item);
void R_Edit_Password_Execute(struct _rq_t *item);
void R_Edit_Clock_Execute(struct _rq_t *item);
void R_Edit_Disp_Execute(struct _rq_t *item);

void R_Edit_JianKong_Execute(struct _rq_t *item);
void R_Edit_Gprs_Execute(struct _rq_t *item);
void R_Edit_Sensor_Execute(struct _rq_t *item);
void CommPara_load_exe(void);

/*
系统设置执行函数
*/
void R_Sys_Manage_Execute(struct _rq_t *item)
{
	//可以根据控件索引设定初始化工作
	
	rq_first_rank.Execute = R_Sys_Manage_Select_Execute;//gh	  	
    
  	R_Execute(item);
}
void R_Comm_Manage_Execute(struct _rq_t *item)
{
	rq_first_rank.Execute = R_Comm_Manage_Select_Execute;//gh
	rq_first_rank_enter_flag = 0;  	
    
  	R_Execute(item);
}
void R_Comm_Manage_Select_Execute(struct _rq_t *item)
{
	if (rq_comm_manage_menu.cur_ocx == &comm_manage_jiankong)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_JianKong_Execute(item);
		
	}
	else if (rq_comm_manage_menu.cur_ocx == &comm_manage_gprs)
	{
		//rq_first_rank.Execute = R_Edit_Clock_Execute;
		R_Edit_Gprs_Execute(item);
	}	
	else if (rq_comm_manage_menu.cur_ocx == &comm_manage_sensor)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Sensor_Execute(item);
	}
}
void R_Edit_JianKong_Execute(struct _rq_t *item)
{

	if (rq_first_rank_enter_flag) goto abcd;	
	

	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//此段代码主要完成：定值组别的填写工作
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_jiankong_para+3);
		len <<= 8;
		len += *(p_jiankong_para+2);
		
		cur_index = 4;//因为是变量名字是从第4开始
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)jiankong_group_type[*(p_jiankong_para+cur_index)];
			//
			menu_t_ocx[j].x = 0;
			menu_t_ocx[j].y = j*2;			
			//
			menu_t_ocx[j].GetFocus = M_GetFocus;
			menu_t_ocx[j].LossFocus = M_LossFocus;
			menu_t_ocx[j].qbch = M_qbch;
		
			menu_t_ocx[j].Token = &rq_second_rank;
		
			share_plist[j] = &menu_t_ocx[j];
			//
			j++;
			//改变P指针
			cur_index++;
			cur_index += *(p_jiankong_para+cur_index)*3 + 1;//第个变量有4个属性，所以乘以4			
		}
		//
		rq_first_rank.cont = j;
		rq_first_rank.df_line = 0;
		rq_first_rank.cur_line = 0;
		rq_first_rank.Focus = 0;//初始化焦点
		rq_first_rank.x = 4;
		rq_first_rank.y = 0;
		rq_first_rank.Width = 16;
		rq_first_rank.Height = 8;
		
		rq_first_rank.cancel = &rq_comm_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;
	cur_s = 0;
abcd:	CLR_SCREEN();	

		GUI_PutStringTier(0,0,(INT8*)"监控设置",1);

	//指定子容器的执行函数
	rq_second_rank.Execute = R_JiankongOcxOptExecute;
	//
	R_Execute(item);
}
void R_JiankongOcxOptExecute(struct _rq_t *item)
{
	UINT8 i,j;
	UINT8 *p_data;
	
	//初始化定值菜单里面的控件。根据定值参数区内参数
	//-------------------
	//找到当前菜单号，根据菜单号去参数区取参数，然后形成控件组。
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			break;
		}
	}
    
	//找到定值参数块首
	p_data = p_jiankong_para+5;	//第一组的存放定值组别数的地址
	while (i--)
	{
		p_data += (*p_data)*3;
		p_data += 2;		//指向下一组的地址
	}
	//
	i = *p_data;			//当前菜单下的控件数
	//printf("i=%d\r\n",i);
	//根据不同的控件进行不同的初始化
	if (i>0)
	{
		UINT8 text_ocx_num;
		UINT8 list_ocx_num;	
		UINT8 mult_adjust;	
		//UINT8 ipad_t_ocx_num;
		//
		text_ocx_num = 0;
		list_ocx_num = 0;
		mult_adjust = 0;
		
		p_data++;		//定值结构的第一个字节	
						//	|行数B8|定值类型B7-B5|位定值所在位B4-B0|
		//

		for (j=0; j<i; j++)		
		{	            		
            text_t_ocx[text_ocx_num].Type = K_TEXT;
            text_t_ocx[text_ocx_num].Focus = text_ocx_num + list_ocx_num;
            //
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)jiankong_group_type[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = "";	//此处为单位
            
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_jiankong_commpara_buf[*(p_data+2)];            
            
            text_t_ocx[text_ocx_num].Cursor = 0;
            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//多行标志
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }
           // text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;
			/****
            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
			*********/
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
       //     printf("oce_edit_fag");

			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
			text_t_ocx[text_ocx_num].ReadOnly = 1;	
			change_para_type_flag =JIANKONG_EDIT_FLAG;

    
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 3;
		}
	}
	//-------------------
	//
	rq_second_rank.cont = j;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 0;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];

	R_Execute(item);

 	if (ocx_key_type == KEY_SET)
 	{
 		CommPara_load_exe();
 	}	
}
void CommPara_load_exe(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	u8 i;	
//     if(load_fix_first_enter_flag == 1)
//     {
//         CLR_SCREEN();
//     }
	if (change_para_type_flag ==JIANKONG_EDIT_FLAG)
	{
         if( (temp_jiankong_commpara_buf[0] >4) //端口号检查
           ||((temp_jiankong_commpara_buf[1] != 9600)&&(temp_jiankong_commpara_buf[1] != 19200))
           ||(temp_jiankong_commpara_buf[2] >2 )//停止位
           ||(temp_jiankong_commpara_buf[3] >2 )//检验位
          )
        {
            comm_para_check_flag =1;
        }
        else
        {
            for(i=0;i<JIANKONG_PARA_NUM;i++)
            { 
                if(temp_jiankong_commpara_buf[i]  != para_temp.value_group[i+JIANKONG_PARA_OFFSET])
                {
                    lcd_comm_para_change_flag =  1;
                    para_temp.value_group[i+JIANKONG_PARA_OFFSET] = temp_jiankong_commpara_buf[i];
                
                }	
            }
        }
	}
	else if (change_para_type_flag ==GPRS_EDIT_FLAG)
	{
         if( (temp_gprs_commpara_buf[0] >4) //端口号检查
           ||((temp_gprs_commpara_buf[1] != 9600)&&(temp_gprs_commpara_buf[1] != 19200))
           ||(temp_gprs_commpara_buf[2] >2 )//停止位
           ||(temp_gprs_commpara_buf[3] >2 )//检验位
          )
        {
            comm_para_check_flag =1;
        }
        else
        {
            for(i=0;i<GPRS_PARA_NUM;i++)
            {
                if(temp_gprs_commpara_buf[i]  != para_temp.value_group[i+GPRS_PARA_OFFSET])
                {
                    lcd_comm_para_change_flag =  1;
                    para_temp.value_group[i+GPRS_PARA_OFFSET] = temp_gprs_commpara_buf[i];
                
                }		
            }
        }
	}
	else if (change_para_type_flag ==SENSOR_EDIT_FLAG)
	{
         if( (temp_sensor_commpara_buf[0] >4) //端口号检查
           ||(temp_sensor_commpara_buf[1] >4)
           ||((temp_sensor_commpara_buf[2] != 9600)&&(temp_sensor_commpara_buf[2] != 19200))
           ||(temp_sensor_commpara_buf[3] >2 )//停止位
           ||(temp_sensor_commpara_buf[4] >2 )//检验位
          )
        {
            comm_para_check_flag =1;
        }
        else
        {
            for(i=0;i<SENSOR_PARA_NUM;i++)
            {
                if(temp_sensor_commpara_buf[i]  != para_temp.value_group[i+SENSOR_PARA_OFFSET])
                {
                    lcd_comm_para_change_flag =  1;
                    para_temp.value_group[i+SENSOR_PARA_OFFSET] = temp_sensor_commpara_buf[i];
                
                }		
            }
        }
	}
    
	//没有按任何键之前，不改变运行的容器
	//item->ct = &rq_comm_manage_menu;
    if(comm_para_check_flag ==1)
    {
        //重新装载候改之前的数据
        Load_TmepPara_Buf();
        comm_para_check_flag = 0;
        CLR_SCREEN();
        GUI_PutString16_16(16,16,(INT8*)"参数输入错误",0);

        GUI_PutString16_16(16,32,(INT8*)"按任意键返回",0);
        while(1)
        {
            OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
            if (cur_key_value)
            {
                OS_ENTER_CRITICAL();    	
                cur_key_value = 0;
                OS_EXIT_CRITICAL();
                break;	
            }				
        }
    }
    else
    {        
        if(lcd_comm_para_change_flag ==1)
        {
            lcd_comm_para_change_flag = 0;
            SavePara();
            comm_para_change_flag = 1;//
            CLR_SCREEN();
            GUI_PutString16_16(24,16,(INT8*)"参数已固化",0);
    
            GUI_PutString16_16(16,32,(INT8*)"按任意键返回",0);
            while(1)
            {
                OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
                if (cur_key_value)
                {
                    OS_ENTER_CRITICAL();    	
                    cur_key_value = 0;
                    OS_EXIT_CRITICAL();
                    break;	
                }				
            }
        }
        else
        {	
            CLR_SCREEN();
            GUI_PutString16_16(24,8,(INT8*)"定值无改动",0);
            GUI_PutString16_16(32,24,(INT8*)"不用固化",0);
            GUI_PutString16_16(16,40,(INT8*)"按任意键返回",0);
            
            while(1)
            {
                OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
                if (cur_key_value)
                {
                    OS_ENTER_CRITICAL();    	
                    cur_key_value = 0;
                    OS_EXIT_CRITICAL();
                    break;	
                }				
            }
        }
    }
}
void R_Edit_Gprs_Execute(struct _rq_t *item)
{
    if (rq_first_rank_enter_flag) goto abcd;
	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//此段代码主要完成：定值组别的填写工作
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_gprs_para+3);
		len <<= 8;
		len += *(p_gprs_para+2);
		
		cur_index = 4;//因为是变量名字是从第4开始
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)gprs_group_type[*(p_gprs_para+cur_index)];
			//
			menu_t_ocx[j].x = 0;
			menu_t_ocx[j].y = j*2;			
			//
			menu_t_ocx[j].GetFocus = M_GetFocus;
			menu_t_ocx[j].LossFocus = M_LossFocus;
			menu_t_ocx[j].qbch = M_qbch;
		
			menu_t_ocx[j].Token = &rq_second_rank;
		
			share_plist[j] = &menu_t_ocx[j];
			//
			j++;
			//改变P指针
			cur_index++;
			cur_index += *(p_gprs_para+cur_index)*3 + 1;//第个变量有4个属性，所以乘以4			
		}
		//
		rq_first_rank.cont = j;
		rq_first_rank.df_line = 0;
		rq_first_rank.cur_line = 0;
		rq_first_rank.Focus = 0;//初始化焦点
		rq_first_rank.x = 4;
		rq_first_rank.y = 0;
		rq_first_rank.Width = 16;
		rq_first_rank.Height = 8;
		
		rq_first_rank.cancel = &rq_comm_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;
	cur_s = 0;
abcd:	CLR_SCREEN();	

		GUI_PutStringTier(0,0,(INT8*)"无线设置",1);

	//指定子容器的执行函数
	rq_second_rank.Execute = R_GprsOcxOptExecute;
	//
	R_Execute(item);
}
void R_GprsOcxOptExecute(struct _rq_t *item)
{
	UINT8 i,j;
	UINT8 *p_data;
	
	//初始化定值菜单里面的控件。根据定值参数区内参数
	//-------------------
	//找到当前菜单号，根据菜单号去参数区取参数，然后形成控件组。
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			break;
		}
	}
    
	//找到定值参数块首
	p_data = p_gprs_para+5;	//第一组的存放定值组别数的地址
	while (i--)
	{
		p_data += (*p_data)*3;
		p_data += 2;		//指向下一组的地址
	}
	//
	i = *p_data;			//当前菜单下的控件数
	//printf("i=%d\r\n",i);
	//根据不同的控件进行不同的初始化
	if (i>0)
	{
		UINT8 text_ocx_num;
		UINT8 list_ocx_num;	
		UINT8 mult_adjust;	
		//UINT8 ipad_t_ocx_num;
		//
		text_ocx_num = 0;
		list_ocx_num = 0;
		mult_adjust = 0;
		
		p_data++;		//定值结构的第一个字节	
						//	|行数B8|定值类型B7-B5|位定值所在位B4-B0|
		//

		for (j=0; j<i; j++)		
		{	            		
            text_t_ocx[text_ocx_num].Type = K_TEXT;
            text_t_ocx[text_ocx_num].Focus = text_ocx_num + list_ocx_num;
            //
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)gprs_group_type[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = "";	//此处为单位
            
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_gprs_commpara_buf[*(p_data+2)];            
            
            text_t_ocx[text_ocx_num].Cursor = 0;
            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//多行标志
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }
           // text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;
			/****
            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
			*********/
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
       //     printf("oce_edit_fag");

			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
			text_t_ocx[text_ocx_num].ReadOnly = 1;	
			change_para_type_flag =GPRS_EDIT_FLAG;

    
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 3;
		}
	}
	//-------------------
	//
	rq_second_rank.cont = j;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 0;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];

	R_Execute(item);

 	if (ocx_key_type == KEY_SET)
 	{
 		CommPara_load_exe();
 	}	
}
void R_Edit_Sensor_Execute(struct _rq_t *item)
{
    if (rq_first_rank_enter_flag) goto abcd;
	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//此段代码主要完成：定值组别的填写工作
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_sensor_para+3);
		len <<= 8;
		len += *(p_sensor_para+2);
		
		cur_index = 4;//因为是变量名字是从第4开始
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)sensor_group_type[*(p_sensor_para+cur_index)];
			//
			menu_t_ocx[j].x = 0;
			menu_t_ocx[j].y = j*2;			
			//
			menu_t_ocx[j].GetFocus = M_GetFocus;
			menu_t_ocx[j].LossFocus = M_LossFocus;
			menu_t_ocx[j].qbch = M_qbch;
		
			menu_t_ocx[j].Token = &rq_second_rank;
		
			share_plist[j] = &menu_t_ocx[j];
			//
			j++;
			//改变P指针
			cur_index++;
			cur_index += *(p_sensor_para+cur_index)*3 + 1;//第个变量有4个属性，所以乘以4			
		}
		//
		rq_first_rank.cont = j;
		rq_first_rank.df_line = 0;
		rq_first_rank.cur_line = 0;
		rq_first_rank.Focus = 0;//初始化焦点
		rq_first_rank.x = 4;
		rq_first_rank.y = 0;
		rq_first_rank.Width = 16;
		rq_first_rank.Height = 8;
		
		rq_first_rank.cancel = &rq_comm_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;
	cur_s = 0;
abcd:	CLR_SCREEN();	

		GUI_PutStringTier(0,0,(INT8*)"传感设置",1);

	//指定子容器的执行函数
	rq_second_rank.Execute = R_SensorOcxOptExecute;
	//
	R_Execute(item);
}
void R_SensorOcxOptExecute(struct _rq_t *item)
{
	UINT8 i,j;
	UINT8 *p_data;
	
	//初始化定值菜单里面的控件。根据定值参数区内参数
	//-------------------
	//找到当前菜单号，根据菜单号去参数区取参数，然后形成控件组。
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			break;
		}
	}
    
	//找到定值参数块首
	p_data = p_sensor_para+5;	//第一组的存放定值组别数的地址
	while (i--)
	{
		p_data += (*p_data)*3;
		p_data += 2;		//指向下一组的地址
	}
	//
	i = *p_data;			//当前菜单下的控件数
	//printf("i=%d\r\n",i);
	//根据不同的控件进行不同的初始化
	if (i>0)
	{
		UINT8 text_ocx_num;
		UINT8 list_ocx_num;	
		UINT8 mult_adjust;	
		//UINT8 ipad_t_ocx_num;
		//
		text_ocx_num = 0;
		list_ocx_num = 0;
		mult_adjust = 0;
		
		p_data++;		//定值结构的第一个字节	
						//	|行数B8|定值类型B7-B5|位定值所在位B4-B0|
		//

		for (j=0; j<i; j++)		
		{	            		
            text_t_ocx[text_ocx_num].Type = K_TEXT;
            text_t_ocx[text_ocx_num].Focus = text_ocx_num + list_ocx_num;
            //
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)sensor_group_type[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = "";	//此处为单位
            
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_sensor_commpara_buf[*(p_data+2)];            
            
            text_t_ocx[text_ocx_num].Cursor = 0;
            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//多行标志
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }
           // text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;
			/****
            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
			*********/
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
       //     printf("oce_edit_fag");

			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
			text_t_ocx[text_ocx_num].ReadOnly = 1;	
			change_para_type_flag =SENSOR_EDIT_FLAG;

    
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 3;
		}
	}
	//-------------------
	//
	rq_second_rank.cont = j;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 0;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];

	R_Execute(item);

 	if (ocx_key_type == KEY_SET)
 	{
 		CommPara_load_exe();
 	}	
}
void R_Sys_Manage_Select_Execute(struct _rq_t *item)
{
	//可以根据控件索引设定初始化工作
     if (rq_sys_manage_menu.cur_ocx == &sys_manage_addr)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Addr_Execute(item);
	}
	else if (rq_sys_manage_menu.cur_ocx == &sys_manage_password)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Password_Execute(item);
	}
	else if (rq_sys_manage_menu.cur_ocx == &sys_manage_rtc)
	{
		//rq_first_rank.Execute = R_Edit_Clock_Execute;
		R_Edit_Clock_Execute(item);
	}	
	else if (rq_sys_manage_menu.cur_ocx == &sys_manage_disp)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Disp_Execute(item);
	}
  	//如果系统设置还有其它菜单，放到此处
  	//R_Execute(item);
}


/*
地址设置段
*/
void R_Edit_Addr_Execute(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	//查询地址
	UINT8 temp;	
	temp_local_addr = para_temp.value.addr;
	//初始化一个文本控件，填入地址
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "本机地址:";
	text_t_ocx[0].Caption2 = " ";	//此处为单位	
	//
	text_t_ocx[0].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[0].p_t_data = &temp_local_addr ;
	//s	
	text_t_ocx[0].Width = 3;
	//
	text_t_ocx[0].x = 2;
	text_t_ocx[0].y = 3;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;			
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 8;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 254;
	text_t_ocx[0].Min = 1;
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;
				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	
	//
	share_plist[0] = &text_t_ocx[0];
	
	//---------------------------------------------
	rq_first_rank.cont = 1;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 2;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 16;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	//
	//CLR_SCREEN();

	//
    cur_s = 0;
	CLR_SCREEN();
	GUI_PutStringTier(0,0,(INT8*)"地址设置",1);
	//
	temp = para_temp.value.addr ;		//备份原地址
	//
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);
	//调用
	ocx_key_type = 0;
	R_Execute(item);  
	//从执行函数退出后，判一下需要不需要保存操作
	if ((temp != temp_local_addr) && (ocx_key_type == KEY_SET))
	{
		UINT8 temp_flag;
		temp_flag = 0;
		
		temp_flag = ExePasswordComp(0, 0);
		if( temp_flag == 0)
		{
			//
			GUI_PutString16_16(4*8,3*8,"密码错误",0);
			REFURBISH_LCD();
			OSTimeDly(OS_TICKS_PER_SEC/1);
		}
		else if (temp_flag == 1)
		{
			CLR_SCREEN();
			para_temp.value.addr = temp_local_addr;
			SavePara();
			GUI_PutString16_16(24,16,(INT8*)"参数已固化",0);
	
			GUI_PutString16_16(16,32,(INT8*)"按任意键返回",0);
			while(1)
			{
				OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
				if (cur_key_value)
				{
					OS_ENTER_CRITICAL();    	
					cur_key_value = 0;
					OS_EXIT_CRITICAL();
					break;	
				}				
			}
		}
		else if (temp_flag == 2)
		{
			//
		}	
	}	
}
/*
密码设置段
*/
void R_Edit_Password_Execute(struct _rq_t *item)
{
	#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	//查询地址
    temp_local_password = 0;
	//temp_local_password = para_temp.value.password;
	//初始化一个文本控件，填入地址
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "新密码:";
	text_t_ocx[0].Caption2 = " ";	//此处为单位	
	//
	text_t_ocx[0].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[0].p_t_data = &temp_local_password ;
	//s	
	text_t_ocx[0].Width = 4;
	//
	text_t_ocx[0].x = 2;
	text_t_ocx[0].y = 3;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;			
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 8;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 254;
	text_t_ocx[0].Min = 1;
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;
				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	
	//
	share_plist[0] = &text_t_ocx[0];
	
	//---------------------------------------------
	rq_first_rank.cont = 1;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 2;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 16;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	//
	//CLR_SCREEN();

	//
    cur_s = 0;
	CLR_SCREEN();
	GUI_PutStringTier(0,0,(INT8*)"密码设置",1);
	//
	//temp = para_temp.value.addr ;		//备份原地址
	//
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);
	//调用
	ocx_key_type = 0;
	R_Execute(item);  
	//从执行函数退出后，判一下需要不需要保存操作
	if ( (ocx_key_type == KEY_SET))
	{
		UINT8 temp_flag;
		temp_flag = 0;
		
		temp_flag = ExePasswordComp(0, 0);
		if( temp_flag == 0)
		{
			//
			GUI_PutString16_16(4*8,3*8,"密码错误",0);
			REFURBISH_LCD();
			OSTimeDly(OS_TICKS_PER_SEC/1);
		}
		else if (temp_flag == 1)
		{
			CLR_SCREEN();
			para_temp.value.password = temp_local_password;
			SavePara();
			GUI_PutString16_16(24,16,(INT8*)"参数已固化",0);
	
			GUI_PutString16_16(16,32,(INT8*)"按任意键返回",0);
			while(1)
			{
				OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
				if (cur_key_value)
				{
					OS_ENTER_CRITICAL();    	
					cur_key_value = 0;
					OS_EXIT_CRITICAL();
					break;	
				}				
			}
		}
		else if (temp_flag == 2)
		{
			//
		}	
	}	
}
/*
时钟设置段
*/
void R_Edit_Clock_Execute(struct _rq_t *item)
{
	//更新液晶新时间
	UINT16 clock_year;
	UINT16 clock_month,clock_day,clock_hour,clock_minuter,clock_second;
	//可能需要关中断，关联时间

	//
	//初始化年、月、日、时、分、秒 6个控件。
	//年--------------------------------
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "日期:";
	text_t_ocx[0].Caption2 = "";	//此处为单位	
	//
	text_t_ocx[0].ReadOnly = 2;		//数据源长度
	
	text_t_ocx[0].p_t_data = (&clock_year);
	//s	
	text_t_ocx[0].Width = 4;
	//
	text_t_ocx[0].x = 1;
	text_t_ocx[0].y = 2;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);		
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 6;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 2099;
	text_t_ocx[0].Min = 2007;
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	//月--------------------------------
	text_t_ocx[1].Type = K_TEXT;
	text_t_ocx[1].Focus = 1;
	//
	text_t_ocx[1].Caption1 = "-";
	text_t_ocx[1].Caption2 = "";	//此处为单位	
	//
	text_t_ocx[1].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[1].p_t_data = &clock_month;
	//s	
	text_t_ocx[1].Width = 2;
	//
	text_t_ocx[1].x = 10;
	text_t_ocx[1].y = 2;
	//
	text_t_ocx[1].Px = 0;
	text_t_ocx[1].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[1],&text_t_ocx[1].Value);		
				
	text_t_ocx[1].Cursor = 0;
	text_t_ocx[1].location = 11;
	
	text_t_ocx[1].dot = 0;
	text_t_ocx[1].Max = 12;
	text_t_ocx[1].Min = 1;
	text_t_ocx[1].MaskChar = 0;
	text_t_ocx[1].GetFocus = T_GetFocus;
	text_t_ocx[1].LossFocus = T_LossFocus;
	text_t_ocx[1].KeyProcess = T_KeyProcess;
	text_t_ocx[1].qbch = T_qbch;
	text_t_ocx[1].SetVal = usr_SetVal_T;
	text_t_ocx[1].GetVal = usr_GetVal_T;				
	
	text_t_ocx[1].ReadOnly &= 0x0f;	

	//日--------------------------------
	text_t_ocx[2].Type = K_TEXT;
	text_t_ocx[2].Focus = 2;
	//
	text_t_ocx[2].Caption1 = "-";
	text_t_ocx[2].Caption2 = "";	//此处为单位	
	//
	text_t_ocx[2].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[2].p_t_data = &clock_day;
	//s	
	text_t_ocx[2].Width = 2;
	//
	text_t_ocx[2].x = 13;
	text_t_ocx[2].y = 2;
	//
	text_t_ocx[2].Px = 0;
	text_t_ocx[2].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[2],&text_t_ocx[2].Value);		
				
	text_t_ocx[2].Cursor = 0;
	text_t_ocx[2].location = 14;
	
	text_t_ocx[2].dot = 0;
	text_t_ocx[2].Max = 31;
	text_t_ocx[2].Min = 1;
	text_t_ocx[2].MaskChar = 0;
	text_t_ocx[2].GetFocus = T_GetFocus;
	text_t_ocx[2].LossFocus = T_LossFocus;
	text_t_ocx[2].KeyProcess = T_KeyProcess;
	text_t_ocx[2].qbch = T_qbch;
	text_t_ocx[2].SetVal = usr_SetVal_T;
	text_t_ocx[2].GetVal = usr_GetVal_T;				
	
	text_t_ocx[2].ReadOnly &= 0x0f;	
	//时--------------------------------
	text_t_ocx[3].Type = K_TEXT;
	text_t_ocx[3].Focus = 3;
	//
	text_t_ocx[3].Caption1 = "时间:";
	text_t_ocx[3].Caption2 = "";	//此处为单位	
	//
	text_t_ocx[3].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[3].p_t_data = &clock_hour;
	//s	
	text_t_ocx[3].Width = 2;
	//
	text_t_ocx[3].x = 1;
	text_t_ocx[3].y = 5;
	//
	text_t_ocx[3].Px = 0;
	text_t_ocx[3].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[3],&text_t_ocx[3].Value);		
				
	text_t_ocx[3].Cursor = 0;
	text_t_ocx[3].location = 6;
	
	text_t_ocx[3].dot = 0;
	text_t_ocx[3].Max = 23;
	text_t_ocx[3].Min = 0;
	text_t_ocx[3].MaskChar = 0;
	text_t_ocx[3].GetFocus = T_GetFocus;
	text_t_ocx[3].LossFocus = T_LossFocus;
	text_t_ocx[3].KeyProcess = T_KeyProcess;
	text_t_ocx[3].qbch = T_qbch;
	text_t_ocx[3].SetVal = usr_SetVal_T;
	text_t_ocx[3].GetVal = usr_GetVal_T;				
	
	text_t_ocx[3].ReadOnly &= 0x0f;	
	//分--------------------------------
	text_t_ocx[4].Type = K_TEXT;
	text_t_ocx[4].Focus = 4;
	//
	text_t_ocx[4].Caption1 = ":";
	text_t_ocx[4].Caption2 = "";	//此处为单位	
	//
	text_t_ocx[4].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[4].p_t_data = &clock_minuter;
	//s	
	text_t_ocx[4].Width = 2;
	//
	text_t_ocx[4].x = 8;
	text_t_ocx[4].y = 5;
	//
	text_t_ocx[4].Px = 0;
	text_t_ocx[4].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[4],&text_t_ocx[4].Value);		
				
	text_t_ocx[4].Cursor = 0;
	text_t_ocx[4].location = 9;
	
	text_t_ocx[4].dot = 0;
	text_t_ocx[4].Max = 59;
	text_t_ocx[4].Min = 0;
	text_t_ocx[4].MaskChar = 0;
	text_t_ocx[4].GetFocus = T_GetFocus;
	text_t_ocx[4].LossFocus = T_LossFocus;
	text_t_ocx[4].KeyProcess = T_KeyProcess;
	text_t_ocx[4].qbch = T_qbch;
	text_t_ocx[4].SetVal = usr_SetVal_T;
	text_t_ocx[4].GetVal = usr_GetVal_T;				
	
	text_t_ocx[4].ReadOnly &= 0x0f;	
	//秒--------------------------------
	text_t_ocx[5].Type = K_TEXT;
	text_t_ocx[5].Focus = 5;
	//
	text_t_ocx[5].Caption1 = ":";
	text_t_ocx[5].Caption2 = "";	//此处为单位	
	//
	text_t_ocx[5].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[5].p_t_data = &clock_second;
	//s	
	text_t_ocx[5].Width = 2;
	//
	text_t_ocx[5].x = 11;
	text_t_ocx[5].y = 5;
	//
	text_t_ocx[5].Px = 0;
	text_t_ocx[5].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[5],&text_t_ocx[5].Value);		
				
	text_t_ocx[5].Cursor = 0;
	text_t_ocx[5].location = 12;
	
	text_t_ocx[5].dot = 0;
	text_t_ocx[5].Max = 59;
	text_t_ocx[5].Min = 0;
	text_t_ocx[5].MaskChar = 0;
	text_t_ocx[5].GetFocus = T_GetFocus;
	text_t_ocx[5].LossFocus = T_LossFocus;
	text_t_ocx[5].KeyProcess = T_KeyProcess;
	text_t_ocx[5].qbch = T_qbch;
	text_t_ocx[5].SetVal = usr_SetVal_T;
	text_t_ocx[5].GetVal = usr_GetVal_T;				
	
	text_t_ocx[5].ReadOnly &= 0x0f;	
	//
	share_plist[0] = &text_t_ocx[0];
	share_plist[1] = &text_t_ocx[1];
	share_plist[2] = &text_t_ocx[2];
	share_plist[3] = &text_t_ocx[3];
	share_plist[4] = &text_t_ocx[4];
	share_plist[5] = &text_t_ocx[5];

	//---------------------------------------------
	rq_first_rank.cont = 6;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 2;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 24;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	
	GUI_PutStringTier(0,0,(INT8*)"时钟设置",1);
	
	//调用
	ocx_key_type = 0;
	R_Execute(item);  
	//从执行函数退出后，判一下需要不需要保存操作
	if (ocx_key_type == KEY_SET)
	{
		CLR_SCREEN();
		//需要关中断


		SetProRunStatus(STA_SAVE_TIME_FLAG);
		ResetParaStatus(ENABLE_DATA_FLAG);

		if (!( ExeCallForm((UINT8 *)"正在固化中,请稍候",0)))
		{
			ResetProRunStatus(STA_SAVE_TIME_FLAG);
			item->ct = item->cancel;
		}
		else
		{
			ResetProRunStatus(STA_SAVE_TIME_FLAG);
		}
		
	}	
}
/*
显示设置段
两个控件：	1、背光延时		TEXT	为0时常亮
			2、测量显示模式	LIST
			
上传的数据帧格式：
两个字节，第一个是延时，第2个是显示模式			
*/
void R_Edit_Disp_Execute(struct _rq_t *item)
{	
	UINT16 disp_bkg_delay,disp_yc_disp_mode;
	disp_bkg_delay = bkg_delay;
	disp_yc_disp_mode = yc_disp_mode;
	//背光延时---------------------------
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "背光延时:";
	text_t_ocx[0].Caption2 = "秒";	//此处为单位	
	//
	text_t_ocx[0].ReadOnly = 1;		//数据源长度
	
	text_t_ocx[0].p_t_data = &bkg_delay;
	//s	
	text_t_ocx[0].Width = 3;
	//
	text_t_ocx[0].x = 0;
	text_t_ocx[0].y = 0;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//此值可能需要关联,此处不用初始化
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);		
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 13;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 255;
	text_t_ocx[0].Min = 0;				
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	//显示模式--------------------------------
	list_t_ocx[0].Type = K_LIST;
	list_t_ocx[0].Focus = 1;
	//
	list_t_ocx[0].Caption1 = "测量显示模式:";
	
	list_t_ocx[0].ReadOnly = 0;	//位定值所在位
	list_t_ocx[0].p_l_data = &yc_disp_mode; 
	usr_GetVal_L(&list_t_ocx[0],&list_t_ocx[0].Cursor); 
					
	
	list_t_ocx[0].Value = (INT8**)dlist_rcz_ycz;
	list_t_ocx[0].Width = 5;
			
	list_t_ocx[0].x = 0;
	list_t_ocx[0].y = 2;
	//
	list_t_ocx[0].Px = 0;
	list_t_ocx[0].Py = 0;
	//		
	list_t_ocx[0].location = 13;	
	//
	list_t_ocx[0].Max = 2;
	list_t_ocx[0].GetFocus = L_GetFocus;
	list_t_ocx[0].LossFocus = L_LossFocus;
	list_t_ocx[0].KeyProcess = L_KeyProcess;
	list_t_ocx[0].qbch = L_qbch;
	list_t_ocx[0].SetVal = usr_SetVal_L;
	list_t_ocx[0].GetVal = usr_GetVal_L;				
	
	list_t_ocx[0].ReadOnly &= 0x0f;			
						
	//
	share_plist_sub[1] = &list_t_ocx[0];

	//
	share_plist[0] = &text_t_ocx[0];
	share_plist[1] = &list_t_ocx[0];

	//---------------------------------------------
	rq_first_rank.cont = 2;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 3;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 24;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	
	GUI_PutStringTier(0,0,(INT8*)"显示设置",1);
	//调用
	ocx_key_type = 0;
	R_Execute(item);  
	//从执行函数退出后，判一下需要不需要保存操作
	if (ocx_key_type == KEY_SET)
	{
		CLR_SCREEN();
		SetProRunStatus(STA_SEQ_DISP_FLAG);
		ResetParaStatus(ENABLE_DATA_FLAG);

		if (!( ExeCallForm((UINT8 *)"正在固化中,请稍候",0)))
		{
			//如果固化不成功，恢复以前参数
			bkg_delay = disp_bkg_delay;
			yc_disp_mode = disp_yc_disp_mode;

			ResetProRunStatus(STA_SEQ_DISP_FLAG);
			item->ct = item->cancel;

		}
		else
		{
			ResetProRunStatus(STA_SEQ_DISP_FLAG);
		}		
	}	
}

//********************************************************************
