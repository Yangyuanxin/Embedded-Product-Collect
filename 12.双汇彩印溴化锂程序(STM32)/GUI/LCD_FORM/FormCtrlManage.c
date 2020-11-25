#define _FORM_CTRL_MANAGE_C
#include "head.h"
/*
模块：<控制管理>


显示框架如下：
		------------------------------------------
		目标:[本体断路器]
		状态:[合位]			操作:[分]
		------------------------------------------
				<确认键>进行执行操作
		------------------------------------------

传动操作	|本体断路器
			|
			|备用开出1
			|
			|备用开出2
			|
			|备用开出3
			

*/

#define OPEN_SAFE_CHECK

const INT8 *ctrl_object[];
const INT8 *ctrl_type[];
const INT8 *opp_ctrl_type[];
void R_CtrlManageExecuteNew(struct _rq_t *item);
void R_Ctrl_Opt_Execute(struct _rq_t *item);
void R_Ctrl_Debug_Execute(struct _rq_t *item);

void R_SecondCtrlExecute(struct _rq_t *item);

void R_Ctrl_Manage_Execute(struct _rq_t *item)
{
	CLR_SCREEN();
	rq_first_rank.Execute = R_CtrlManageExecuteNew;	
	
	rq_first_rank_enter_flag = 0;

	R_Execute(item);
}

void R_CtrlManageExecuteNew(struct _rq_t *item)
{

	//可以根据控件索引设定初始化工作
	if (rq_ctrl_manage_menu.cur_ocx == &ctrl_manage_opt)
	{
		R_Ctrl_Opt_Execute(item);
	}
	else if (rq_ctrl_manage_menu.cur_ocx == &ctrl_manage_debug)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Ctrl_Debug_Execute(item);
	}
}

/*
进行传动一级菜单动态生成即容器运行所需要的控件初始化

遥控参数在缓冲区的存放顺序是：
//每组6个字节
0	遥控号
1	对应量逻辑类型(b7) 对应量类型(b6-b4) 遥控类型(b3-b0)
2	备用
3	遥控对象名称
4	控制方式 控制方式所在位
5	备用

6
.


*/
void R_Ctrl_Opt_Execute(struct _rq_t *item)
{
//#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
//    OS_CPU_SR     cpu_sr = 0;
//#endif
	UINT8 j;

	rq_first_rank.cancel = &rq_ctrl_manage_menu;
	
	if (rq_first_rank_enter_flag) goto ctrl_abcd;	
	//查询遥控参数
	
	j = 0;
	while (j<(CTRL_PARA_LENGTH/6))
	{
		//填充一级菜单
		menu_t_ocx[j].Type = K_MENU;
		menu_t_ocx[j].Focus = j;	
		//
		menu_t_ocx[j].Text = (INT8*)ctrl_object[ctrl_para_xl_a[(j*6) + 3]];
		//
		menu_t_ocx[j].x = 0;
		menu_t_ocx[j].y = j*2;			
		//
		menu_t_ocx[j].GetFocus = M_GetFocus;
		menu_t_ocx[j].LossFocus = M_LossFocus;
		menu_t_ocx[j].qbch = M_qbch;
	
		menu_t_ocx[j].Token = &rq_second_rank;
	
		share_plist[j] = &menu_t_ocx[j];
		
		j++;
	}
	//
	rq_first_rank.cont = j;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 3;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 24;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_ctrl_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;		
	
	rq_first_rank.kj = &share_plist[0];
	
	rq_first_rank_enter_flag = 1;
    
    cur_s = 0;
ctrl_abcd:	CLR_SCREEN();

	GUI_PutStringTier(0,0,(INT8*)"传动操作",1);
	//指定子容器的执行函数
	rq_second_rank.Execute = R_SecondCtrlExecute;
	//
	R_Execute(item);	
}

void R_Ctrl_Debug_Execute(struct _rq_t *item)
{
	/********
	CLR_SCREEN();
	rq_first_rank.cancel = &rq_ctrl_manage_menu;	
	GUI_PutString16_16(4*8,24,(INT8*)"权限不够,请返回",0);	
	REFURBISH_LCD();
	//此处开始等待用户按键返回
	WaitHitKeyReturn(item,0);	
	item->ct = item->cancel;
	return;
	***************/
}

