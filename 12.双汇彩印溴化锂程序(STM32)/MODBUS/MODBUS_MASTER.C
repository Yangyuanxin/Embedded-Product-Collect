//MODBUS主站

//对应于抄收数字传感器的数据
//其运行受参数块控制，参数块在上电初始化时初始化。



/*

*/

#define _MODBUS_MASTER_C

#include "head.h"

typedef union f_value
{
	float abc;
	UINT8 gh[4];
	
}aaa;


//#define DBG_MB_MST		//调试MODBUS主站的开关

#ifdef DBG_MB_MST
INT8 dbg_mb_mst_buf[128];
#endif

UINT8 cur_index,cur_mega;	//轮循的当前终端号和当前段
UINT32 frame_delay;

//收发缓冲区
MODBUS_MST_RECE_BUF mb_mst_rece;
MODBUS_MST_SEND_BUF mb_mst_send;

#define MB_SEND_TIME   35	//发送延时25


static void MB_MST_SetupFrame(void );
static void MB_MST_SendFrame(u8 port);
static void MB_MST_ProcessData(u8 port);
static void MB_MST_ReceFrame(u8 port);
//


/*
主站MODBUS初始化函数
*/

void Init_Mb_Mst(void)
{  
	//此处开始从FLASH中搬运相应参数到主站终端参数结构体中，并且也要拷贝一份到MODBUS的保持寄存器区
	
	//如果FLASH是无效的，则装置默认参数。	
	mb_mst_terminal_para.num = 6;
	
	//第1个终端为1#主传感器，地址是1
    mb_mst_terminal_para.terminal[0].addr = 1;	//
	mb_mst_terminal_para.terminal[0].word_len = 10;
	//mb_mst_terminal_para.terminal[0].word_offset = 0x0829;
	mb_mst_terminal_para.terminal[0].available_num = 2;
	mb_mst_terminal_para.terminal[0].reg_offset[0] = 0x0829;
	mb_mst_terminal_para.terminal[0].save_place[0] = 4;
	mb_mst_terminal_para.terminal[0].reg_offset[1] = 0x0969;
	mb_mst_terminal_para.terminal[0].save_place[1] = 32;
	
	//第2个终端为1#从传感器，地址是3
    mb_mst_terminal_para.terminal[1].addr = 3;	//
	mb_mst_terminal_para.terminal[1].word_len = 10;
	//mb_mst_terminal_para.terminal[1].word_offset = 0x0829;
	mb_mst_terminal_para.terminal[1].available_num = 2;
	mb_mst_terminal_para.terminal[1].reg_offset[0] = 0x0829;
	mb_mst_terminal_para.terminal[1].save_place[0] = 7;
	mb_mst_terminal_para.terminal[1].reg_offset[1] = 0x0969;
	mb_mst_terminal_para.terminal[1].save_place[1] = 34;

    //第3个终端为2#主传感器，地址是2
    mb_mst_terminal_para.terminal[2].addr = 2;	//
	mb_mst_terminal_para.terminal[2].word_len = 10;
	//mb_mst_terminal_para.terminal[0].word_offset = 0x0829;
	mb_mst_terminal_para.terminal[2].available_num = 2;
	mb_mst_terminal_para.terminal[2].reg_offset[0] = 0x0829;
	mb_mst_terminal_para.terminal[2].save_place[0] = 5;
	mb_mst_terminal_para.terminal[2].reg_offset[1] = 0x0969;
	mb_mst_terminal_para.terminal[2].save_place[1] = 33;
	
	//第4个终端为2#从传感器，地址是4
    mb_mst_terminal_para.terminal[3].addr = 4;	//
	mb_mst_terminal_para.terminal[3].word_len = 10;
	//mb_mst_terminal_para.terminal[1].word_offset = 0x0829;
	mb_mst_terminal_para.terminal[3].available_num = 2;
	mb_mst_terminal_para.terminal[3].reg_offset[0] = 0x0829;
	mb_mst_terminal_para.terminal[3].save_place[0] = 8;
	mb_mst_terminal_para.terminal[3].reg_offset[1] = 0x0969;
	mb_mst_terminal_para.terminal[3].save_place[1] = 35;
	
    //第5个终端为2#主传感器，地址是5
    mb_mst_terminal_para.terminal[4].addr = 5;	//
	mb_mst_terminal_para.terminal[4].word_len = 10;
	//mb_mst_terminal_para.terminal[0].word_offset = 0x0829;
	mb_mst_terminal_para.terminal[4].available_num = 2;
	mb_mst_terminal_para.terminal[4].reg_offset[0] = 0x0829;
	mb_mst_terminal_para.terminal[4].save_place[0] = 40;
	mb_mst_terminal_para.terminal[4].reg_offset[1] = 0x0969;
	mb_mst_terminal_para.terminal[4].save_place[1] = 36;
	
	//第6个终端为2#从传感器，地址是6
    mb_mst_terminal_para.terminal[5].addr = 6;	//
	mb_mst_terminal_para.terminal[5].word_len = 10;
	//mb_mst_terminal_para.terminal[1].word_offset = 0x0829;
	mb_mst_terminal_para.terminal[5].available_num = 2;
	mb_mst_terminal_para.terminal[5].reg_offset[0] = 0x0829;
	mb_mst_terminal_para.terminal[5].save_place[0] = 41;
	mb_mst_terminal_para.terminal[5].reg_offset[1] = 0x0969;
	mb_mst_terminal_para.terminal[5].save_place[1] = 37;
    
	//MODBUS主站使用的参数开始初始化
	cur_index = 0;
	frame_delay = 0;
	
    mst_modbus_state[0].rece_byte_num = 0;
    mst_modbus_state[0].send_byte_num = 0;
    mst_modbus_state[1].rece_byte_num = 0;
    mst_modbus_state[1].send_byte_num = 0;
    mst_modbus_state[2].rece_byte_num = 0;
    mst_modbus_state[2].send_byte_num = 0;
    mst_modbus_state[3].rece_byte_num = 0;
    mst_modbus_state[3].send_byte_num = 0;
    
    mst_modbus_state[0].connect_flag = 0;
    mst_modbus_state[1].connect_flag = 0;
    mst_modbus_state[2].connect_flag = 0;
    mst_modbus_state[3].connect_flag = 0;
   
    
    
    
	//还需要在此处打开串口，如果统一打开的话，此处可省略
}

