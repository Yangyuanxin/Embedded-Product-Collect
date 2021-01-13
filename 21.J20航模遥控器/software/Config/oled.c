//////////////////////////////////////////////////////////////////////////////////
//  ¹¦ÄÜÃèÊö   : 0.69´çOLED ½Ó¿ÚÑÝÊ¾Àý³Ì(STM32F103C8T6 IIC)
//              ËµÃ÷:
//              ----------------------------------------------------------------
//              GND   µçÔ´µØ
//              VCC   ½Ó3.3vµçÔ´
//              SCL   ½ÓPB8£¨SCL£©
//              SDA   ½ÓPB9£¨SDA£©
//////////////////////////////////////////////////////////////////////////////////£

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"
//OLEDµÄÏÔ´æ
//´æ·Å¸ñÊ½ÈçÏÂ.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

u8 OLED_GRAM[128][8];//¶¨ÒåÊý×é£¬ÏÈÔ¤´æ´¢ÔÚramÖÐ£¬ÔÙÒ»ÆðÐ´½øOLED
/**********************************************
//IIC Start
**********************************************/
void IIC_Start(void)
{

    OLED_SCLK_Set() ;
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop(void)
{
    OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
    OLED_SDIN_Clr();
    OLED_SDIN_Set();

}

void IIC_Wait_Ack(void)
{
    OLED_SCLK_Set() ;
    OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m, da;
    da = IIC_Byte;
    OLED_SCLK_Clr();

    for(i = 0; i < 8; i++)
    {
        m = da;
        //	OLED_SCLK_Clr();
        m = m & 0x80;

        if(m == 0x80)
        {
            OLED_SDIN_Set();
        }
        else OLED_SDIN_Clr();

        da = da << 1;
        OLED_SCLK_Set();
        OLED_SCLK_Clr();
    }
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);			//write command
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Command);
    IIC_Wait_Ack();
    IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x40);			//write data
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();
    IIC_Stop();
}
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
    if(cmd)
    {
        Write_IIC_Data(dat);
    }
    else
    {
        Write_IIC_Command(dat);
    }
}

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
    unsigned char m, n;

    for(m = 0; m < 8; m++)
    {
        OLED_WR_Byte(0xb0 + m, 0);		//page0-page1
        OLED_WR_Byte(0x00, 0);		//low column start address
        OLED_WR_Byte(0x10, 0);		//high column start address

        for(n = 0; n < 128; n++)
        {
            OLED_WR_Byte(fill_Data, 1);
        }
    }
}

//×ø±êÉèÖÃ
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
//¿ªÆôOLEDÏÔÊ¾
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDCÃüÁî
    OLED_WR_Byte(0X14, OLED_CMD); //DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); //DISPLAY ON
}
//¹Ø±ÕOLEDÏÔÊ¾
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDCÃüÁî
    OLED_WR_Byte(0X10, OLED_CMD); //DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); //DISPLAY OFF
}
//ÇåÆÁº¯Êý,ÇåÍêÆÁ,Õû¸öÆÁÄ»ÊÇºÚÉ«µÄ,ºÍÃ»µãÁÁÒ»Ñù
void OLED_Clear(void)
{
    u8 i, n;

    for(i = 0; i < 8; i++)
    {
        OLED_WR_Byte (0xb0 + i, OLED_CMD); //ÉèÖÃÒ³µØÖ·£¨0~7£©
        OLED_WR_Byte (0x00, OLED_CMD);     //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁÐµÍµØÖ·
        OLED_WR_Byte (0x10, OLED_CMD);     //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁÐ¸ßµØÖ·

        for(n = 0; n < 128; n++)OLED_WR_Byte(0, OLED_DATA);
    } //¸üÐÂÏÔÊ¾
}
//ÖØÖÃÊý×éOLED_GRAM£¬Ê¹È«²¿Îª0
void OLED_Clear_GRAM(void)
{
    u8 x, y;

    for(x = 0; x < 128; x++)
    {
        for(y = 0; y < 8; y++)
        {
            OLED_GRAM[x][y] = 0;
        }
    }
}

