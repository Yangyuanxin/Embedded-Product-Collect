/*

*/

#define _MODBUS_C

#include "head.h"
#include "uart.h"

u8 len;

u8 cur_port;


//-----------------------------------------------------


//
static void ReceFrame(u8 port);
static void ProcessData(u8 port);
static void SendFrame(u8 port);
static void SetupYc(UINT16 *scr,UINT8 len);
static void SetupYx(UINT16 *scr,UINT8 len);
static void SetupPara(UINT16 data_offset,UINT16 value);
static void SetupWriteYk(UINT16 yk_flag,UINT8 yx_num);
//
static void ProReadYk(UINT8 *pFrame);

static void ProReadYc(UINT8 *pFrame);
static void ProEdit(UINT8 *pFrame);
static void ProWriteYk(UINT8 *pFrame);
static void ProOneEdit(UINT8 *pFrame);

u16 FloatInt(UINT8 byte1, UINT8 byte0);


static void SetupReadYk(UINT16 *scr,UINT8 len);


void TaskModbus(u8 port)
{

	cur_port = port;
  //  printf("port=%d\r\n",cur_port);
	ReceFrame(cur_port);
	ProcessData(cur_port);
	SendFrame(cur_port);

    //TaskSensor();  
}

/*
读取数据
*/
static void ReceFrame(u8 port)
{
	UINT32 i;
	//
	i = get_rec_len(port);

	if (i)
	{
		//接收数据
		if ((MODBUS_SEND_BUF_LEN - rece_buf[port].rece_len) > i)
		{			
			i =	procure_data(	&rece_buf[port].modbus_rece_buf[rece_buf[port].rece_len],
								port,
								i);
			//
		//	modbus_state.rece_byte_num += i;
			//
			rece_buf[port].rece_len += i;
			
		}
	}
   
}


static void ProcessData(u8 port)
{
	UINT32 check_value;	
	//

	while(rece_buf[port].rece_len >= MODBUS_FRAME_MIN_LEN)
	{
		if (rece_buf[port].modbus_rece_buf[0] == para.value.addr)
		{	
                  
                  
			if (	(rece_buf[port].modbus_rece_buf[1] == CODE_YX)
				|| 	(rece_buf[port].modbus_rece_buf[1] == CODE_YC)
				|| 	(rece_buf[port].modbus_rece_buf[1] == CODE_YK)
                || 	(rece_buf[port].modbus_rece_buf[1] == READ_YK)
                || 	(rece_buf[port].modbus_rece_buf[1] == ONE_EDIT)
				)
			{
                          
				check_value = rece_buf[port].modbus_rece_buf[7];
				check_value <<= 8;
				check_value += rece_buf[port].modbus_rece_buf[6];
				//
				if (check_value == CheckCrc(rece_buf[port].modbus_rece_buf,6))
				{
                                  
					if (rece_buf[port].modbus_rece_buf[1] == CODE_YX)
					{
						ProReadYx(rece_buf[port].modbus_rece_buf);
					}
					else if (rece_buf[port].modbus_rece_buf[1] == CODE_YC)
					{
						ProReadYc(rece_buf[port].modbus_rece_buf);
					}
                    else if (rece_buf[port].modbus_rece_buf[1] == READ_YK)
					{
						
						ProReadYk(rece_buf[port].modbus_rece_buf);
					}
                    else if (rece_buf[port].modbus_rece_buf[1] == ONE_EDIT)
					{
						ProOneEdit(rece_buf[port].modbus_rece_buf);
                   
					}
					else
					{
						ProWriteYk(rece_buf[port].modbus_rece_buf);
						

					}
					//移出8个字节
					PopNByte(	rece_buf[port].modbus_rece_buf,
								rece_buf[port].rece_len,
								8);
					rece_buf[port].rece_len -= 8;
					break;
				}
				else
				{
					pop_one_byte(rece_buf[port].modbus_rece_buf,rece_buf[port].rece_len);
					rece_buf[port].rece_len--;
					continue;
				}
			}
			//修改多个参数
			else if (rece_buf[port].modbus_rece_buf[1] == CODE_EDIT)		
			{
				UINT32 len;
				//通道长度
				len = rece_buf[port].modbus_rece_buf[5];
				len <<= 8;
				len += rece_buf[port].modbus_rece_buf[4];
				if (len>(YC_MAX_NUM-PARA_OFFSET))
				{
					pop_one_byte(rece_buf[port].modbus_rece_buf,rece_buf[port].rece_len);
					rece_buf[port].rece_len--;
					continue;
				}
				//
				//整帧是否收够
				if (rece_buf[port].rece_len < len*2+8)
				{
					break;
				}
				//
				check_value = rece_buf[port].modbus_rece_buf[7+len*2];
				check_value <<= 8;
				check_value += rece_buf[port].modbus_rece_buf[6+len*2];
				if (check_value == CheckCrc(rece_buf[port].modbus_rece_buf,6+len*2))
				{
					ProEdit(rece_buf[port].modbus_rece_buf);
					//移出整帧的字节
					PopNByte(	rece_buf[port].modbus_rece_buf,
								rece_buf[port].rece_len,
								8+len*2);
					rece_buf[port].rece_len -= 8+len*2;
					break;
				}
				else
				{
					pop_one_byte(rece_buf[port].modbus_rece_buf,rece_buf[port].rece_len);
					rece_buf[port].rece_len--;
					continue;
				}
			}
			else
			{				
				pop_one_byte(	rece_buf[port].modbus_rece_buf,
								rece_buf[port].rece_len
							);
				rece_buf[port].rece_len--;
				continue;
			}			
			//						
		}
		else
		{
			pop_one_byte(	rece_buf[port].modbus_rece_buf,
							rece_buf[port].rece_len);
			rece_buf[port].rece_len--;
			continue;
		}
	}
}
/*
发送数据
*/
static void SendFrame(u8 port)
{
  if (send_buf[port].send_len)
  {
  //  printf("peng123=%d\r\n",send_buf.send_len);
  //  
    send_data(port,send_buf[port].modbus_send_buf,send_buf[port].send_len);
    //
    //modbus_state.send_byte_num += send_buf.send_len;
    //
    send_buf[port].send_len = 0;
  }

}


