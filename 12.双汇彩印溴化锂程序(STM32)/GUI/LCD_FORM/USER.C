#define _GUI_USER_C

#include "head.h"


//定义参数修正类型
const INT8 *adj_group_type[] =
{
    "ORP系数 ",
	"流量",
	"备用",
	"备用",
	"PH温度",
	"PH值",
	"电导值",
	"电导温度"
};
const INT8 *jiankong_group_type[] =
{
    "端口号 ",
	"波特率",
	"停止位",
	"校验方式",
};
const INT8 *gprs_group_type[] =
{
    "端口号 ",
	"波特率",
	"停止位",
	"校验方式",
};
const INT8 *sensor_group_type[] =
{
    "传感器数量 ",
	"端口号",
	"波特率",
	"停止位",
	"校验方式",
};


const INT8 *yc_unit[] = 
{
	" ","S",
	"V","KV",
	"欧","欧",		//以后改为单位
	"S","S",
	"Hz","Hz",
	"Hz/S","Hz/S",
	"相","相",
	"℃","℃",
	"千欧","千欧",
	"%","%",
	"度","度",
	"W","MW",
	"Var","MVar",
	"","",
	"","",
	"","",
};

const INT8 *yx_meaning[] = 
{
	"冷却塔水位低",//第I2路
	"1药罐液位低",//第I5路
	"2药罐液位低",//第I6路
	"3药罐液位低",//第I7路
	"4药罐液位低",//第I8路
	"排污阀全关",//第I9路
	"排污阀全开",//第I10路
	"线路侧地刀",
	"重瓦斯",
	"轻瓦斯",
	"调压重瓦斯",
	"调压轻瓦斯",
	"压力释放",
	"风机启动",
	"变压器温度高",
	"油温高",
	//10H
	"绕组过温",
	"冷却器故障",
	"油位异常",
	"风冷故障",
	"SF6气压低闭锁",
	"SF6气压低报警",
	"交流电源消失",
	"直流电源消失",
	"远方/就地",
	"弹簧已储能",
	"弹簧未储能",
	"弹簧储能",
	"外变跳本变",
	"高压侧复合电压",
	"中压侧复合电压",
	"低压侧复合电压",
	//20H
	"开关位置",
	"AB相过流",
	"BC相过流",
	"ABC相过流",
	"A相接地",
	"B相接地",
	"C相接地",
	"PT1带电状态",
	"PT2带电状态",
	"装置交流电源",
	"电池低电压",
	"电池活化",
	"控制方式",
	"重合闸闭锁",
	"重合闸投入",
	"就地/远方",
	//30H
	"速断动作",
	"过流Ⅰ段动作",
	"过流Ⅱ段动作",
	"一次重合闸动作",
	"二次重合闸动作",
	"三次重合闸动作",
	"反时限Ⅰ动作",
	"反时限Ⅱ动作",
	"反时限Ⅲ动作",
	"PT断线告警",
	"低周减载",
	"交流失电",
	"电池故障",
	"差动投跳",
	"接地刀闸",
	"温度报警",
	//40H	
	"自启动投入",
	"试验位置",
	"进线Ⅰ位置",
	"进线Ⅱ位置",
	"分段开关位置",
	"闭锁备自投",
	"水工故障",
	"失磁信号",
	"并列/解列",
	"  ",
	"控制/节点",
	"PT1刀闸",
	"PT2刀闸",
	"小车位置",
	"工艺联跳1",
	"工艺联跳2",
	//50H
	"信号复归",
	"超温信号",
	"本体跳闸",
	"本体告警",	
	"开入1",
	"开入2",
	"开入3",
	"开入4",
	"开入5",
	"开入6",
	"开入7",
	"开入8",
	"开入9",
	"开入10",
	"开入11",
	"开入12",
	//60HS		
	"开入13",
	"开入14",
	"开入15",	
	"开入16",
	"无效信号量",
	"闭锁信号",
	"低压侧断路器合位",
	"",	
	//如果是7F，则显示“遥信N”。
};

