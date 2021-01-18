/*********************************************************************************
* 描述    ：通过stm32的spi1读取max6675的温度值，并通过uart1发送出来
 *          
 * 实验平台：STM32开发板
 * 库版本  ：ST3.0.0
 * 硬件连接： ------------------------------------
 *           |PA6-SPI1-MISO：MAX6675-SO          |
 *           |PA7-SPI1-MOSI：MAX6675-SI          |
 *           |PA5-SPI1-SCK ：MAX6675-SCK         |
 *           |PA4-SPI1-NSS ：MAX6675-CS          |
 *            ------------------------------------
**********************************************************************************/
#include "max6675.h"
#include "usart.h"
/*
 * 函数名：SPI1_Init
 * 描述  MAX6675 接口初始化
 * 输入  ：无 
 * 输出  ：无
 * 返回  ：无
 */																						  
void SPI_MAX6675_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
	
	/* 使能 SPI1 时钟 */                         
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	/* ---------通信I/O初始化----------------
	 * PA5-SPI1-SCK :MAX6675_SCK
	 * PA6-SPI1-MISO:MAX6675_SO
	 * PA7-SPI1-MOSI:MAX6675_SI	 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---------控制I/O初始化----------------*/
	/* PA4-SPI1-NSS:MAX6675_CS */							// 片选
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);						// 拉高CSN
 
	SPI_Cmd(SPI1, DISABLE); // SPI外设不使能，先关闭SPI，再进行设置
	/* SPI1 配置 */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	
	/* 使能 SPI1  */
	SPI_Cmd(SPI1, ENABLE); 
//	GPIO_SetBits(GPIOA, GPIO_Pin_4);//拉高开始使用
}

/*
 *
 *
 *
 */
u8 MAX6675_ReadByte(void)
{
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, 0xff); //通过外设SPIx发送一个数据，启动传输
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
}
/*
D15始终无用
D14~ D3对应于热电偶模拟输入电压的数字转换量
D2用于检测热电偶是否断线(D2为1表明热电偶断开) 
D1为MAX6675的标识符
D0为三态。
*/
float getMax6675_Data(void)
{
	u16 t,tempData;
	u8 c,flag;
	float temprature;
	MAX6675_CS = 0;//拉低CS，使SO开始输出数据
	c = MAX6675_ReadByte();//读取高8位
	tempData = c;
//	printf("高8位:%x\r\n",c);
	tempData = tempData<<8;//左移8位，留出低8位
	c = MAX6675_ReadByte();//读取低8位
//	printf("低8位:%x\r\n",c);
	MAX6675_CS = 1;//拉高CS，使SO停止输出数据，MAX6675进行下一次转换
	
	tempData = tempData|c;//将低8位合并进tempData
	flag = tempData&0x0004;	//得到D2数据，flag保存了热电偶的连接状态
	printf("flag:%d ",flag);
	t = tempData<<1;		//左移一位，删除掉D15
	t = t>>4;//删除掉D0-2
	temprature = t*0.25;
	printf("原始数据:%d ",t);
	printf("温度：%4.2f\r\n",temprature);
	if(tempData!=0)							//max6675有数据返回
	{
		if(flag==0)//热电偶已连接
		{
			return temprature;//返回读出来的原始数据
		}	
		else							
		{
			return MAX6675Left;//热电偶掉线
		}
	
	}
	else								//max6675没有数据返回
	{
		return MAX6675Error;
	}
}