/*
处理读信号量帧
此帧的帧长8个字节
通讯约定的起始地址是从0000h，其实在此规约中对应的是遥信缓冲区
如果说起始地址超过了最高地址的话，认为无效，如果说所要的数据超过了最高地址的话，有多少送多少

*/
static void ProReadYx(UINT8 *pFrame)
{
	UINT8 *p;
    u8 dbyte_offset;//双字节偏移
    u8 dbyte_num;//双字节长度
	UINT32 length;
	UINT32 data_offset;

	UINT8 i,bit_offset;	//位偏移地址

	UINT16  temp_buf[32];
	//
	p = pFrame;
	//
	data_offset = *(p+2);
	data_offset <<= 8;
	data_offset += *(p+3);
    
//     if((data_offset%16) && (data_offset > 16))//看是否是16的整数倍
//     {
//       dbyte_offset = data_offset/16+1;
//     }
//     else
//     {
      dbyte_offset = data_offset/16;      
 //    }
    
	//
	length = *(p+4);
	length <<= 8;
	length += *(p+5);
    
    if(length%16)//看是否是16的整数倍
    {
      dbyte_num = length/16+1;
    }
    else
    {
      dbyte_num = length/16;
      
    }

	bit_offset = data_offset%16;	//得到位偏移地址s
	//重新整理数据
	//printf("OFFSET = %04x,length= %d,dbyte_num=%d \r\n",dbyte_offset,length, dbyte_num);
	
	for (i=0; i<dbyte_num; i++)
	{
		//printf("buf-%d=%04x ",dbyte_offset+i,yx_buf[dbyte_offset+i]);
		//printf("buf-%d=%04x ",dbyte_offset+i+1,yx_buf[dbyte_offset+i+1]);

		//printf("bit_offset=%d ", bit_offset);

		if (bit_offset)
		{
			temp_buf[i] = (yx_buf[dbyte_offset+i] >> bit_offset) + (yx_buf[dbyte_offset+i+1] << (16- bit_offset));
		}
		else
		{
			temp_buf[i] = yx_buf[dbyte_offset+i];
		}

		//printf("temp_buf=%04x \r\n",temp_buf[i]);
	}
	
	//
    /*********
	if (
		(data_offset > (YX_OFFSET+YX_MAX_NUM))			
		|| (length == 0)
		)
	{
		//所要数据无效，组织无数据帧
		SetupYx(yx_buf,0);
		return;
	}
    ***************/
	//
	
	//
	SetupYx(temp_buf,dbyte_num);
	return;	
}
void SetupWriteYk(UINT16 yk_flag,UINT8 yk_num)
{
    UINT32 i;

	send_buf[cur_port].modbus_send_buf[0] = para.value.addr;
	send_buf[cur_port].modbus_send_buf[1] = CODE_YK;
	//
	send_buf[cur_port].modbus_send_buf[2] = 0; 
	send_buf[cur_port].modbus_send_buf[3] = yk_num;
	//返回字节数						
	send_buf[cur_port].modbus_send_buf[4] = yk_flag&0xff; 
	send_buf[cur_port].modbus_send_buf[5] = 0;	
	//CRC
	i = CheckCrc(send_buf[cur_port].modbus_send_buf,6);
	send_buf[cur_port].modbus_send_buf[6] = i & 0xff;
	send_buf[cur_port].modbus_send_buf[7] = i>>8;	
	//
	send_buf[cur_port].send_len = 8;
}
static void ProOneEdit(UINT8 *pFrame)
{
	UINT8 *p;
    u16 value;
    UINT32 data_offset;
    //
    p = pFrame;
    //
    data_offset = *(p+2);
    data_offset <<= 8;
    data_offset += *(p+3);
    
    value = *(p+4);
    value <<= 8;
    value += *(p+5);
	if (data_offset >= PARA_OFFSET)	//为了不与遥测冲突
	{
		yc_buf[data_offset] = value;
		para.value_group[data_offset-16+2]=value;
		SavePara();	
	} 
    SetupPara(data_offset,value);
    return;
}
static void ProWriteYk(UINT8 *pFrame)
{
    UINT8 *p;
	u16 yk_on_off;
	p = pFrame;	
	//
	yk_num=*(p+3);
//	printf("yk_num = %d\r\n",yk_num);
	yk_on_off = *(p+4);
//	printf("yk_on_off = %d\r\n",yk_on_off);
 //   printf("yk_num=%d\r\n",yk_num);
	if((*(p+4))==0xff)
	{

			yx_buf[yk_num/16] |= 1<<(yk_num%16);
	

	}
	else if((*(p+4))==0x00)
	{

			yx_buf[yk_num/16] &= ~(1<<(yk_num%16));
	

	}
	SetupWriteYk(yk_on_off,yk_num);
	return;	
  
}