/*
定值组类型
*/
const INT8 *fix_group_type[] = 
{
	"PH高限  ",
	"PH低限  ",
	"ORP高限 ",
	"ORP低限 ",
	"温度高限",
	"温度低限",
	"电导高限",
	"电导低限",
	"反时限过流Ⅰ段",
	"反时限过流Ⅱ段",
	"反时限过流Ⅲ段",
	"相间距离Ⅰ段",
	"相间距离Ⅱ段",
	"相间距离Ⅲ段",
	"接地距离Ⅰ段",
	"接地距离Ⅱ段",
	//10H
	"接地距离Ⅲ段",
	"负序过流Ⅰ段",
	"负序过流Ⅱ段",
	"负序过流Ⅲ段",
	"负序反时限过流Ⅰ段",
	"负序反时限过流Ⅱ段",
	"负序反时限过流Ⅲ段",
	"过压保护",
	"失压保护",
	"不平衡零序过流",
	"零序过压",
	"间隙过流",
	"过负荷",
	"低周",
	"短路保护",	
	"启动中堵转",
	//20H
	"运行中堵转",
	"返回系数",
	"运行参数",
	"停转散热时间常数",
	"重合闸",
	"低电压保护Ⅰ段",
	"低电压保护Ⅱ段",
	"同期合闸",
	"风机启动",
	"调压闭锁",
	"备自投",
	"",
	"",
	"电压元件",
	"电流元件",
	"有压合闸",
	//30H
	"失压跳闸",
	"启动元件",
	"功率方向",
	"抽取电压",
	"录波",
	"母联保护",
	"复合电压",
	"外变跳本变",
	"本变跳外变",
	"负序过流",
	"间隙过压",
	"过热保护",
	"启动时间过长保护",
	"零序过流",
	"横差保护",
	"其它"
	//40H
	"反时限负序过流",
	"定时限负序过流",
	"定子单相接地",
	"转子一点接地",
	"转子两点接地",
	"失磁保护",
	"调整率",
	"反时限过负荷",
	"熔丝缺相保护",
	"遥控1",
	"遥控2",
	"遥控3",
	"遥控4",
	"遥控5",
	"遥控6",
	"遥控7",
	//50H
	"遥控8",
	"遥控9",
	"遥控10",
	"遥控11",
	"遥控12",
	"遥控13",
	"遥控14",
	"遥控15",
	"遥控16",	
	"记忆过流保护",
	"低压闭锁",
	"过流公用组",
	"零序过流公用组",
	"高压侧零序过流"
	"低压侧反时限零序",
	"I段母线参数",
	//60H
	"II段母线参数",
	"分段参数",
	"进线Ⅰ参数",
	"进线Ⅱ参数",
	"母线参数",
	"备自投正向动作",
	"备自投逆向动作",
	"备自投运行方式",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

/*
定值名称列表
*/
const INT8 *fix_name_list[] = 
{
	"定值",
	"延时",
	"I时限T1",
	"II时限T2",
	"返回系数",
	"一次重合闸延时",
	"二次重合闸延时",
	"三次重合闸延时",
	"同期电压差",
	"无压元件",
	"无流元件",
	"闭锁电压",
	"闭锁电流",
	"滑差闭锁值",
	"零序闭锁电流",
	"T1跳高分段",
	//10h
	"T1跳中分段",
	"T1跳低分段",
	"T1跳本侧",
	"T1跳高压侧",
	"T1跳中压侧",
	"T1跳低压侧",
	"T2跳高分段",
	"T2跳中分段",
	"T2跳低分段",
	"T2跳本侧",
	"T2跳高压侧",
	"T2跳中压侧",
	"T2跳低压侧",
	"投入",
	"带方向",
	"带小延时",
	//20h
	"经电压闭锁",
	"最小时间",
	"热容常数",
	"突变量元件",
	"经零序电流启动",
	"经电流闭锁",
	"经低电压闭锁",
	"经滑差闭锁",
	"保护启动录波",
	"跳闸启动录波",
	"合闸启动录波",
	"前加速投入",
	"斜率I",
	"斜率Ⅱ",
	"拐点I",
	"拐点Ⅱ",
	//30h
	"中性点调整率",
	"导前时间",
	"启动中正序电流分量",
	"运行中正序电流分量",
	"负序电流分量",
	"PT断线闭锁低压",
	"闭锁重合闸",
	"经本侧复合电压闭锁",
	"经高压侧复合电压闭锁",
	"经中压侧复合电压闭锁",
	"经低压侧复合电压闭锁",
	"后加速投入",
	"过负荷启动(定值)",
	"投入T1",
	"投入T2",
	"大接地电流系统",
	//40
	"小延时周波数",
	"一次重合闸投入",
	"二次重合闸投入",
	"三次重合闸投入",
	"检同期",
	"检无压",
	"经负序电压闭锁",
	"经CT断线闭锁",
	"零序算法",							//(基波/5次谐波)
	"保护方式",							//(投跳闸/投告警)
	"动作方式",							//(投跳闸/投信号)
	"95%保护方式",						//(投跳闸/投告警)
	"100%保护方式",						//(投跳闸/投告警)
	"备自投运行方式",					//(主备/互备)
	"主电源",							//进线I/进线II
	"",
	//50H
	"过流启动值",
	"电压上限",
	"电压下限",
	"残压上限",
	"单侧有压合闸延时",
	"失压分闸延时",
	"联络开关单侧失压合闸延时",
	"残压检测延时",
	"残压闭锁延时",
	"继电器返回时间",
	"合闸全压判定",
	"开关描述",
	"装置投退",
	"过流检测延时",
	"涌流抑制投入",
	"额定电流",
	//60h
	"功率方向角为30度",
	"功率方向角为45度",
	"功率方向角为60度",
	"抽取电压为Ua",
	"抽取电压为Ub",
	"抽取电压为Uc",
	"抽取电压为Uab",
	"抽取电压为Ubc",
	"抽取电压为Uca",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
/*
系数名称列表
*/
const INT8 *Adj_name_list[] = 
{
    "系数",
};
/*
工作参数名称列表
*/
const INT8 *jiayao_name_list[] = 
{
    "脉冲时长",
    0,
};

const INT8 *Work_Para_Group_Type[]=
{
    "1号加药脉冲",
    "2号加药脉冲",
    "3号加药脉冲",
    "4号加药脉冲",
    "最大流量加药脉",
    "最大ph加药脉冲",
    "最大orp加药脉冲",
    
};
/*
实时显示YC数据,并且响应3个键:上,下,取消
	
首先根据YC参数中得到的遥测个数，算出遥测需要显示的行数
//内容排列：YC类型、YC值、单位
*/
void DispRealTimeYc(struct _rq_t *item)
{	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	UINT8 i;
	UINT8 to;							//遥测刷新间隔时间
	UINT16 max_line=0;					//所需显示的最大行
	UINT16 bak_key_value;	
	
	PARA_YC_STRU *p_yc_stru;	

	INT8 str[16];
		
	//计算所需显示的行数	
	//
	//max_line = (*(p_yc_para+2) + ((UINT16)*(p_yc_para+3))<<8)/4;
// 	max_line = *(p_yc_para+3);
// 	max_line <<= 8;
// 	max_line += *(p_yc_para+2);
// 	
// 	max_line = (max_line-4)>>2;		//除以4	
	max_line = NUM_YC;
	
	item->df_line = 0;
	item->cur_line = 0;
    

    //
    CurHideCur();

    CLR_SCREEN();	
    GUI_PutStringTier(0,0,(INT8*)"实时遥测",1);
    //
    while (1)
    {
        OSTimeDly(OS_TICKS_PER_SEC/20);			
        //
        if (0 == to)		//刷新显示的标志
        {
            UINT16 temp_value;
            //clr screen
            GUI_RectangleFill(16,0,127,63,0);
            
            for (i=item->df_line; (i<item->df_line+4)&&(i<max_line); i++)
            {
                //此处显示YC类型\数值\单位
                //显示单位的时候要判断是显示一次值或是二次值
                //其实最好全用成一次值
                //sprintf(str,"测量值 %02d",i);
                //GUI_PutString16_16(32,(i-item->df_line)*2*8,str,0);
                //-------------------------------------------------------------------------------------					

                p_yc_stru = (PARA_YC_STRU*)(p_yc_para + 4);
                p_yc_stru += i;

        //        GUI_PutString16_16(16,(i-item->df_line)*2*8,(INT8*)yc_type[p_yc_stru->type],0);

				temp_value = lcd_disp_measure[i];

                PrintfFormat(str,(INT32)temp_value,5);
                //小数点调整
                //进行小数点填入
                if (p_yc_stru->format)
                {
                    UINT8 temp1,temp2;
                    //
                    temp1 = strlen(str);
                    
                    for (temp2=0; temp2<p_yc_stru->format; temp2++)
                    {
                        str[temp1] = str[temp1-1];
                        temp1--;
                    }  		
                    str[temp1] = '.';
                }
                //
                GUI_PutString16_16(64,(i-item->df_line)*2*8,str,0);
                //
                
                //显示单位
                //GUI_PutString16_16(80+6*8,(i-item->df_line)*2*8,(INT8*)yc_unit[((p_yc_stru->unit)*2)+yc_disp_mode],0);
            
            }				
            //-------------------------------------------------------------
            //竖滚动条
            if (item->df_line != 0)
            {
                GUI_PutString16_16(GUI_LCM_XMAX-16,0,"∧",1);
            }
            else
            {
                GUI_PutString16_16(GUI_LCM_XMAX-16,0,"  ",0);
            }
            if ((item->df_line+4) < max_line)
            {
                GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"∨",1);
            }
            else
            {
                GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"  ",0);
            }
            //-------------------------------------------------------------
            
            REFURBISH_LCD();				//刷新显示屏
        }
        //刷新YC值显示时间计数			
        if (++to>10)
        {
            to = 0;	
		
        }
        //进行按键处理
        if (0 == cur_key_value)	continue;
        //
        OS_ENTER_CRITICAL();
        
        bak_key_value = cur_key_value;		
        cur_key_value = 0;	
        
        OS_EXIT_CRITICAL();
        //
        switch (bak_key_value)
        {
            case KEY_ESC:

                ResetProRunStatus(STA_SEQ_REAL_DATA_FLAG);

                item->ct = item->cancel;
                return;
            case KEY_ADD:					
                if ((item->df_line+4) < max_line)
                {
                    to = 0;
                    //item->df_line++;
                    item->df_line+=4;
                    
                    //GUI_RectangleFill(32,0,192,64,0);
                }
                else
                {
                    item->df_line=0;
                    
                }
                break;

        }
    }	

}


