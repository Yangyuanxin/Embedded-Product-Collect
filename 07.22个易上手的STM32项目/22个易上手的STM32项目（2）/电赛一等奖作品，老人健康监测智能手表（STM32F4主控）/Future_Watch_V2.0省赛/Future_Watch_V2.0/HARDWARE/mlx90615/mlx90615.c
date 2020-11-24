#include "mlx90615.h"
#include "delay.h"
#include "gpio.h"
#include "sys.h"

#define ACK	 0
#define	NACK 1



//MLX90615
#define SA			    0x00	// 从机地址
#define DEFAULT_SA		0x5B	// 多个时地址默认为0x5B
#define RAM_ACCESS	    0x20	// RAM 访问命令
#define EEPROM_ACCESS	0x10	// EEPROM 访问命令
#define RAM_TA		    0x06	// Ta 在RAM中的地址，Ta为封装(环境) 温度
#define RAM_TO  		0x07	// To 在RAM中的地址，To为物体温度


#define SMBUS_PORT	    GPIOA
#define SMBUS_SCK		11
#define SMBUS_SDA		12

#define SMBUS_SCK_H()	    SMBUS_PORT->ODR |= (1<<SMBUS_SCK)
#define SMBUS_SCK_L()	    SMBUS_PORT->ODR &= ~(1<<SMBUS_SCK)
#define SMBUS_SDA_H()	    SMBUS_PORT->ODR |= (1<<SMBUS_SDA)
#define SMBUS_SDA_L()	    SMBUS_PORT->ODR &= ~(1<<SMBUS_SDA)

#define SMBUS_SDA_PIN()	   ((SMBUS_PORT->IDR) & (1<<SMBUS_SDA)) //读取引脚电平


/*******************************************************************************
** 函数名称: SMBus_StartBit
** 功能描述: 在SMBus总线上产生起始状态
** 参    数: 无
** 返 回 值: 无       
** 作　  者: 
*******************************************************************************/
void SMBus_StartBit(void)
{
    SMBUS_SDA_H();		//设置SDA线为高电平
    delay_us(5);	    //延时5us
    SMBUS_SCK_H();		//设置SCL线为高电平
    delay_us(5);	    //在终止和起始状态之间产生总线空闲时间(Tbuf=4.7us最小值)
    SMBUS_SDA_L();		//设置SDA线为低电平
    delay_us(5);	    //(重复)开始状态后的保持时间，在该时间后，产生第一个时钟信号(Thd:sta=4us最小值)
    SMBUS_SCK_L();	    //设置SCL线为低电平
    delay_us(5);	    
}

/*******************************************************************************
** 函数名称: SMBus_StopBit
** 功能描述: 在SMBus总线上产生终止状态
** 参    数: 无
** 返 回 值: 无       
** 作　  者: 
*******************************************************************************/
void SMBus_StopBit(void)
{
    SMBUS_SCK_L();		//设置SCL线为低电平
    delay_us(5);	    
    SMBUS_SDA_L();		//设置SDA线为低电平
    delay_us(5);	    
    SMBUS_SCK_H();		//设置SCL线为高电平
    delay_us(5);	    //终止状态建立时间(Tsu:sto=4.0us最小值)
    SMBUS_SDA_H();		//设置SDA线为高电平 
}

/*******************************************************************************
** 函数名称: SMBus_SendByte
** 功能描述: 在SMBus总线上发送一个字节
** 参    数: 将要在总线上发送的字节
** 返 回 值: SMBus总线上接收到的位数据      
** 作　  者: 
*******************************************************************************/
u8 SMBus_SendByte(u8 Tx_buffer)
{
    u8	Bit_counter;
    u8	Ack_bit;
    u8	bit_out;

    for(Bit_counter=8; Bit_counter; Bit_counter--)
    {
        if (Tx_buffer&0x80)
        {
            bit_out=1;   //如果TX_buffer的当前位是1,设置bit_out为1
        }
        else
        {
            bit_out=0;  //否则，设置bit_out为0
        }
        SMBus_SendBit(bit_out);	//发送SMBus总线上的当前位   
        Tx_buffer<<=1;			//核对下一位	
    }

    Ack_bit=SMBus_ReceiveBit();	//得到SMBus总线上接收到的位数据
    return	Ack_bit;            //返回SMBus总线上接收到的位数据
}