/*
处理读寄存器帧
*/
static void ProReadYk(UINT8 *pFrame)
{
	UINT8 *p;
    u8 dbyte_offset;//双字节偏移
    u8 dbyte_num;//双字节长度
	UINT32 length;
	UINT32 data_offset;

	UINT8 i,bit_offset;	//位偏移地址

	UINT16  temp_buf[32];
	//
	p = pFrame;
	//
	data_offset = *(p+2);
	data_offset <<= 8;
	data_offset += *(p+3);
    
//     if((data_offset%16) && (data_offset > 16))//如时不是16的整数倍
//     {
//       dbyte_offset = data_offset/16+1;
//     }
//     else//如果是16的整数倍
//     {
      dbyte_offset = data_offset/16;      
/*    }*/
    
	//
	length = *(p+4);
	length <<= 8;
	length += *(p+5);
    
    if(length%16)//如果不是16的整数倍
    {
      dbyte_num = length/16+1;
    }
    else//如果是16的整数倍
    {
      dbyte_num = length/16;      
    }
	bit_offset = data_offset%16;	//得到位偏移地址s
	//重新整理数据
	//printf("OFFSET = %04x,length= %d,dbyte_num=%d \r\n",dbyte_offset,length, dbyte_num);
	
	for (i=0; i<dbyte_num; i++)
	{
		//printf("buf-%d=%04x ",dbyte_offset+i,yx_buf[dbyte_offset+i]);
		//printf("buf-%d=%04x ",dbyte_offset+i+1,yx_buf[dbyte_offset+i+1]);

		//printf("bit_offset=%d ", bit_offset);

		if (bit_offset)
		{
			temp_buf[i] = (yx_buf[dbyte_offset+i] >> bit_offset) + (yx_buf[dbyte_offset+i+1] << (16- bit_offset));
		}
		else
		{
			temp_buf[i] = yx_buf[dbyte_offset+i];
		}

		//printf("temp_buf=%04x \r\n",temp_buf[i]);
	}
	
	//
    /*********
	if (
		(data_offset > (YX_OFFSET+YX_MAX_NUM))			
		|| (length == 0)
		)
	{
		//所要数据无效，组织无数据帧
		SetupYx(yx_buf,0);
		return;
	}
    ***************/
	//
	
	//
	SetupReadYk(temp_buf,dbyte_num);
	return;	
  
  
}
static void ProReadYc(UINT8 *pFrame)
{
	UINT8 *p;
	UINT32 length;
	UINT32 data_offset;        
	//
	p = pFrame;
	//
	data_offset = *(p+2);
	data_offset <<= 8;
	data_offset += *(p+3);
	//
	length = *(p+4);
	length <<= 8;
	length += *(p+5);
    //    if(data_offset<100)
	//   {
    SetupYc(&yc_buf[data_offset-YC_OFFSET],length); 
	
	//   }
    //    else
	//   {
	//    SetupYc(&para_temp.value_group[data_offset-100],length);
    //    }
	
	
	return;	
}
/*
处理修改参数帧
*/
void ProEdit(UINT8 *pFrame)
{
	UINT8 *p;
	UINT32 length,i;
	UINT32 data_offset;
	//
	p = pFrame;
	//
	data_offset = *(p+2);
	data_offset <<= 8;
	data_offset += *(p+3);
	//
	length = *(p+4);
	length <<= 8;
	length += *(p+5);	
	//
	if (
		(data_offset > YC_MAX_NUM)
		|| (data_offset < PARA_OFFSET)
		|| (length == 0)
		)
	{
		
		//SetupPara(yc_buf,0);
		return;
	}
	//
	if (length > (YC_MAX_NUM-data_offset ) )
	{
		length = YC_MAX_NUM-data_offset;
	}
	//
	//保存参数
	for (i=0; i<length; i++)
	{
		//modbus_para_buf[data_offset-PARA_OFFSET+i] = (*(p+7+i*2))<<8 +*(p+6+i*2);
          para.value_group[data_offset+i]=	(*(p+7+i*2))<<8 +*(p+6+i*2);
          
          //逻辑中取的参数为control_para_temp中的参数，故在保存的同时，也让继电器的参数同时起作用
//           if((data_offset+i>=1)||(data_offset+i<=10))
//           {
//             relay_state_temp[data_offset+i]=para_temp.value_group[data_offset+i+1];
//           }
	}
    SavePara();
	
	//
	SetupPara(data_offset,length);
	//
//	OSTimeDly(OS_TICKS_PER_SEC/2);	
//	para_change_flag = 1;		//参数改变，需要重新初始化
	return;	
}

