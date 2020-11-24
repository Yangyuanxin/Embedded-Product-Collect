#include"LCD12864.H"


void LCD12864_PortInit(void)
{
	GPIO_InitTypeDef GPIO_Initstruct;

	//初始化时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//
	GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Pin=GPIO_Pin_1;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);   
	
	GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);   
	
	GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);   
	
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}

void Lcd12864_SendByte(u8 byte)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if((byte&0x80) == 0x80)
			LCD_SDA = 1;
		else 
			LCD_SDA = 0;
		delay_us(1);
		LCD_SCL = 1;
		delay_us(1);
		byte <<=1;
		LCD_SCL = 0;
		delay_us(1);
	}

}


void Lcd12864_WriCmdDat(u8 start,u8 dataa)
{
	u8 start_data,Hdata,Ldata;
	if(start==0)
		start_data=0xf8;
	else
		start_data=0xfa;
	Hdata=dataa&0xf0;
	Ldata=(dataa<<4)&0xf0;
	Lcd12864_SendByte(start_data);
	Lcd12864_SendByte(Hdata);
	Lcd12864_SendByte(Ldata);
}

void Lcd12864_Init(void)
{
	LCD12864_PortInit();
	LCD_CS=1;
	Lcd12864_WriCmdDat(0,0x30);
	delay_ms(2);
	Lcd12864_WriCmdDat(0,0x0c);
	delay_ms(2);
	Lcd12864_WriCmdDat(0,0x01);
	delay_ms(2);
	delay_ms(2);
	delay_ms(2);
	delay_ms(2);
	delay_ms(2);
}


//void Lcd12864_WriAdd(u8 x,u8 y)
//{
//	switch(x)
//	{
//		case 0:Lcd12864_WriCmdDat(0,0x80+y);break;
//		case 1:Lcd12864_WriCmdDat(0,0x90+y);break;
//		case 2:Lcd12864_WriCmdDat(0,0x88+y);break;
//		case 3:Lcd12864_WriCmdDat(0,0x98+y);break;		
//	}
//}

void Lcd12864_WriAdd(u8 x,u8 y)
{
	switch(x)
	{
		case 0:Lcd12864_WriCmdDat(0,0x80+y);break;
		case 1:Lcd12864_WriCmdDat(0,0x90+y);break;
		case 2:Lcd12864_WriCmdDat(0,0x88+y);break;
		case 3:Lcd12864_WriCmdDat(0,0x98+y);break;		
	}
}

void Lcd12864_PriAsc(u8 ascii)
{
	Lcd12864_WriCmdDat(1,ascii);
}

void Lcd12864_PriStr(char *p)
{
	while(*p!='\0')
	{
		Lcd12864_PriAsc(*p);
		p++;
	}
}

/******************************************************************************************

DDRAM 8*16 ?????

Line1 80H 81H 82H 83H 84H 85H 86H 87H

Line2 90H 91H 92H 93H 94H 95H 96H 97H

Line3 88H 89H 8AH 8BH 8CH 8DH 8EH 8FH

Line4 98H 99H 9AH 9BH 9CH 9DH 9EH 9FH

*

******************************************************************************************/


#ifdef ___SetWhite__

void Lcd12864_SetWhite(u8 y,u8 x,u8 endx)
{
	u8 i,j,white_x,white_y,white_endx;
	white_endx = endx-x+1;
	switch(y)
	{
		case 0: 
			white_x = 0x80+x;
			white_y = 0x80;
			break;
		case 1:
			white_x = 0x80+x;
			white_y = 0x90;
			break;
		case 2:
			white_x = 0x88+x;
			white_y = 0x80;
			break;
		case 3:
			white_x = 0x88+x;
			white_y = 0x90;
			break;
	}
	Lcd12864_WriCmdDat(0,0x36);  //开绘图显示
	for(i=0;i<16;i++)
	{
		Lcd12864_WriCmdDat(0,white_y+i);
		Lcd12864_WriCmdDat(0,white_x);
		for(j=0;j<white_endx;j++)
		{
			Lcd12864_WriCmdDat(1,0xff);
			Lcd12864_WriCmdDat(1,0xff);
		}
	}
	Lcd12864_WriCmdDat(0,0x36);  
	Lcd12864_WriCmdDat(0,0x32);  //开GPRAM显示、关基本指令
}

void  Lcd12864_SetWhite_Init()
{
	u8 i,j;
	Lcd12864_WriCmdDat(0,0x36);   //写GDRAM是扩展操作指令
	for(i=0;i<32;i++)
	{
		Lcd12864_WriCmdDat(0,0x80+i);  //先写入水平做坐标值
		Lcd12864_WriCmdDat(0,0x80);  //写入垂直坐标值
		for(j=0;j<16;j++)            //写入两个8位元的数据
		{
			Lcd12864_WriCmdDat(1,0x00);
			Lcd12864_WriCmdDat(1,0x00);
		}
	}
	Lcd12864_WriCmdDat(0,0x36);
	Lcd12864_WriCmdDat(0,0x32);   //写完数据，开图形显示,关扩展指令
 }

void Lcd12864_ClrWhite(u8  y,u8 x,u8 endx)
{
	u8 i,j,white_x,white_y,white_endx;
	white_endx = endx-x+1;
	switch(y)
	{
		case 0: 
			white_x = 0x80+x;
			white_y = 0x80;
			break;
		case 1:
			white_x = 0x80+x;
			white_y = 0x90;
			break;
		case 2:
			white_x = 0x88+x;
			white_y = 0x80;
			break;
		case 3:
			white_x = 0x88+x;
			white_y = 0x90;
			break;
	}
	Lcd12864_WriCmdDat(0,0x36);  //GDRAM--ON EXTEND--ON
	for(i=0;i<16;i++)
	{
		Lcd12864_WriCmdDat(0,white_y+i);
		Lcd12864_WriCmdDat(0,white_x);
		for(j=0;j<white_endx;j++)
		{
			Lcd12864_WriCmdDat(1,0x00);
			delay_us(1);
			Lcd12864_WriCmdDat(1,0x00);
			delay_us(1);
		}
	}
	Lcd12864_WriCmdDat(0,0x36);
	Lcd12864_WriCmdDat(0,0x32);
}

#endif