/*******************************************************************************
** 函数名称: SMBus_SendBit
** 功能描述: 在SMBus总线上发送一位数据
** 参    数: 将要在总线上发送的位
** 返 回 值: 无      
** 作　  者: 
*******************************************************************************/
void SMBus_SendBit(u8 bit_out)
{
    if(bit_out==0)                  //核对字节的位
    {                               //如果bit_out=1，设置SDA线为高电平
        SMBUS_SDA_L();
    }
    else                            
    {
        SMBUS_SDA_H();              //如果bit_out=0，设置SDA线为低电平
    }
    delay_us(2);					//Tsu:dat=250ns 最小值
    SMBUS_SCK_H();					//设置SCL线为高电平
    delay_us(6);					//时钟脉冲高电平脉宽(10.6us)
    SMBUS_SCK_L();					//设置SCL线为低电平
    delay_us(3);					//时钟脉冲低电平脉宽  

    return;
}

/*******************************************************************************
** 函数名称: SMBus_ReceiveBit
** 功能描述: 在SMBus总线上接收一位数据
** 参    数: 无
** 返 回 值: 接收到SMBus总线上接收一位数据      
** 作　  者: 
*******************************************************************************/
u8 SMBus_ReceiveBit(void)
{
    u8 Ack_bit;

    SMBUS_SDA_H();          //引脚靠外部电阻上拉，当作输入
	delay_us(2);			//时钟脉冲低电平脉宽 
    SMBUS_SCK_H();		    //设置SCL线为高电平
    delay_us(5);			//时钟脉冲高电平脉宽(10.6us)
    
    if (SMBUS_SDA_PIN())     //从总线上读取一位，赋Ack_bit
    {
        Ack_bit=1;
    }
    else
    {
        Ack_bit=0;
    }
    SMBUS_SCK_L();			//设置SCL线为低电平
    delay_us(3);			//时钟脉冲低电平脉宽 

    return	Ack_bit;        //返回Ack_bit
}

/*******************************************************************************
** 函数名称: SMBus_ReceiveByte
** 功能描述: 在SMBus总线上接收一个字节
** 参    数: 主控器件发送 ACK(0)/NACK(1)
** 返 回 值: 接收到SMBus总线接收的字节  
** 作　  者: 
*******************************************************************************/
u8 SMBus_ReceiveByte(u8 ack_nack)
{
    u8 	RX_buffer;
    u8	Bit_Counter;

    for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
    {
        if(SMBus_ReceiveBit())			 //由SDA线读取一位
        {
            RX_buffer <<= 1;			//如果位为"1"，赋"1"给RX_buffer 
            RX_buffer |=0x01;
        }
        else
        {
            RX_buffer <<= 1;			//如果位为"0"，赋"0"给RX_buffer
            RX_buffer &=0xfe;
        }
    }
    SMBus_SendBit(ack_nack);			 //发送确认位
    return RX_buffer;
}

/*******************************************************************************
** 函数名称: SMBus_Init
** 功能描述: SMBus初始化
** 参    数: 无
** 返 回 值: 无
** 作　  者: 
*******************************************************************************/
void SMBus_Init(void)
{
  	GPIO_Clock_Set(IOPAEN); //使能PA端口
	GPIO_Init(GPIOA,11,IO_MODE_OUT,IO_SPEED_25M,IO_OTYPER_OD,IO_PULL,IO_H);	//PA11推挽上拉输出
	GPIO_Init(GPIOA,12,IO_MODE_OUT,IO_SPEED_25M,IO_OTYPER_OD,IO_PULL,IO_H);	//PA12推挽上拉输出

	
	// GPIO_InitTypeDef    GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SMBUS_PORT, ENABLE);//使能时钟

    /*配置SMBUS_SCK、SMBUS_SDA为集电极开漏输出*/
//    GPIO_InitStructure.GPIO_Pin = SMBUS_SCK | SMBUS_SDA;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SMBUS_PORT, &GPIO_InitStructure);

    SMBUS_SCK_H();
    SMBUS_SDA_H();
}