static void SetupReadYk(UINT16 *scr,UINT8 len)
{
  	UINT32 i;
	//
   //     printf("led=%d",len);
	if (len)	//组织数据
	{
		send_buf[cur_port].modbus_send_buf[0] = para.value.addr;
		send_buf[cur_port].modbus_send_buf[1] = READ_YK;
		//返回字节数						
		
		send_buf[cur_port].modbus_send_buf[2] = (len*2)&0xff; 
		//
		for (i=0; i<len; i++)
		{
			send_buf[cur_port].modbus_send_buf[3+i*2] = *scr & 0xff;
			send_buf[cur_port].modbus_send_buf[4+i*2] = *scr >> 8;
			scr++;
		}
		//CRC
		i = CheckCrc(send_buf[cur_port].modbus_send_buf,len*2+3);
		send_buf[cur_port].modbus_send_buf[len*2+3] = i & 0xff;
		send_buf[cur_port].modbus_send_buf[len*2+4] = i>>8;	
		//
		send_buf[cur_port].send_len = len*2 +5;	
        }
}
/*
组织读信号量帧
输入参数：
scr:	数据源，(字)
len:	长度
*/
static void SetupYx(UINT16 *scr,UINT8 len)
{
	UINT32 i;
	//
   //     printf("led=%d",len);
	if (len)	//组织数据
	{
		send_buf[cur_port].modbus_send_buf[0] = para.value.addr;
		send_buf[cur_port].modbus_send_buf[1] = CODE_YX;
		//返回字节数						
		
		send_buf[cur_port].modbus_send_buf[2] = (len*2)&0xff; 
		//
		for (i=0; i<len; i++)
		{
			send_buf[cur_port].modbus_send_buf[3+i*2] = *scr & 0xff;
			send_buf[cur_port].modbus_send_buf[4+i*2] = *scr >> 8;
			scr++;
		}
		//CRC
		i = CheckCrc(send_buf[cur_port].modbus_send_buf,len*2+3);
		send_buf[cur_port].modbus_send_buf[len*2+3] = i & 0xff;
		send_buf[cur_port].modbus_send_buf[len*2+4] = i>>8;	
		//
		send_buf[cur_port].send_len = len*2 +5;	
	}
	else		//如果出错，要进行错误回应
	{
		
	}
}
/*
组织读寄存器帧
*/
static void SetupYc(UINT16 *scr,UINT8 len)
{
	UINT32 i;
	
	//
	if (len)	//组织数据
	{
		
		send_buf[cur_port].modbus_send_buf[0] = para.value.addr;
		send_buf[cur_port].modbus_send_buf[1] = CODE_YC;
		
		send_buf[cur_port].modbus_send_buf[2] = (len*2)&0xff;
		
		//
		for (i=0; i<len; i++)
		{
			
			send_buf[cur_port].modbus_send_buf[3+i*2] = *scr >> 8;
			send_buf[cur_port].modbus_send_buf[4+i*2] = *scr & 0xff;
			scr++;
		}
		//CRC
		i = CheckCrc(send_buf[cur_port].modbus_send_buf,len*2+3);
		send_buf[cur_port].modbus_send_buf[len*2+3] = i & 0xff;
		send_buf[cur_port].modbus_send_buf[len*2+4] = i>>8;	
		//
		send_buf[cur_port].send_len = len*2 + 5;	
		
	}
	else		//如果出错，要进行错误回应
	{
		
	}
	
}