/*
实时缩略显示YX数据,并且响应3个键:上,下,取消
*/
void DispRealTimeYxEasy(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	UINT16 bak_key_value;

	UINT8 max_line;
	UINT8 to;
	UINT8 i;	
    u8 temp_yx_value[32];//与遥信个数保持一致
    u8 temp_yx_num;
    temp_yx_num =32;//遥信个数
	INT8 str[32];
	
	//
	CurHideCur();
	
	CLR_SCREEN();	
	GUI_PutStringTier(0,0,(INT8*)"实时遥信",1);	
	//
	
    if(temp_yx_num%5)
    {
        max_line = temp_yx_num/5+1;		//每行只能显示5个位置
        
    }
    else
    {  
        max_line = temp_yx_num/5;
    }
    
	
	item->df_line = 0;
	item->cur_line = 0;

		
    //
    CLR_SCREEN();	
    GUI_PutStringTier(0,0,(INT8*)"实时遥信",1);
    to = 0;
    //
    while (1)
    {		
        //
        if (0 == to)	
        {	
            UINT8 x,y;		
            
            x = 24;
            y = 0;
            //clr screen
            GUI_RectangleFill(16,0,127,63,0);
            //
            for (i=0; i<temp_yx_num; i++)
            {
                
               // sprintf(str,"%02d",i);
                //根据YX数据决定显示
                if (yx_buf[ (i/16) ] & (1<<(i%16)))	//
                {
                   // GUI_PutString16_16(x+((i%5)*20),y+(i/5)*16,str,1);
                    temp_yx_value[i] = 1;
                }
                else
                {
                    temp_yx_value[i] = 0;
                   // GUI_PutString16_16(x+((i%5)*20),y+(i/5)*16,str,0);
                }
            }
            for (i=((item->df_line)*5); (i<(item->df_line+4)*5)&&(i<32); i++)
            {
                sprintf(str,"%02d",i);
              //  printf("lint=%d\r\n",item->df_line);
                //-------------------------------------------------------------------------------------					
                if(temp_yx_value[i])
                {
                    GUI_PutString16_16(x+(((i-item->df_line*5)%5)*20),(y+((i-item->df_line*5)/5)*16),str,1);
                }
                else
                {
                    GUI_PutString16_16(x+(((i-item->df_line*5)%5)*20),(y+((i-item->df_line*5)/5)*16),str,0);
                }
       
            
            }	
            
            
            //
            REFURBISH_LCD();				//刷新显示屏
        }
        //刷新YX值显示时间计数			
        if (++to>5)
        {
            to = 0;				
        }
        
        OSTimeDly(OS_TICKS_PER_SEC/20);
        
        //进行按键处理
        if (0 == cur_key_value)	continue;
        //
        OS_ENTER_CRITICAL();
        
        bak_key_value = cur_key_value;		
        cur_key_value = 0;	
        
        OS_EXIT_CRITICAL();
        //
        switch (bak_key_value)
        {
            case KEY_ESC:

                item->ct = item->cancel;
                return;	
            case KEY_ADD:
                if ((item->df_line+4) < max_line)
                {
                    to = 0;
                    //item->df_line++;
                    item->df_line+=4;
                    
                    //GUI_RectangleFill(32,0,192,64,0);
                }
                else
                {
                    item->df_line=0;
                    
                }
                break;
        }	
        //		
    }

}