/*
主站的主运行函数,该函数建议20MS被调用一次
*/
void Pro_Mb_Mst(void)
{
	//如果没有终端，则返回
	if (mb_mst_terminal_para.num == 0)
	{
		return;
	}
	
	
	//进行时间累计
	frame_delay++;
	
	//如果时间到
	if (frame_delay > MB_SEND_TIME)		//可以发送数据状态
	{
		frame_delay = 0;
		
		//把上次收到的应答进行处理
		MB_MST_ReceFrame(MODBUS_MST_USE_PORT);
		MB_MST_ProcessData(MODBUS_MST_USE_PORT);
		
		//先把本终端的数据要完毕
		cur_mega++;
		if (cur_mega >= mb_mst_terminal_para.terminal[cur_index].available_num )
		{
			cur_mega = 0;

			//切换到下一个终端，如果终端循完，从头开始
			cur_index++;
			if (cur_index >= mb_mst_terminal_para.num )
			{
				cur_index = 0;
			}

		}		
		
		//开始进行组织帧
		MB_MST_SetupFrame();
		//发送出去
		MB_MST_SendFrame(MODBUS_MST_USE_PORT);
		
		
	}
	
}

/*
开始组织数据帧
*/
static void MB_MST_SetupFrame(void)
{
	UINT16 temp;
	
	//使用最简单的主站功能，只组织一帧YC帧
	
	mb_mst_send.modbus_send_buf[0] = mb_mst_terminal_para.terminal[cur_index].addr;
	
	mb_mst_send.modbus_send_buf[1]  = 0x03;	
	
	//
	//mb_mst_send.modbus_send_buf[2]  = mb_mst_terminal_para.terminal[cur_index].word_offset >> 8 ;	
	//mb_mst_send.modbus_send_buf[3]  = mb_mst_terminal_para.terminal[cur_index].word_offset & 0xFF ;
	mb_mst_send.modbus_send_buf[2]  = mb_mst_terminal_para.terminal[cur_index].reg_offset[cur_mega] >> 8 ;	
	mb_mst_send.modbus_send_buf[3]  = mb_mst_terminal_para.terminal[cur_index].reg_offset[cur_mega] & 0xFF ;
	//
	mb_mst_send.modbus_send_buf[4] = 0x0;	
	mb_mst_send.modbus_send_buf[5] = mb_mst_terminal_para.terminal[cur_index].word_len;	
	
	//CRC
	temp = CheckCrc(mb_mst_send.modbus_send_buf,6);
	mb_mst_send.modbus_send_buf[6] = temp & 0xff;
	mb_mst_send.modbus_send_buf[7] = temp >> 8;
	
	//更新发送长度
	mb_mst_send.send_len = 8;
	
	
    
}

static void MB_MST_SendFrame(u8 port)
{
	if (mb_mst_send.send_len)
	{
		send_data(MODBUS_MST_USE_PORT,mb_mst_send.modbus_send_buf,mb_mst_send.send_len);
		
        //统计发出的数据
        mst_modbus_state[cur_index].send_byte_num += mb_mst_send.send_len;
        
		mb_mst_send.send_len = 0;
	}
}


/*
接收数据
*/
static void MB_MST_ReceFrame(u8 port)
{
	UINT32 i;
	//
	i = get_rec_len(port);
	
	if (i)
	{
		//接收数据
		
		i =	procure_data( mb_mst_rece.modbus_rece_buf, port, i);
		
		
		mb_mst_rece.rece_len = i;		
        
        
		
#ifdef DBG_MB_MST
		sprintf(dbg_mb_mst_buf,"MB MST收到数据%d个\n",i);
		send_str(COM1,(INT8U*)dbg_mb_mst_buf);
#endif
		
		//可以进行接收累计
        //统计接收的数据
        mst_modbus_state[cur_index].rece_byte_num += mb_mst_rece.rece_len;
	}
}