/*******************************************************************************
** 函数名称: SMBus_ReadMemory
** 功能描述: 给定受控地址和命令时由MLX90615读取数据
** 参    数: 无
** 返 回 值: 读取到的数据
** 作　  者: 
*******************************************************************************/
u16 SMBus_ReadMemory(u8 slaveAddress, u8 command)
{
    u16 data;			// 由MLX90615返回的寄存器数值
    u8 Pec;				
    u8 DataL=0;			
    u8 DataH=0;			//由MLX90615读取的数据包
    u8 arr[6];			//存储已发送字节的缓冲器
    u8 PecReg;			//存储计算所得PEC字节
    u8 ErrorCounter;	//用于与MLX90614数据沟通

    ErrorCounter=0x00;	// Initialising of ErrorCounter
//	slaveAddress <<= 1;	//2-7位表示从机地址，挂载多个温度探头时使用
	
    do
    {
repeat:
        SMBus_StopBit();			    //如果从机发送停止信号
        --ErrorCounter;				    //Pre-decrement ErrorCounter
        if(!ErrorCounter) 			    //ErrorCounter=0?
        {
            break;					    //跳出循环
        }

        SMBus_StartBit();				//发送起始位
        if(SMBus_SendByte(slaveAddress))//发送从机地址 最低位Wr=0表示接下来写命令
        {
            goto	repeat;			   
        }
        if(SMBus_SendByte(command))	    //写命令
        {
            goto	repeat;		    	
        }

        SMBus_StartBit();					//发送重复起始位	
        if(SMBus_SendByte(slaveAddress+1))	//发送从机地址，最低位Rd=1表示接下来读数据
        {
            goto	repeat;             	
        }

        DataL = SMBus_ReceiveByte(ACK);	//读取低字节数据，主机必须发送 ACK
        DataH = SMBus_ReceiveByte(ACK); //读取高字节数据,主机必须发送 ACK
        Pec = SMBus_ReceiveByte(NACK);	//读取PEC字节, 主机必须发送 NACK
        SMBus_StopBit();				//发送终止位

        arr[5] = slaveAddress;		
        arr[4] = command;			
        arr[3] = slaveAddress+1;	
        arr[2] = DataL;				
        arr[1] = DataH;				
        arr[0] = 0;					
        PecReg=PEC_Calculation(arr);//调用计算 CRC 的函数
    }
    while(PecReg != Pec);		    //判断收到计算CRC是否相等

	data = (DataH<<8) | DataL;	   
    return data;    
}

/*******************************************************************************
** 函数名称: PEC_Calculation
** 功能描述: 根据接收的字节计算PEC码
** 参    数: pec值
** 返 回 值: pec[0] - 该字节包含计算所得crc数值
** 作　  者: 
*******************************************************************************/
u8 PEC_Calculation(u8 pec[])
{
    u8 	crc[6];
    u8	BitPosition=47;
    u8	shift;
    u8	i;
    u8	j;
    u8	temp;

    do
    {
        //载入 CRC数值 0x000000000107
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;

        BitPosition=47;//设置Bitposition的最大值为47

        shift=0;//设置转移位为0

        //在传送的字节中找出第一个"1"
        i=5;//设置最高标志位 (包裹字节标志)
        j=0;//字节位标志，从最低位开始
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }//while语句结束，并找出Bitposition中为"1"的最高位位置

        shift=BitPosition-8; //得到CRC数值将要左移/右移的数值"shift"，对CRC数据左移"shift"位

        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))//核对字节的最高位的下一位是否为"1"
                {
                    temp=1;                //是 - 当前字节 + 1
                }                          //否 - 当前字节 + 0
                else                       //实现字节之间移动"1"
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }
            shift--;
        }

        for(i=0; i<=5; i++) //pec和crc之间进行异或计算
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); 
    return pec[0];        //返回计算所得的crc数值
}

/*******************************************************************************
** 函数名称: SMBus_ReadTemp
** 功能描述: SMBus读取温度
** 参    数: 无
** 返 回 值: 读取到的温度
** 作　  者: 
*******************************************************************************/
float SMBus_ReadTemp(void)
{   
	float temp;
	temp = ((float)SMBus_ReadMemory(SA, RAM_ACCESS|RAM_TO)*(float)0.02-(float)273.15)/(float)0.89;//获取物体温度
	return temp;
}


/*********************************END OF FILE*********************************/