//----------------------------------------------------------------------
void R_SecondCtrlExecute(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr = 0;
#endif
    
	UINT8 i,index;
	//printf("test\r\n");
	//找到当前菜单号，根据菜单号去参数区取参数，然后形成控件组。
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			index = i;
		//	printf("index=%d",index);
			break;
		}
	}	
	//
	//填充控件
	/*	
		目标:[本体断路器]
		状态:[合位]			操作:[分]
		------------------------------------------
				<确认键>进行执行操作
		------------------------------------------
	*/
	//目标:[本体断路器]	(只读)

//	ctrl_code = share_buf[index*6];
	
	list_t_ocx[0].Type = K_LIST;
	list_t_ocx[0].Focus = 0;
	//
	list_t_ocx[0].Caption1 = (INT8*)" ";
	list_t_ocx[0].Caption2 = (INT8*)" ";
	list_t_ocx[0].ReadOnly = 0xf0;
    
	//list_t_ocx[0].p_l_data = &share_buf[(index*6)+3];//光标位置
	//usr_GetVal_L_Gern(&list_t_ocx[0],&list_t_ocx[0].Cursor);
    list_t_ocx[0].Cursor = ctrl_para_xl_a[(index*6)+3];
    
	list_t_ocx[0].Value = (INT8**)ctrl_object;
    
	list_t_ocx[0].Width = 20;
	//
	list_t_ocx[0].x = 0;
	list_t_ocx[0].y = 0;
	list_t_ocx[0].Px = 0;
	list_t_ocx[0].Py = 0;
	//
	list_t_ocx[0].location = 0;	
//	list_t_ocx[0].Max = GetListCount((INT8**)ctrl_object);
	list_t_ocx[0].GetFocus = L_GetFocus;
	list_t_ocx[0].LossFocus = L_LossFocus;
	list_t_ocx[0].KeyProcess = L_KeyProcessGern;
	list_t_ocx[0].qbch = L_qbch;
	list_t_ocx[0].SetVal = usr_SetVal_L_Gern;
	list_t_ocx[0].GetVal = usr_GetVal_L_Gern;
	share_plist_sub[0] = &list_t_ocx[0];
    
	//状态:[合位]
	list_t_ocx[1].Type = K_LIST;
	list_t_ocx[1].Focus = 1;
	//
	list_t_ocx[1].ReadOnly = 0x01;
	
	list_t_ocx[1].Caption1 = (INT8*)"状态:";
	list_t_ocx[1].Caption2 = (INT8*)" ";
    
    list_t_ocx[1].Cursor = control_object_state[list_t_ocx[0].Cursor];
	//