/*
实时详细显示YX数据,并且响应3个键:上,下,取消
*/
void DispRealTimeYxAll(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	UINT16 bak_key_value;

	UINT16 max_line;
	UINT8 to;
	UINT8 i;
	UINT8 temp;
	
	DISP_YX_PARA_STRU *p_yx_stru;	
	//
	//max_line = (*(p_yx_para+2) + ((UINT16)*(p_yx_para+3))<<8)/4;
	max_line = *(p_yx_para+3);
	max_line <<= 8;
	max_line += *(p_yx_para+2);
	
	max_line = (max_line-4)>>2;		//除以4	

	item->df_line = 0;
	item->cur_line = 0;

	//
	CurHideCur();
	
	CLR_SCREEN();	
	GUI_PutStringTier(0,0,(INT8*)"实时遥信",1);
	//
	while (1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/20);			
		//
		if (0 == to)		//刷新显示的标志
		{			
			//clr screen
			GUI_RectangleFill(24,0,191-16,63,0);
			
			for (i=item->df_line; (i<item->df_line+4)&&(i<max_line); i++)
			{
				//此处显示YX意义\状态												
				//-------------------------------------------------------------------------------------					

				p_yx_stru = (DISP_YX_PARA_STRU*)(p_yx_para + 4) + i;
				
				//GUI_PutString16_16(18,(i-item->df_line)*2*8,(INT8*)yc_type[yc_para[i].type],0);
				//显示遥信意义

                GUI_PutString16_16(	24,
                                    (i-item->df_line)*2*8,
                                    (INT8*)yx_meaning[p_yx_stru->yx_name],
                                    0);
				//根据正负逻辑显示当前YX状态
				temp = p_yx_stru->yx_num;
                
				if (yx_buf[ (temp/16) ] & (1<<(temp%16)))	//当前位为1
				{
               
                    
					if (p_yx_stru->yx_type & 0x80)			//负逻辑
					{
						switch (p_yx_stru->yx_type & 0x7)		
						{
							case 0:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"分",0);
								break;
							case 1:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"○",0);
								break;
							case 2:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"远方",0);
								break;
							default:
								break;
						}
					}
					else
					{
						switch (p_yx_stru->yx_type & 0x7)	//正逻辑	
						{
							case 0:
                                printf("temp=%d\r\n",temp);
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"合",0);
								break;
							case 1:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"●",0);
								break;
							case 2:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"就地",0);
								break;
							default:
								break;
						}
					}
				}
				else	//该位为0
				{
					if (p_yx_stru->yx_type & 0x80)			//负逻辑
					{
						switch (p_yx_stru->yx_type & 0x7)		
						{
							case 0:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"合",0);
								break;
							case 1:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"●",0);
								break;
							case 2:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"就地",0);
								break;
							default:
								break;
						}
					}
					else
					{
						switch (p_yx_stru->yx_type & 0x7)	//正逻辑	
						{
							case 0:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"分",0);
								break;
							case 1:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"○",0);
								break;
							case 2:
								GUI_PutString16_16(127-32,(i-item->df_line)*2*8,"远方",0);
								break;
							default:
								break;
						}
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
				GUI_PutString16_16(GUI_LCM_XMAX-16,0,"  ",0);
			}
			if ((item->df_line+4) < max_line)
			{
				GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"∨",1);
			}
			else
			{
				GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"  ",0);
			}
			//-------------------------------------------------------------
			
			REFURBISH_LCD();				//刷新显示屏
		}
		//刷新YC值显示时间计数			
		if (++to>5)
		{
			to = 0;				
		}
		//进行按键处理
		if (0 == cur_key_value)	continue;
		//
		OS_ENTER_CRITICAL();
		    
		bak_key_value = cur_key_value;		
		cur_key_value = 0;	
		
		OS_EXIT_CRITICAL();
		//
		switch (bak_key_value)
		{
			case KEY_ESC:
				//ResetProRunStatus(STA_SEQ_REAL_DATA_FLAG);
				item->ct = item->cancel;
				return;
			case KEY_ADD:					
				if ((item->df_line+4) < max_line)
				{
					to = 0;
					item->df_line+=4;
					
					//GUI_RectangleFill(32,0,192,64,0);
				}
                else
                {
                    to = 0;
					item->df_line=0;
                    
                }
				break;
			case KEY_DEC:
				if (4 <= item->df_line)
				{
					to = 0;
					item->df_line-=4;
				
					//GUI_RectangleFill(32,0,192,64,0);
				}
				break;
		}
	}
}