/*
处理数据,此处所有处理的寄存器均假设成了浮点数，如果有寄存器量，以后需要更改此函数
*/
static void MB_MST_ProcessData(u8 port)
{
	UINT32 check_value;
	
	UINT32 value;
	
	UINT8 temp;
	UINT8 temp_offset;	//可用寄存器的基值
	
	while (mb_mst_rece.rece_len >= 25)
	{
	
		
#ifdef DBG_MB_MST
        sprintf(dbg_mb_mst_buf,"rece_len=%d",mb_mst_rece.rece_len);
		send_str(COM1,(INT8U*)dbg_mb_mst_buf);
		
		sprintf(dbg_mb_mst_buf,"cur_index = %d,buf[0]=%d,add=%d\n",cur_index,mb_mst_rece.modbus_rece_buf[0],mb_mst_terminal_para.terminal[cur_index].addr);
		send_str(COM1,(INT8U*)dbg_mb_mst_buf);
		
#endif
		

		
		//开始判断地址
		if (mb_mst_rece.modbus_rece_buf[0] == mb_mst_terminal_para.terminal[cur_index].addr)	//地址正确
		{
			
#ifdef DBG_MB_MST
			
			send_str(COM1,(INT8U*)"地址正确\n");
#endif
			if (mb_mst_rece.modbus_rece_buf[1] == CODE_YC)
			{
				temp = mb_mst_rece.modbus_rece_buf[2];
				
				check_value = mb_mst_rece.modbus_rece_buf[temp+4];
				check_value <<= 8;
                check_value += mb_mst_rece.modbus_rece_buf[temp+3];
				//CRC计算
				if (check_value == CheckCrc(mb_mst_rece.modbus_rece_buf,temp+3))
				{
#ifdef DBG_MB_MST
					
					send_str(COM1,(INT8U*)"校验正确\n");
#endif
					
					//整帧校验通过，可以从中提出数据了
					if (mb_mst_terminal_para.terminal[cur_index].available_num )
					{
						
							//开始分段取出数据
							
							value = Float2Int(	mb_mst_rece.modbus_rece_buf[9],
												mb_mst_rece.modbus_rece_buf[10],
												mb_mst_rece.modbus_rece_buf[7],
												mb_mst_rece.modbus_rece_buf[8]);
							//存放的位置,把刚才得到的数据按参数规定的基址保存进去
							temp_offset = mb_mst_terminal_para.terminal[cur_index].save_place[cur_mega];
							
#ifdef DBG_MB_MST
							sprintf(dbg_mb_mst_buf,"保存位置=%d",temp_offset);
							send_str(COM1,(INT8U*)dbg_mb_mst_buf);								
#endif

							//ad_measure[temp_offset].ad_value = value;
                            //如果是1#传感器的第一个量时，因最大值是500 000，故缩小10倍
                            if ((cur_index <2) && (cur_mega == 0))
                            {
                                value /= 10;
                            }
                            
                            
                            yc_buf[temp_offset] = value;

#ifdef DBG_MB_MST
							sprintf(dbg_mb_mst_buf,"ycbuf[5]=%d,%d",yc_buf[temp_offset],yc_buf[5]);
							send_str(COM1,(INT8U*)dbg_mb_mst_buf);								
#endif
					
						
						
					}
					//清掉缓冲区
					mb_mst_rece.rece_len = 0;
                    
                    //置位连接标志
                    mst_modbus_state[cur_index].connect_flag = 1;
                    
					break;
				}
				else
				{
					//移出一个字节
					pop_one_byte(	mb_mst_rece.modbus_rece_buf,mb_mst_rece.rece_len);
					mb_mst_rece.rece_len--;
				}
			}
            else
            {
				//移出一个字节
				pop_one_byte(	mb_mst_rece.modbus_rece_buf,mb_mst_rece.rece_len);
				mb_mst_rece.rece_len--;
			}
		}
		else
		{
			//移出一个字节
			pop_one_byte(	mb_mst_rece.modbus_rece_buf,mb_mst_rece.rece_len);
            mb_mst_rece.rece_len--;	

		}
        
        //置位通讯错误
        //
        mst_modbus_state[cur_index].connect_flag = 0;
	}
}


u32 Float2Int(UINT8 byte3,UINT8 byte2, UINT8 byte1, UINT8 byte0)
{
	aaa float_temp;
	
	float_temp.gh[3] = byte3;
	float_temp.gh[2] = byte2;
	float_temp.gh[1] = byte1;
	float_temp.gh[0] = byte0;

    //扩大10倍
	float_temp.abc *= 10;
    
	//return (INT16)(float_temp.abc);
	return (u32)(float_temp.abc);
	
}