//¸üÐÂÏÔ´æµ½OLED
void OLED_Refresh_Gram(void)
{
    u8 i, n;

    for(i = 0; i < 8; i++)
    {
        OLED_WR_Byte (0xb0 + i, OLED_CMD); //ÉèÖÃÒ³µØÖ·£¨0~7£©
        OLED_WR_Byte (0x00, OLED_CMD);     //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁÐµÍµØÖ·
        OLED_WR_Byte (0x10, OLED_CMD);     //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁÐ¸ßµØÖ·

        for(n = 0; n < 128; n++)OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}

//»­µã
//x:0~127
//y:0~63
//t:1 Ìî³ä 0,Çå¿Õ
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;

    if(x > 127 || y > 63)return; //³¬³ö·¶Î§ÁË.

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);

    if(t)OLED_GRAM[x][pos] |= temp;
    else OLED_GRAM[x][pos] &= ~temp;
}
//»­¼ÓºÅ
//x:1~126£¬ÖÐÐÄ×ø±êµÄx
//y:1~62£¬ÖÐÐÄ×ø±êµÄy
void OLED_DrawPlusSign(u8 x, u8 y)
{
    OLED_DrawPoint(x, y - 1, 1);
    OLED_DrawPoint(x, y, 1);
    OLED_DrawPoint(x, y + 1, 1);
    OLED_DrawPoint(x - 1, y, 1);
    OLED_DrawPoint(x + 1, y, 1);
}
//x1,y1,x2,y2 Ìî³äÇøÓòµÄ¶Ô½Ç×ø±ê
//È·±£x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,Çå¿Õ;1,Ìî³ä
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)
{
    u8 x, y;

    for(x = x1; x <= x2; x++)
    {
        for(y = y1; y <= y2; y++)OLED_DrawPoint(x, y, dot);
    }
}

//ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û,°üÀ¨²¿·Ö×Ö·û
//x:0~127
//y:0~63
//mode:0,·´°×ÏÔÊ¾;1,Õý³£ÏÔÊ¾
//size:Ñ¡Ôñ×ÖÌå 12/16/24
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//µÃµ½×ÖÌåÒ»¸ö×Ö·û¶ÔÓ¦µãÕó¼¯ËùÕ¼µÄ×Ö½ÚÊý
    chr = chr - ' '; //µÃµ½Æ«ÒÆºóµÄÖµ

    for(t = 0; t < csize; t++)
    {
        if(size == 12)temp = asc2_1206[chr][t]; 	 	//µ÷ÓÃ1206×ÖÌå
        else if(size == 16)temp = asc2_1608[chr][t];	//µ÷ÓÃ1608×ÖÌå
        else if(size == 24)temp = asc2_2412[chr][t];	//µ÷ÓÃ2412×ÖÌå
        else return;								//Ã»ÓÐµÄ×Ö¿â

        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}
//m^nº¯Êý
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}
//ÏÔÊ¾2¸öÊý×Ö
//x,y :Æðµã×ø±ê
//len :Êý×ÖµÄÎ»Êý
//size:×ÖÌå´óÐ¡12/16/24
//mode:Ä£Ê½	0,·´°×Ä£Ê½;1,Õý³£Ä£Ê½
//num:ÊýÖµ(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                OLED_ShowChar(x + (size / 2)*t, y, ' ', size, mode);
                continue;
            }
            else enshow = 1;

        }

        OLED_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode);
    }
}
//ÏÔÊ¾×Ö·û´®
//x,y:Æðµã×ø±ê
//size:×ÖÌå´óÐ¡12/16/24
//*p:×Ö·û´®ÆðÊ¼µØÖ·
//mode:0,·´°×ÏÔÊ¾;1,Õý³£ÏÔÊ¾
void OLED_ShowString(u8 x, u8 y, u8 *p, u8 size, u8 mode)
{
    while((*p <= '~') && (*p >= ' ')) //ÅÐ¶ÏÊÇ²»ÊÇ·Ç·¨×Ö·û!
    {
        if(x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }

        if(y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }

        OLED_ShowChar(x, y, *p, size, mode);
        x += size / 2;
        p++;
    }
}


//ÔÚÖ¸¶¨Î»ÖÃ£¬ÏÔÊ¾Ò»¸ösize*size´óÐ¡µÄºº×Ö
//x:0~127
//y:0~63
//index:ºº×Ö±àºÅ£¨ÔÚ×Ö¿âÊý×éÀïÃæµÄ±àºÅ£©
//size:Ñ¡Ôñ×ÖÌå 12/16/24
//mode:0,·´°×ÏÔÊ¾;1,Õý³£ÏÔÊ¾

