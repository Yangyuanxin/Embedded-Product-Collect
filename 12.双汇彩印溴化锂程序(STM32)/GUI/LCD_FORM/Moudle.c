#define _LCD_MOUDLE_C

#include "head.h"

//此程序文件实现三个LCD提示框函数功能

//
UINT8 ExeSaveForm(UINT8 *str, UINT8 mode)
{
	return 0;
}

UINT8 ExeCallForm(UINT8 *str, UINT8 mode)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	UINT8 i;
	UINT8 temp;
	UINT16 bak_key_value;

	CurHideCur();
	GUI_RectangleFill(0, 0, 24*8, 8*8,back_color);
	//进行居中显示，超过1行，则分行显示
	i = strlen((INT8*)str);
	if (i>20)
	{
		i = 20;
	}
	GUI_PutString16_16((24-i)/2*8,3*8,(INT8*)str,0);
	REFURBISH_LCD();				//刷新显示屏

	temp = 200;
	//
	while (!(GetParaStatus() & ENABLE_DATA_FLAG))
	{
		if ((--temp) == 0)
		{
			//数据没有召唤成功，提示一下然后返回
			GUI_RectangleFill(0, 0, 24*8, 8*8,back_color);
			GUI_PutString16_16(64,24,(INT8*)"超时失败",0);
			REFURBISH_LCD();				//刷新显示屏
			OSTimeDly(OS_TICKS_PER_SEC);
			OSTimeDly(OS_TICKS_PER_SEC);
			//						
			return 0;	
		}
		//
		OSTimeDly(OS_TICKS_PER_SEC/20);
		//
		//允许用户按取消键返回
		if (0 == cur_key_value)	continue;
		//
	    OS_ENTER_CRITICAL();
	    
	    bak_key_value = cur_key_value;		
		cur_key_value = 0;	
		
		OS_EXIT_CRITICAL();
		//			
		if (bak_key_value == KEY_ESC)
		{			
			return 0;
		}
	}
	
	return 1;
}

/*
密码认证对话界面
应该允许用户3次输入密码，但为了安全起见，只允许输入一次。
返回值：
		0:错误
		1:正确
		2:取消		
*/
UINT8 ExePasswordComp(UINT8 *str, UINT8 mode)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	
	k_text_t temp_text_ocx;
	UINT16 temp_password;
	UINT16 bak_key_value; 
	UINT16 flag;
	
	str = str;
	temp_password = 0;
	flag = 0;

	//
	//临时生成一个文本控件
	temp_text_ocx.Type = K_TEXT;
	temp_text_ocx.Focus = 0;
	//
	temp_text_ocx.Caption1 = "输入密码:";
	temp_text_ocx.Caption2 = " ";
	//
	temp_text_ocx.ReadOnly = 1;

	temp_text_ocx.p_t_data = &temp_password;
	temp_text_ocx.Width = 4;
	//
	temp_text_ocx.x = 0;
	temp_text_ocx.y = 3;
	temp_text_ocx.Px = 0;
	temp_text_ocx.Py = 0;
	//
	temp_text_ocx.Cursor = 0;
	temp_text_ocx.location = 9;

	temp_text_ocx.dot = 0;
	temp_text_ocx.Max = 60000;
	temp_text_ocx.Min = 0;

	temp_text_ocx.MaskChar = 0;
	temp_text_ocx.GetFocus = T_GetFocus;
	temp_text_ocx.LossFocus = T_LossFocus;
	temp_text_ocx.KeyProcess = T_KeyProcess;
	temp_text_ocx.qbch = T_qbch;
	temp_text_ocx.SetVal = usr_SetVal_T;
	temp_text_ocx.GetVal = usr_GetVal_T;
	
	temp_text_ocx.ReadOnly &= 0x0F;
	//初始化控件
	usr_GetVal_T(&temp_text_ocx,&temp_text_ocx.Value);
	CurHideCur();
	GUI_RectangleFill(0, 0, 24*8, 8*8,back_color);  
	SetItemPx(&temp_text_ocx,0);
	SetItemPy(&temp_text_ocx,0);
	qbchItem(&temp_text_ocx);
	T_GetFocus(&temp_text_ocx);
	//
	REFURBISH_LCD();	
	//处理文本控件
	while (1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/20);
	
		if (0 == cur_key_value) continue;
		//
		OS_ENTER_CRITICAL();

		bak_key_value = cur_key_value;
		cur_key_value = 0;

		OS_EXIT_CRITICAL();
		//
		switch(bak_key_value)
		{
			case KEY_ESC:
				return 2;
//				break;
			case KEY_SET:
				flag = 1;
				break;				
			default:

				T_KeyProcess(&temp_text_ocx,bak_key_value);
				break;
		}
		//
		if (flag)
		{
			usr_SetVal_T(&temp_text_ocx,temp_password);
			break;
		}
	}
	//比较输入密码与系统密码是否一致
	if (para_temp.value.password != temp_password)
	{
		return 0;
	}
	//
	return 1;
}