//	if (0x00 == ctrl_para_xl_a[(i*6)+1] & 0x7f)			//YX
//	{
	//	list_t_ocx[1].ReadOnly |= lcd_ctrl_flag[list_t_ocx[0].Cursor];			//取断路器位置或其它节点位置，是由灯显数据得到
	//list_t_ocx[1].p_l_data = (u16*)&control_object_state[list_t_ocx[0].Cursor];

    //如果操作的是排污
   // if((list_t_ocx[0].Cursor == LCD_OPEN_PAIWU)||(list_t_ocx[0].Cursor == LCD_CLOSE_PAIWU))
    if(list_t_ocx[0].Cursor == LCD_OPEN_PAIWU)
    {
        //如果此时排污阀正在动作
        if(control_object_state[list_t_ocx[0].Cursor] ==OBJECT_RUNNING)
        {
            CLR_SCREEN();
            while(1)
            {
                GUI_PutString16_16(8,8,(INT8*)"排污阀正在动作",0);
                GUI_PutString16_16(32,24,(INT8*)"禁止操作",0);
                GUI_PutString16_16(16,40,(INT8*)"按任意键返回",0);
                if (cur_key_value)                            
                {
                    OS_ENTER_CRITICAL();    	
                    cur_key_value = 0;
                    OS_EXIT_CRITICAL();
                    item->ct = (void*)&rq_first_rank;
                    return;
                }
                OSTimeDly(OS_TICKS_PER_SEC/50 );
            }            
        }        
    }	
	//usr_GetVal_L(&list_t_ocx[1],&list_t_ocx[1].Cursor);
	list_t_ocx[1].Value = (INT8**)&ctrl_type[(ctrl_para_xl_a[(index*6)+1])*2];
	list_t_ocx[1].Width = 20;
	//
	list_t_ocx[1].x = 0;
	list_t_ocx[1].y = 3;
	list_t_ocx[1].Px = 0;
	list_t_ocx[1].Py = 0;
	//
	list_t_ocx[1].location = 6;	
	list_t_ocx[1].Max = 2;
	list_t_ocx[1].GetFocus = L_GetFocus;
	list_t_ocx[1].LossFocus = L_LossFocus;
	list_t_ocx[1].KeyProcess = L_KeyProcessGern;
	list_t_ocx[1].qbch = L_qbch;
	list_t_ocx[1].SetVal = usr_SetVal_L_Gern;
	list_t_ocx[1].GetVal = usr_GetVal_L;
	share_plist_sub[1] = &list_t_ocx[1];
	//操作:[分]
	list_t_ocx[2].Type = K_LIST;
	list_t_ocx[2].Focus = 2;
	//
	list_t_ocx[2].ReadOnly = 0x01;
	list_t_ocx[2].Caption1 = (INT8*)"操作:";	//	
	list_t_ocx[2].Caption2 = (INT8*)" ";
	
    //在按键操作中，操作的是此值

	list_t_ocx[2].p_l_data =(u16*)&lcd_ctrl_flag[index]; 

    
    //仅用于状态显示
    if(control_object_state[index]==OBJECT_OPEN)
    {
        list_t_ocx[2].Cursor =1;
    }
    else if(control_object_state[index]==OBJECT_CLOSE)
    {
        list_t_ocx[2].Cursor =0;
    }
	//usr_GetVal_L(&list_t_ocx[2],&list_t_ocx[2].Cursor);
    
	list_t_ocx[2].Value = (INT8**)&ctrl_type[(ctrl_para_xl_a[(index*6)+1])*2];
	list_t_ocx[2].Width = 20;
	//
	list_t_ocx[2].x = 0;
	list_t_ocx[2].y = 5;
	list_t_ocx[2].Px = 0;
	list_t_ocx[2].Py = 0;
	//
	list_t_ocx[2].location = 6;	
	list_t_ocx[2].Max = 2;
	list_t_ocx[2].GetFocus = L_GetFocus;
	list_t_ocx[2].LossFocus = L_LossFocus;
	list_t_ocx[2].KeyProcess = L_KeyProcessControl;
	list_t_ocx[2].qbch = L_qbch;
	list_t_ocx[2].SetVal = usr_SetVal_L_Gern;
	list_t_ocx[2].GetVal = usr_GetVal_L;
	share_plist_sub[2] = &list_t_ocx[2];
	//---------
	rq_second_rank.cont = 3;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 2;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];
	//------------------------------------------
	CLR_SCREEN();

    
	R_Execute(item);
   
	//
}

const INT8 *ctrl_object[] = 
{
    "[1号加药]",
	"[2号加药]",
	"[3号加药]",
	"[4号加药]",
	"[排污阀]",
	"[本体下刀]",
	"[本体地刀]",
	"[母线侧刀]",
	"[中性点刀]",
	"[线路侧地刀]",
	"[档位升降]",
	"[风机]",
	"[过热闭锁合闸解除]",
	"[调压闭锁]",
	"[档位紧急停]",
	"[保留]",
	//10H
	"[备用开出1]",
	"[备用开出2]",
	"[备用开出3]",
	"[备用开出4]",
	"[备用开出5]",
	"[备用开出6]",
	"[备用开出7]",
	"[备用开出8]",
	"[备用开出9]",
	"[备用开出10]",
	"[备用开出11]",
	"[备用开出12]",
	"[备用开出13]",
	"[备用开出14]",
	"[备用开出15]",
	"[备用开出16]",
	//20h
	"[进线Ⅰ断路器]",
	"[进线Ⅱ断路器]",
	"[分段开关]",
	"[PT切换]",
};


const INT8 *ctrl_type[] = 
{
	"分","合",
	"降","升",
	"风停","风启",
	"复原","切换",
	"解锁","闭锁",
	"紧急停解除","紧急停",
};