void OLED_ShowChinese(u16 x, u16 y, u8 index, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u16 y0 = y;
    u8 *dzk;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * size; //Ò»¸ö24*24µÄºº×Ö72×Ö½Ú

    if(size == 12) dzk = (u8*)Chinese_1212[index];      //µÃµ½ºº×Ö±àºÅ¶ÔÓ¦µÄµãÕó¿â
    else if(size == 16)	dzk = (u8*)Chinese_1616[index]; //µÃµ½ºº×Ö±àºÅ¶ÔÓ¦µÄµãÕó¿â
    else dzk = (u8*)Chinese_2424[index]; //µÃµ½ºº×Ö±àºÅ¶ÔÓ¦µÄµãÕó¿â

    for(t = 0; t < csize; t++)
    {
        temp = dzk[t];                              //µÃµ½µãÕóÊý¾Ý

        for(t1 = 0; t1 < 8; t1++)                  //°´ÕÕ´Ó¸ßÎ»µ½µÍÎ»µÄË³Ðò»­µã
        {
            if(temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if((y - y0) == size)                    //¶Ôy×ø±êµÄ´¦Àí£¬µ±y×ø±ê¾àÆðÊ¼×ø±ê²î24¸öÏñËØµã£¬x×ø±ê¼Ó1
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//ÔÚÖ¸¶¨Î»ÖÃ£¬ÏÔÊ¾Ò»¸ö16*16´óÐ¡µÄºº×Ö
//x:0~127
//y:0~63
//hzIndex[]:ºº×Ö±àºÅ£¨ÔÚ×Ö¿âÊý×éÀïÃæµÄ±àºÅ£©
//len;ºº×Ö¸öÊý
//mode:0,·´°×ÏÔÊ¾;1,Õý³£ÏÔÊ¾
void OLED_ShowChineseWords(u16 x, u16 y, u8 hzIndex[], u8 len, u8 mode)
{
    u8 n;

    for(n = 0; n < len; n++)
    {
        OLED_ShowChinese(x + n * 16, y, hzIndex[n], 16, mode);
    }
}

//ÔÚÖ¸¶¨Î»ÖÃ£¬ÒÔ»­µãµÄ·½Ê½ÏÔÊ¾Í¼Æ¬
//x,y:ÆðÊ¼µã×ø±êxµÄ·¶Î§0~127£¬yÎªÒ³µÄ·¶Î§0~64
//BMP£ºÍ¼Æ¬Êý×é
//length£ºÍ¼Æ¬µÄÏñËØ³¤0-128
//height£ºÍ¼Æ¬µÄÏñËØ¸ß0-64
//mode:0,·´°×ÏÔÊ¾;1,Õý³£ÏÔÊ¾
void OLED_DrawPointBMP(u8 x, u8 y, unsigned char BMP[], u8 length, u8 height, u8 mode)
{
    u16 temp, t, t1;
    u8 y0 = y;

    for(t = 0; t < length * height / 8; t++)
    {
        temp = BMP[t];

        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if((y - y0) == height)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/***********¹¦ÄÜÃèÊö£ºÏÔÊ¾ÏÔÊ¾BMPÍ¼Æ¬128¡Á64ÆðÊ¼µã×ø±ê(x,y),xµÄ·¶Î§0¡«127£¬yÎªÒ³µÄ·¶Î§0¡«7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if(y1 % 8 == 0) y = y1 / 8;
    else y = y1 / 8 + 1;

    for(y = y0; y < y1; y++)
    {
        OLED_Set_Pos(x0, y);

        for(x = x0; x < x1; x++)
        {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

//³õÊ¼»¯SSD1306
void OLED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	 //Ê¹ÄÜB¶Ë¿ÚºÍAFIO¸´ÓÃ¹¦ÄÜÄ£¿éÊ±ÖÓ

    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);//IIC1ÖØÓ³Éä -> PB8,9

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ËÙ¶È50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  //³õÊ¼»¯GPIOB8,9
    GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);


    delay_ms(10);
    OLED_WR_Byte(0xAE, OLED_CMD); //--ÏÔÊ¾¹Ø±Õ
    OLED_WR_Byte(0x00, OLED_CMD); //---ÉèÖÃ×îÐ¡ÁÐµØÖ·
    OLED_WR_Byte(0x10, OLED_CMD); //---ÉèÖÃ×î´óÁÐµØÖ·
    OLED_WR_Byte(0x40, OLED_CMD); //--set start line address
    OLED_WR_Byte(0xB0, OLED_CMD); //--set page address
    OLED_WR_Byte(0x81, OLED_CMD); // contract control
    OLED_WR_Byte(0xFF, OLED_CMD); //--128
    OLED_WR_Byte(0xA1, OLED_CMD); //set segment remap
    OLED_WR_Byte(0xA6, OLED_CMD); //--normal / reverse
    OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F, OLED_CMD); //--1/32 duty
    OLED_WR_Byte(0xC0, OLED_CMD); //ComÉ¨Ãè·½Ïò£¬ÈôÏÔÊ¾µÄÊÇ¾µ¶Ô³Æ£¬¸ÄÎªC8
    OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset
    OLED_WR_Byte(0x00, OLED_CMD); //

    OLED_WR_Byte(0xD5, OLED_CMD); //set osc division
    OLED_WR_Byte(0x80, OLED_CMD); //

    OLED_WR_Byte(0xD8, OLED_CMD); //set area color mode off
    OLED_WR_Byte(0x05, OLED_CMD); //

    OLED_WR_Byte(0xD9, OLED_CMD); //Set Pre-Charge Period
    OLED_WR_Byte(0xF1, OLED_CMD); //

    OLED_WR_Byte(0xDA, OLED_CMD); //set com pin configuartion
    OLED_WR_Byte(0x12, OLED_CMD); //

    OLED_WR_Byte(0xDB, OLED_CMD); //set Vcomh
    OLED_WR_Byte(0x30, OLED_CMD); //

    OLED_WR_Byte(0x8D, OLED_CMD); //set charge pump enable
    OLED_WR_Byte(0x14, OLED_CMD); //

    OLED_WR_Byte(0xAF, OLED_CMD); //--turn on oled panel
}