//----------------------------------------------------------------------
void usr_SetVal_T(struct _text_t *item, int32s vl)
{
  	//item->Value = vl;
  	//UINT8 i;
    u8 temp_save_index;
  	INT16 temp;
  	//i = rq_second_rank.Focus + rq_first_rank.Focus;
  	temp = item->Value;
	*item->p_t_data = temp;
 //   printf("temp=%d\r\n",temp);
	//printf("change_flag=%d\r\n",change_para_type_flag);
    temp_save_index = rq_second_rank.Focus + rq_first_rank.Focus;
    if(change_para_type_flag ==FIX_EDIT_FLAG)//操作的是定值
    {
    //    change_para_type_flag = 0;
        temp_fix_para_buf[temp_save_index] = temp;
	//	printf("temp_save_index=%d, temp_fix_para_buf[temp_save_index]=%d\r\n",temp_save_index, temp_fix_para_buf[temp_save_index]);
      //  printf("temp =%d\r\n",temp);
        
    }
    else if(change_para_type_flag == XISHU_EDIT_FLAG)
    {
     //   change_para_type_flag = 0;
        temp_xishu_para_buf[temp_save_index] = temp;
    }
    else if(change_para_type_flag == WORK_EDIT_FLAG)
    {
     //   change_para_type_flag = 0;
        temp_jiayao_para_buf[temp_save_index] = temp;
    }
	else if(change_para_type_flag == JIANKONG_EDIT_FLAG)
    {
     //   change_para_type_flag = 0;
        temp_jiankong_commpara_buf[temp_save_index] = temp;
    }
    else if(change_para_type_flag == GPRS_EDIT_FLAG)
    {
     //   change_para_type_flag = 0;
        temp_gprs_commpara_buf[temp_save_index] = temp;
    }
	else if(change_para_type_flag == SENSOR_EDIT_FLAG)
    {
     //   change_para_type_flag = 0;
        temp_sensor_commpara_buf[temp_save_index] = temp;
    }
}

