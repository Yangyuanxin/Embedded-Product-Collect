#include "hsc32i1.h" 

extern void TimerDelay_Ms(u16 counter);


//初始化IIC接口
void HSC32I1BaseInit(void)
{
	IIC_Init();
	HSC32RstInit();
}


//在HSC32I1指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 HSC32I1_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp = 0;	  	 
	
	IIC_Start();  
	
	IIC_Send_Byte(0x60);			//发送写命令(1010 0000)
	IIC_Wait_Ack(); 

	IIC_Send_Byte(ReadAddr);		//发送高地址
	IIC_Wait_Ack();	
	
	IIC_Start();  	 				//开始读取数据	   
	IIC_Send_Byte(0x61);			//进入接收模式			   
	IIC_Wait_Ack();	 
	
    temp = IIC_Read_Byte(0);		//读取一个Byte的数据		
	
    IIC_Stop();						//产生一个停止条件	
    
	return temp;
}


//在HSC32I1指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void HSC32I1_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  					//开始传输
	
	IIC_Send_Byte(0x60);	    	//发送写命令
	IIC_Wait_Ack();					//等待ACK 	
	
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();					//等待ACK  	
    IIC_Stop();						//产生一个停止条件 
	delay_ms(10);	 
}


//读取指定长度的数据
u8 HSC32I1_ReadLenByte(u8 ReadAddr,u8 *Buf,u16 Size)
{				  
	u8 i= 0;	  	 
		
	IIC_Start();  					//产生一个开始条件
	
	IIC_Send_Byte(0x60);			//发送写命令(1010 0000)
	if( IIC_Wait_Ack() )			//等待应答
    {
        IIC_Stop();
        return 1;
    }
//	//修改
//	IIC_Wait_Ack();
	
	IIC_Send_Byte(ReadAddr);		//写入高地址(从机解析地址)
//	IIC_Wait_Ack();	
	if( IIC_Wait_Ack() )			//等待应答
    {
        IIC_Stop();
        return 1;
    }
	
	IIC_Start();  					//开始读取数据(从机从指定地址发送数据)	
	   
	IIC_Send_Byte(0x61);			//进入读取数据模式			   
//	IIC_Wait_Ack();	 				//等待主机应答
	if( IIC_Wait_Ack() )			//等待应答
    {
        IIC_Stop();
        return 1;
    }
	while(Size)
    {
        if(Size == 1)
			*Buf = IIC_Read_Byte(0); 	//读数据,发送nACK
        else
			*Buf = IIC_Read_Byte(1);	//读数据,发送ACK
        Size--;
        Buf++;
    }
		
    IIC_Stop();							//产生一个停止条件	
	
	return 0;
}


//读取指定长度的数据
void HSC32I1_WriteLenByte(u8 WriteAddr,u8 *Buf,u16 Size)
{				  
	u8 i= 0;	  	 
	
	IIC_Start();  				//开始传输
	
	IIC_Send_Byte(0x60);	    //发送写命令
	IIC_Wait_Ack();				//等待从机的ACK
	
	IIC_Send_Byte(WriteAddr);	//发送数据写入的地址
	IIC_Wait_Ack();	   	
 		
	for(i = 0; i < Size; i++)	
	{
		IIC_Send_Byte(Buf[i]);	//发送字节							   
		IIC_Wait_Ack();			//等待从机的ACK  	
	}
	
    IIC_Stop();					//停止传输数据 
//	delay_ms(10);	 
}