/*
组织参数修改确认帧
*/
static void SetupPara(UINT16 data_offset,UINT16 value)
{
	UINT32 i;
	
	send_buf[cur_port].modbus_send_buf[0] = para.value.addr;
	send_buf[cur_port].modbus_send_buf[1] = ONE_EDIT;
	//
	send_buf[cur_port].modbus_send_buf[2] = data_offset>>8;
	send_buf[cur_port].modbus_send_buf[3] = data_offset&0xff; 	
	//返回字节数
	send_buf[cur_port].modbus_send_buf[4] = value>>8;	
	send_buf[cur_port].modbus_send_buf[5] = value&0xff; 
	
	//CRC
	i = CheckCrc(send_buf[cur_port].modbus_send_buf,6);
	send_buf[cur_port].modbus_send_buf[6] = i & 0xff;
	send_buf[cur_port].modbus_send_buf[7] = i>>8;	
	//
	send_buf[cur_port].send_len = 8;
}
void UpdataModbusYc(void)
{
            
//     for (i=0; i<16; i++)
//     {
//       yc_buf[i] = ad_measure[i].ad_value;       
//     }	
//     for(i=0;i<48;i++)
//     {
//       yc_buf[i+16]=para.value_group[i+2]; //空出前两个寄存器分别是0x68标识和 本机地址。
//     }
    //德州双汇自洁
//#ifdef CUR_PROJECT_DE_ZHOU_SH_ZI_JIE
    //德州双汇自洁，AD处只接了3个4-20MA传感器。分别是 WD,余氯，辅助PH
    //yc_buf[0] = ad_measure[1].ad_value;	//余氯
	//yc_buf[5] = ad_measure[2].ad_value;	//辅助PH
	//yc_buf[2] = ad_measure[0].ad_value;	//WD
	//yc_buf[3] = ad_measure[3].ad_value;
//#endif
    yc_buf[ZJ_AI_MST_3_VAL] = ad_measure[3].ad_value / 10;
    yc_buf[ZJ_AI_SLV_3_VAL] = ad_measure[2].ad_value / 10;
    
    yc_buf[ZJ_AI_SLV2_3_VAL] = ad_measure[1].ad_value / 10;
    
}
/*
更新遥信
*/
void UpdataModbusYx(UINT16 *p_scr,UINT8 len)
{
	UINT32 i;
	//	
	if (len>YX_MAX_NUM)
	{
		len = YX_MAX_NUM;
	}
	//-----------------------------------------	
	
	//
	for (i=0; i<len; i++)
	{
		yx_buf[i] = *p_scr++;
	}
	//
	
	//-----------------------------------------	
		
}


void pop_one_byte(UINT8 *scr,UINT8 length)
{
	UINT32 i;

	for(i=0; i< length-1; i++)
	{
		scr[i] = scr[i+1];							/* 逐字节前移 */
	}
}


void PopNByte(UINT8 *scr,UINT8 length, UINT8 pop_len)
{
	UINT32 i;
	//
	for (i=0; i<length - pop_len; i++)
	{
		scr[i] = scr[i+pop_len];
	}
}

UINT16 CheckCrc(UINT8 *source,UINT8 length)
{
	UINT16 crc_register=0xffff;
	UINT8 i,j;
	UINT8 back_check;

	for(i=0;i<length;i++)
	{
		crc_register=crc_register^source[i];
		
		for(j=0;j<8;j++)
		{
			back_check = crc_register & 1;
			crc_register = crc_register >> 1;
			crc_register = crc_register & 0x7fff;
			
			if(back_check==1)
			{
				crc_register = crc_register^0xa001;
				crc_register = crc_register & 0xffff;
			}
		}
	}
	return(crc_register);
}