void usr_GetVal_T(struct _text_t *item, int32s *vl)
{  
  	//*vl = item->Value;
//  	UINT8 i;
  	UINT32 temp=0;
  	
//  	for (i=0; i<(item->ReadOnly&0x0f); i++)
 // 	{
 // 		temp <<= 8;
 // 		temp += *(item->p_t_data+(item->ReadOnly&0x0f)-1-i);
  		
 // 	}
  	temp = *item->p_t_data;
  	*vl = temp;
  	
}

//列表框处理函数1
void usr_SetVal_L(struct _list_t *item, UINT8 vl)			//
{  	
//    u8 i;
	*(item->p_l_data) = vl;
    
//   	if (item->Cursor)
//   	{
//   		//*(item->p_l_data) |= (1<<(item->ReadOnly&0x0f));
//        // *(item->p_l_data) |= item->ReadOnly&0x0f;
//         *(item->p_l_data) = 1;
//         
//         
//   	}
//   	else
//   	{
//   		//*(item->p_l_data) &= (~(1<<(item->ReadOnly&0x0f)));
//         //*(item->p_l_data) &= (~(item->ReadOnly&0x0f));
//         *(item->p_l_data) = 0;
//   	}
}
void usr_GetVal_L(struct _list_t *item, UINT8 *vl)			//
{  
  	//if (*(item->p_l_data) & (1<<(item->ReadOnly&0x0f)))
    if (*(item->p_l_data) & (item->ReadOnly&0x0f))
  	{
  		*vl = 1;
  	}
  	else
  	{
  		*vl = 0;
  	}
}

//列表框处理函数2	通用处理函数
void usr_SetVal_L_Gern(struct _list_t *item, UINT8 vl)
{
	*item->p_l_data = item->Cursor;
}

void usr_GetVal_L_Gern(struct _list_t *item, UINT8 *vl)
{
	*vl = *item->p_l_data;
}
