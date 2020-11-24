/******************************************************************************
 * Ks0108.C - Ks0108 液晶控制器驱动程序
 *
 *
 * DESCRIPTION: -
 *
 * modification history
 * --------------------
 * 01a, 04/sep/2007, 梁炎昌 written
 * --------------------
 ******************************************************************************/

#include "CPU.H"
#include "Ks0108.H"
#include "Chinese.C"
#include "ASCII.c"



//------------------------------------------------------------------------------------------

/*
-------------------------------------------------------------------------
GUI相关全局变量
-------------------------------------------------------------------------
*/
U8  GUIEnFont = En_8x16;                //英文字体
U8  GUITextMode = GUI_TEXTMODE_NORMAL;  //显示模式

U8 Char_XSIZE;   //英文字体X宽度
U8 Char_YSIZE;   //英文字体Y宽度

U8 PosX = 0;     //当前系统X坐标
U8 PosY = 0;     //当前系统Y坐标

U8 NoXY = true;//是否使用PosX PosY的标志   1-->不使用   0-->使用


// ---- 延迟程序 ----------------------------------------------------
#if OSC_48MHZ == true

#define SomeNop();      {               \
    _nop_(); _nop_(); _nop_(); _nop_(); \
    _nop_(); _nop_(); _nop_(); _nop_(); \
   }

#elif OSC_24MHZ == true

#define SomeNop();      {               \
    _nop_(); _nop_(); _nop_(); _nop_(); \
    }

#endif






/*
*****************************************************************************
* WriteCommand_0108 - 写命令到KS0108
* DESCRIPTION: -
* 本函数不接管片选信号
* @Param CmdData:命令码
* @Return :
* ----
*****************************************************************************
*/
void WriteCommand_0108(U8 CmdData)
{
    LCD_E = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_Bus = CmdData;
    SomeNop();
    LCD_E = 1;
    SomeNop();
    LCD_E = 0;

}


/*
*****************************************************************************
* WriteData_0108 - 写数据到KS0108
* DESCRIPTION: -
* 本函数不接管片选信号
* @Param Data:写入的数据
* @Return :
* ----
*****************************************************************************
*/
void WriteData_0108(U8 Data)
{
    LCD_E = 0;
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_Bus = Data;
    SomeNop();
    LCD_E = 1;
    SomeNop();
    LCD_E = 0;

}
/*-------------------读LCD数据----------------------------*/
#if 0
U8 ReadData_0108(void)
{   U8 Temp;
    LCD_Bus = 0xFF;//切换成输入
    LCD_RS = 1;
    LCD_RW = 1;
    SomeNop();
    LCD_E = 1;
    SomeNop();
    Temp = LCD_Bus;
    return(Temp);
/*
    U8 Temp;
    PIN_KS0108_Di_HIGH;
    PIN_KS0108_RW_HIGH;
    DelayS(0);
    PIN_KS0108_Enable_HIGH;
    DelayS(0);
    PIN_KS0108_Enable_LOW;
    Temp = PIN_KS0108_Lcd_InBus;
    return (Temp);
*/
}

U8 ReadState_0108(void)
{
    U8 Temp;
    LCD_Bus = 0xFF;
    LCD_RS = 0;
    LCD_RW = 1;
    SomeNop();
    LCD_E = 1;
    SomeNop();
    LCD_E = 0;
    Temp = LCD_Bus;
    return (Temp);


}

U8 Busy(void)
{
    U16 TimeOut;
    U8 Flag;
    TimeOut = 200;
    do{
        Flag = ReadState_0108();
        LED1 ^= 0x01;
        if (TimeOut-- == 0){
            return 0;
        }
    }while(Flag &0x80);
    return 1;
}
#endif
//Option = 1   --->Disp On
//Option = 0   --->Disp Off
void KS0108_DisplayOn(U8 Option)
{
    if (Option == 1) {
        WriteCommand_0108(Ks0108_CMD_Disp_ON);
    }else{
        WriteCommand_0108(Ks0108_CMD_Disp_OFF);
    }
}

void KS0108_SetStartRow(U8 Row)
{
    WriteCommand_0108(Ks0108_CMD_Disp_Start_Line + Row);
}
/*
*****************************************************************************
* Display_Locate - LCD上指定位置 显示指定数据
* DESCRIPTION: -
*
* @Param x0:0--191横向坐标
* @Param y0:0--7  纵向坐标
* @Param DisplayData:写入的数据
* @Return :
* ----
*****************************************************************************
*/
void Display_Locate(U8 DisplayData, U8 x0, U8 y0)
{
    if ((x0 <= (LCD_XSIZE-1))&(y0 <= (LCD_YSIZE-1))){
        if (x0 <= 63){                                //0--191
            LCD_CSA();
//            Busy();//判忙
            WriteCommand_0108(Ks0108_CMD_Set_Addr + x0);
        } else if (x0 <= 127){
            LCD_CSB();
//            Busy();//判忙
            WriteCommand_0108(Ks0108_CMD_Set_Addr + x0 - 64);
        } else if (x0 <= 191){
            LCD_CSC();
//            Busy();//判忙
            WriteCommand_0108(Ks0108_CMD_Set_Addr + x0 -128);
        }
//        Busy();//判忙
        WriteCommand_0108(y0 + Ks0108_CMD_Set_Page);   //0--7
//        Busy();//判忙
        WriteData_0108(DisplayData);
        LCD_CS_None();//不再片选任何一个Ks0108
    }
}


/*
*****************************************************************************
* Display_Clear - 清空LCD显示
* DESCRIPTION: -
*
* @Param void:
* @Return :
* ----
*****************************************************************************
*/
void Display_Clear(void)
{
    U8 j,k;
//--------------------------------------
    LCD_CSA();
    for(k = 0; k < 8; k++){
        WriteCommand_0108(0 + Ks0108_CMD_Set_Addr); //0--7
        WriteCommand_0108(k + Ks0108_CMD_Set_Page); //0--63
        for(j = 0;j < 64;j++){
            WriteData_0108(0x00);
        }
    }
    KS0108_SetStartRow(0);
//--------------------------------------
    LCD_CSB();
    for(k = 0; k < 8; k++){
        WriteCommand_0108(0 + Ks0108_CMD_Set_Addr); //0--7
        WriteCommand_0108(k + Ks0108_CMD_Set_Page); //0--63
        for(j = 0;j < 64;j++){
            WriteData_0108(0x00);
        }
    }
    KS0108_SetStartRow(0);
//-------------------------------------
    LCD_CSC();
    for(k = 0; k < 8; k++){
        WriteCommand_0108(0 + Ks0108_CMD_Set_Addr);//0--7
        WriteCommand_0108(k + Ks0108_CMD_Set_Page);//0-63
        for(j = 0;j < 64;j++){
            WriteData_0108(0x00);
        }
    }
    KS0108_SetStartRow(0);
//-------------------------------------
    LCD_CS_None();
}


/*
*****************************************************************************
* Display_Init - 初始化LCD
* DESCRIPTION: -
*
* @Param void:
* @Return :
* ----
*****************************************************************************
*/
void Display_Init(void)
{
//---------------------------
    LCD_CSA();
    KS0108_DisplayOn(0);
    KS0108_SetStartRow(0);
    KS0108_DisplayOn(1);
//---------------------------
    LCD_CSB();
    KS0108_DisplayOn(0);
    KS0108_SetStartRow(0);
    KS0108_DisplayOn(1);
//---------------------------
    LCD_CSC();
    KS0108_DisplayOn(0);
    KS0108_SetStartRow(0);
    KS0108_DisplayOn(1);
//---------------------------
    LCD_CS_None();
}




/*
*****************************************************************************
* Display_Chinese - 指定位置显示一个16x16的汉字
* DESCRIPTION: -
*
* @Param x0:X轴坐标0--(192-16)
* @Param y0:Y轴坐标0--(8-2)
* @Param UniCode:汉字内码
* @Return :
* ----
*****************************************************************************
*/
void Display_Chinese(U16 UniCode, U8 x0, U8 y0)
{
  U16 CodeID;
  U8 i, ImgData;
  U8 HZ_X = x0;
  // 在字库里搜索汉字
    for(CodeID=0; CodeID < hzNum; CodeID++){
        // 找到 指定汉字的 Index 后, 跳出循环
        if ( (hzIndex[2*CodeID] == UniCode/256) && (hzIndex[2*CodeID+1] == UniCode%256) ){
            break;
        }else if(CodeID == (hzNum-1)){//未检索到的汉字,显示个"##"提示吧
            GUI_DispCharAt('#',x0,y0);
            GUI_DispCharAt('#',x0+Char_XSIZE,y0);
            return;
        }
    }
//----------------------------------
    switch (GUITextMode) {
        case GUI_TEXTMODE_NORMAL :
//-------------------------------------------------------------------------
            // 写汉字的上半部分  (一个汉字 32 个字节,  显示为2行 * 16列)
            for(i=0; i<16; i++){
                ImgData = hzdot[i+CodeID*32];
                Display_Locate(ImgData, HZ_X, y0);
                HZ_X++;
            }
            // 写汉字的下半部分
            HZ_X = x0;
            for(i=16; i<32; i++){
                ImgData = hzdot[i+CodeID*32];
                Display_Locate(ImgData, HZ_X, y0 + 1);
                HZ_X++;
            }
//-------------------------------------------------------------------------
             break;
        case GUI_TEXTMODE_REVERSE :
//-------------------------------------------------------------------------
            // 写汉字的上半部分  (一个汉字 32 个字节,  显示为2行 * 16列)
            for(i=0; i<16; i++){
              ImgData = hzdot[i+CodeID*32]^0xFF;
              Display_Locate(ImgData, HZ_X, y0);
              HZ_X++;
            }
            // 写汉字的下半部分
            HZ_X = x0;
            for(i=16; i<32; i++){
              ImgData = hzdot[i+CodeID*32]^0xFF;
              Display_Locate(ImgData, HZ_X, y0 + 1);
              HZ_X++;
            }
//-------------------------------------------------------------------------
             break;
        case GUI_TEXTMODE_UNDERLINE :
//-------------------------------------------------------------------------
            // 写汉字的上半部分  (一个汉字 32 个字节,  显示为2行 * 16列)
            for(i=0; i<16; i++){
              ImgData = hzdot[i+CodeID*32];
              Display_Locate(ImgData, HZ_X, y0);
              HZ_X++;
            }
            // 写汉字的下半部分
            HZ_X = x0;
            for(i=16; i<32; i++){
              ImgData = hzdot[i+CodeID*32]|0x80;
              Display_Locate(ImgData, HZ_X, y0 + 1);
              HZ_X++;
            }
//-------------------------------------------------------------------------
             break;
        default :
             break;
    }
}


/*
*****************************************************************************
* Display_ASCII - 显示一个ASCII
* DESCRIPTION: -
*
* @Param X:X轴坐标 0--192  (注意边界)
* @Param Y:Y轴坐标 0-- 7   (注意边界)
* @Param Char:ASCII码
* @Return :
* ----
*****************************************************************************
*/
void Display_ASCII(U8 Char, U8 X, U8 Y)
{
    U8 i, ImgData;
    U8 HZ_X = X;
    if(GUIEnFont == En_5x8){
        Display_Locate(0x00, HZ_X, Y);
        HZ_X++;
        for(i = 0; i < 5; i++){
            ImgData = chardot_5x8[i+(Char-0x20)*5];
            Display_Locate(ImgData, HZ_X, Y);
            HZ_X++;
        }
        Display_Locate(0x00, HZ_X, Y);
        HZ_X++;
        Display_Locate(0x00, HZ_X, Y);
    } else {
        for(i=0;i<8;i++){
            ImgData = chardot_8x16[i+(Char-0x20)*16];
            Display_Locate(ImgData, HZ_X, Y);
            HZ_X++;
        }
        HZ_X=X;
        for(i=8;i<16;i++){
            ImgData = chardot_8x16[i+(Char-0x20)*16];
            Display_Locate(ImgData, HZ_X, Y + 1);
            HZ_X++;
      }

   }
}




/*
*****************************************************************************
* Display_InvASCII - 反显示ASCII
* DESCRIPTION: -
*
* @Param X:X轴坐标  0--191
* @Param Y:Y轴坐标  0--7
* @Param Char:ASCII码
* @Return :
* ----
*****************************************************************************
*/
void Display_InvASCII(U8 Char, U8 X, U8 Y)
{

    U8 i, ImgData;
    U8 HZ_X = X;
    if(GUIEnFont == En_5x8){
        Display_Locate(0xFF, HZ_X, Y);
        HZ_X++;
        for(i = 0; i < 5; i++){
            ImgData = chardot_5x8[i+(Char-0x20)*5]^0xFF;
            Display_Locate(ImgData, HZ_X, Y);
            HZ_X++;
        }
        Display_Locate(0xFF, HZ_X, Y);
        HZ_X++;
        Display_Locate(0xFF, HZ_X, Y);
    } else {
        for(i=0;i<8;i++){
            ImgData = chardot_8x16[i+(Char-0x20)*16] ^ 0xFF;
            Display_Locate(ImgData, HZ_X, Y);
            HZ_X++;
        }
        HZ_X=X;
        for(i=8;i<16;i++){
            ImgData = chardot_8x16[i+(Char-0x20)*16] ^ 0xFF;
            Display_Locate(ImgData, HZ_X, Y + 1);
            HZ_X++;
        }
    }
}

/*
*****************************************************************************
* Display_ASCII_UnderLine - 显示一个带有下划线的ASCII
* DESCRIPTION: -
*
* @Param X:X轴坐标
* @Param Y:Y轴坐标
* @Param Char:ASCII码
* @Return :
* ----
*****************************************************************************
*/
void Display_ASCII_UnderLine(U8 Char, U8 X, U8 Y)
{
  //U16 Position;
  U8 i, ImgData;
  U8 HZ_X = X;
    if(GUIEnFont == En_5x8){
        Display_Locate(0x80, HZ_X, Y);
        HZ_X++;
        for(i = 0; i < 5; i++){
          ImgData = chardot_5x8[i+(Char-0x20)*5] | 0x80;
          Display_Locate(ImgData, HZ_X, Y);
          HZ_X++;
        }
        Display_Locate(0x80, HZ_X, Y);
        HZ_X++;
        Display_Locate(0x80, HZ_X, Y);
    } else {
        for(i=0;i<8;i++){
            ImgData = chardot_8x16[i+(Char-0x20)*16];
            Display_Locate(ImgData, HZ_X, Y);
            HZ_X++;
        }
        HZ_X=X;
        for(i=8;i<16;i++){
            ImgData = chardot_8x16[i+(Char-0x20)*16] | 0x80;
            Display_Locate(ImgData, HZ_X, Y + 1);
            HZ_X++;
        }
    }
}

/*
----------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------
*/
#if 0
/*
----------------------------------------------------------------------------------------------
//移动当前Y坐标
----------------------------------------------------------------------------------------------
*/
void GUI_GotoY(U8 y)
{
    if (y > 7) {
        PosY = 7;
    } else {
        PosY = y;
    }
}
/*
----------------------------------------------------------------------------------------------
//移动当前Y坐标
----------------------------------------------------------------------------------------------
*/
void GUI_GotoX(U8 x)
{
    if (x > 127) {
        PosX = 127;
    } else {
        PosX = x;
    }
}
/*
*****************************************************************************
* GUI_GotoXY - 系统光标移动到指定位置
* DESCRIPTION: -
*
* @Param y:Y轴坐标
* @Param x:X轴坐标
* @Return :
* ----
*****************************************************************************
*/
void GUI_GotoXY(U8 x,U8 y)
{
    GUI_GotoX(x);
    GUI_GotoY(y);
}

#endif
/*
*****************************************************************************
* GUI_SetTextMode - 设置文字显示模式
* DESCRIPTION: -
*
* @Param TextMode:模式代码
* @Return :模式代码
* ----
*****************************************************************************
*/

U8 GUI_SetTextMode(U8 TextMode)
{
    GUITextMode = TextMode;
    return GUITextMode;
}

/*
*****************************************************************************
* GUI_SetEnFont - 设置显示的英文字体
* DESCRIPTION: -
* 目前支持两种字体8x16 5x8
* @Param EnFont:字体代码
* @Return :
* ----
*****************************************************************************
*/
void GUI_SetEnFont(U8 EnFont)
{
  if (EnFont == En_5x8){
    GUIEnFont = En_5x8;
    Char_XSIZE = En_5x8_XSIZE;
    Char_YSIZE = En_5x8_YSIZE;
  } else {
    GUIEnFont = En_8x16;
    Char_XSIZE = En_8x16_XSIZE;
    Char_YSIZE = En_8x16_YSIZE;

  }

}
/*
----------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------
*/
void GUI_Delay(U16 Period)
{
    U8 j;
    for (;  Period>0 ; Period--) {
        for (j = 0; j < 200; j++) {
        }
    }
}
/*
*****************************************************************************
* GUI_DispCharAt - 指定位置显示一个ASCII码
* DESCRIPTION: - 字体使用是系统设定的字体
*
* @Param x:X轴坐标
* @Param y:Y轴坐标
* @Param c:ASCII码
* @Return :
* ----
*****************************************************************************
*/
void GUI_DispCharAt(U8 c, U8 x, U8 y)
{
    switch (GUITextMode) {
        case GUI_TEXTMODE_NORMAL :
             Display_ASCII(c, x, y);
             break;
        case GUI_TEXTMODE_REVERSE :
             Display_InvASCII(c, x, y);
             break;
        case GUI_TEXTMODE_UNDERLINE :
             Display_ASCII_UnderLine(c, x, y);
             break;
        default :
             break;
    }
}
/*
----------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------
*/
#if 0
void GUI_DispChar(U8 c)
{
    GUI_DispCharAt(c, PosX, PosY);
    PosX += 8;
}
#endif
/*
*****************************************************************************
* GUI_Init - 初始化GUI显示
* DESCRIPTION: -
*
* @Param void:
* @Return :
* ----
*****************************************************************************
*/
void GUI_Init(void)
{
    Display_Init();
    GUIEnFont = En_8x16;
    GUITextMode = GUI_TEXTMODE_NORMAL;
    Char_XSIZE = En_8x16_XSIZE;
    Char_YSIZE = En_8x16_YSIZE;
    PosX = 0;
    PosY = 0;
    NoXY = true;


}
void GUI_Clear(void)
{
    Display_Clear();
}
#if 0
void GUI_DispCEOL(void)
{
    U8 i;
    for(i = PosX; i <= LCD_XSIZE-8; i += 8) {
        GUI_DispCharAt(' ', i, PosY);

    }
}
#endif
/*
*****************************************************************************
* GUI_DispStringAt - 指定位置显示一个字符串
* DESCRIPTION: -
* 支持中英混合 当中英混合时英文字体强制为8x16 如果单单英文 按照设定字体
* @Param x0:X轴坐标
* @Param y0:Y轴坐标
* @Param s:字符串
* @Return :
* ----
*****************************************************************************
*/
//void GUI_DispStringAt(U8 _CONST_ *s,U8 x0,U8 y0)
void GUI_DispStringAt(U8 *s,U8 x0,U8 y0)
{
    U8 i;
    if (NoXY == false){//没有提供坐标,则使用系统坐标
        x0 = PosX;
        y0 = PosY;
    }
    i = 0;
    while(s[i]!='\0'){
        if (s[i] < 0x80) {//是ASCII
             if(s[i] == '\n'){
                    x0 = 0;
                    y0 += Char_YSIZE;
             } else {
                if (x0 > (LCD_XSIZE-1)) {//换行
                    x0 = 0;
                    y0 += Char_YSIZE;
                }
                switch (GUITextMode) {
                    case GUI_TEXTMODE_NORMAL :
                         Display_ASCII(s[i], x0, y0);
                         break;
                    case GUI_TEXTMODE_REVERSE :
                         Display_InvASCII(s[i], x0, y0);
                         break;
                    case GUI_TEXTMODE_UNDERLINE :
                         Display_ASCII_UnderLine(s[i], x0, y0);
                         break;
                    default :
                         break;
                }
                x0 += Char_XSIZE;
             }
        } else {//是中文
            if (x0 > LCD_XSIZE-16){
                x0 = 0;
                y0 += Chinese_YSIZE;
            }
            Display_Chinese(s[i]*256+s[i+1],x0,y0);//中文
            i++;
            x0 += Chinese_XSIZE;
       }
       s++;
    }
    if (NoXY == false){
        PosX = x0;
        PosY = y0;
    }

}
#if 0
/*
*****************************************************************************
* GUI_DispString - 在当前系统坐标下显示字符串
* DESCRIPTION: -
* 除了坐标不能指定外 功能跟GUI_DispStringAt一致
* @Param s:字符串
* @Return :
* ----
*****************************************************************************
*/
void GUI_DispString(U8 _CONST_ *s)
{
    NoXY = false;
    GUI_DispStringAt(s,PosX,PosY);
    NoXY = true;
}

void GUI_DispStringLen(U8 _CONST_ *s, U16 Len)
{
    U8 i;
    i = 0;
    while(s[i]!='\0'){
        if (i >= Len ){
            return;
        }
        if (s[i] < 0x80) {//是ASCII
             if(s[i] == '\n'){
//                    PosX = 0;
//                    PosY += 2;
             } else {
               if (PosX > (LCD_XSIZE-1)) {//换行
                    PosX = 0;
//                    PosY += 2;
                }
                switch (GUITextMode) {
                    case GUI_TEXTMODE_NORMAL :
                         Display_ASCII(s[i], PosX, PosY);
                         break;
                    case GUI_TEXTMODE_REVERSE :
                         Display_InvASCII(s[i], PosX, PosY);
                         break;
                    case GUI_TEXTMODE_UNDERLINE :
                         Display_ASCII_UnderLine(s[i], PosX, PosY);
                         break;
                    default :
                         break;
                }
                PosX += 8;
             }
        } else {//是中文
            if (PosX > LCD_XSIZE-16){
//                PosX = 0;
//                PosY += 2;
                return;
            }
            Display_Chinese(s[i]*256+s[i+1],PosX,PosY);//中文
            i++;
            PosX += 16;
       }
       i++;
    }

}

void GUI_DrawCursor(U8 x, U8 y)
{
    if(GUITextMode == GUI_TEXTMODE_NORMAL){
        Display_Locate(0xFF,x,y);
        Display_Locate(0xFF,x,y+1);
    } else if (GUITextMode == GUI_TEXTMODE_REVERSE){
        Display_Locate(0x00,x,y);
        Display_Locate(0x00,x,y+1);
    } else {
        Display_Locate(0xFF,x,y);
        Display_Locate(0xFF,x,y+1);
    }
}

void GUI_CursorOn(U8 x, U8 y)
{
    Display_Locate(0xFF,x,y);
    Display_Locate(0xFF,x,y+1);
}

void GUI_CursorOFF(U8 x, U8 y)
{
    Display_Locate(0x00,x,y);
    Display_Locate(0x00,x,y+1);
}
#endif

/*
*****************************************************************************
* GUI_DispStringAtBar - 显示一个字符串在一个BAR中
* DESCRIPTION: -
* 可以设定对齐方式
* @Param s:字符串
* @Param x0:BAR起始X轴坐标
* @Param x1:BAR结束点的X轴坐标
* @Param y0:BAR的Y轴坐标
* @Param Mode:对齐方式 居中于BAR 左对齐BAR 右对齐BAR
* @Return :
* ----
*****************************************************************************
*/
//void GUI_DispStringAtBar(U8 _CONST_ *s,U8 x0, U8 y0, U8 x1,U8 Mode)
void GUI_DispStringAtBar(U8 *s,U8 x0, U8 y0, U8 x1,U8 Mode)
{
    U8 ENCount;//英文统计
    U8 CNCount;//中文统计
    U8 XLen;
    U8 X;
    U8 Y;
    U8 i;
    CNCount = 0;
    ENCount = 0;
    XLen = 0;
    i = 0;
    while(s[i]!='\0'){//统计中英文个数
        if (s[i] > 0x80) {
            CNCount ++;
            i++;
        } else {
           ENCount++;
        }
        i++;
    }
        GUI_SetEnFont(En_8x16);
/*
    if(CNCount){
        GUI_SetEnFont(En_8x16);
    } else {
        GUI_SetEnFont(En_5x8);
    }
*/
    XLen = ENCount*8 + CNCount*16;

    if (XLen > 127) {//我们只能显示一行
        XLen = 127;
    }
//识别对其方式属性
    switch (Mode&0x30) {//只有这两个Bit是这个属性
        case GUI_TA_LEFT: //左对齐
            XLen = 0;
            break;
        case GUI_TA_HCENTER:  //居中
            XLen  = ((x1-x0)-XLen)/2;
            break;
        case GUI_TA_RIGHT:   //右对齐
            XLen  = (x1-x0)-XLen;
            break;
        default:
            break;
    }
//--------------------------------------
//得到喀什显示的起点坐标
    X = x0+XLen;
    Y = y0;

//---------------------
//清除字符前面的空白部分
    if(Mode&GUI_JUST_TEXT){
    } else {
        x0 = X;
        if (GUITextMode == GUI_TEXTMODE_REVERSE){
            for(i = 0; i <x0; i++){
                Display_Locate(0xFF, i,y0);
                Display_Locate(0xFF, i,y0+1);
            }
        }else{
            for(i = 0; i <x0; i++){
                Display_Locate(0x00, i,y0);
                Display_Locate(0x00, i,y0+1);
            }
        }
    }

//-----------------------
    i = 0;
    while(s[i]!='\0'){
        if (s[i] < 0x80) {//是ASCII
            switch (GUITextMode) {
                case GUI_TEXTMODE_NORMAL :
                     Display_ASCII(s[i], X, Y);
                     break;
                case GUI_TEXTMODE_REVERSE :
                     Display_InvASCII(s[i], X, Y);
                     break;
                case GUI_TEXTMODE_UNDERLINE :
                     Display_ASCII_UnderLine(s[i], X, Y);
                     break;
                default :
                     break;
            }
            X += 8;
        } else {//是中文
            Display_Chinese(s[i]*256+s[i+1],X,Y);//中文
            i++;
            X += 16;
       }
//       s++;
        i++;
    }
//清除字符后面的空白部分
    if(Mode&GUI_JUST_TEXT){
        GUI_SetEnFont(En_5x8);
        return;
    } else {
        x0 = X;
        if (GUITextMode == GUI_TEXTMODE_REVERSE){
            for(i = 0; i <x1-x0; i++){
                Display_Locate(0xFF, x0+i,y0);
                Display_Locate(0xFF, x0+i,y0+1);
            }
        }else{
            for(i = 0; i <x1-x0; i++){
                Display_Locate(0x00, x0+i,y0);
                Display_Locate(0x00, x0+i,y0+1);
            }
        }
        GUI_SetEnFont(En_5x8);
        return;
    }
//---------------------------------------
}

#if 0
//--------------------------
//显示ASCII(16X32)
//Char 字符
//X 0--6
//Y 0--(128-8)

U16 _CONST_ Changedot[] =
{
0x0000, 0x0003, 0x000c, 0x000f, 0x0030, 0x0033, 0x003c, 0x003f, 0x00c0, 0x00c3,
0x00cc, 0x00cf, 0x00f0, 0x00f3, 0x00fc, 0x00ff, 0x0300, 0x0303, 0x030c, 0x030f,
0x0330, 0x0333, 0x033c, 0x033f, 0x03c0, 0x03c3, 0x03cc, 0x03cf, 0x03f0, 0x03f3,
0x03fc, 0x03ff, 0x0c00, 0x0c03, 0x0c0c, 0x0c0f, 0x0c30, 0x0c33, 0x0c3c, 0x0c3f,
0x0cc0, 0x0cc3, 0x0ccc, 0x0ccf, 0x0cf0, 0x0cf3, 0x0cfc, 0x0cff, 0x0f00, 0x0f03,
0x0f0c, 0x0f0f, 0x0f30, 0x0f33, 0x0f3c, 0x0f3f, 0x0fc0, 0x0fc3, 0x0fcc, 0x0fcf,
0x0ff0, 0x0ff3, 0x0ffc, 0x0fff, 0x3000, 0x3003, 0x300c, 0x300f, 0x3030, 0x3033,
0x303c, 0x303f, 0x30c0, 0x30c3, 0x30cc, 0x30cf, 0x30f0, 0x30f3, 0x30fc, 0x30ff,
0x3300, 0x3303, 0x330c, 0x330f, 0x3330, 0x3333, 0x333c, 0x333f, 0x33c0, 0x33c3,
0x33cc, 0x33cf, 0x33f0, 0x33f3, 0x33fc, 0x33ff, 0x3c00, 0x3c03, 0x3c0c, 0x3c0f,
0x3c30, 0x3c33, 0x3c3c, 0x3c3f, 0x3cc0, 0x3cc3, 0x3ccc, 0x3ccf, 0x3cf0, 0x3cf3,
0x3cfc, 0x3cff, 0x3f00, 0x3f03, 0x3f0c, 0x3f0f, 0x3f30, 0x3f33, 0x3f3c, 0x3f3f,
0x3fc0, 0x3fc3, 0x3fcc, 0x3fcf, 0x3ff0, 0x3ff3, 0x3ffc, 0x3fff, 0xc000, 0xc003,
0xc00c, 0xc00f, 0xc030, 0xc033, 0xc03c, 0xc03f, 0xc0c0, 0xc0c3, 0xc0cc, 0xc0cf,
0xc0f0, 0xc0f3, 0xc0fc, 0xc0ff, 0xc300, 0xc303, 0xc30c, 0xc30f, 0xc330, 0xc333,
0xc33c, 0xc33f, 0xc3c0, 0xc3c3, 0xc3cc, 0xc3cf, 0xc3f0, 0xc3f3, 0xc3fc, 0xc3ff,
0xcc00, 0xcc03, 0xcc0c, 0xcc0f, 0xcc30, 0xcc33, 0xcc3c, 0xcc3f, 0xccc0, 0xccc3,
0xcccc, 0xcccf, 0xccf0, 0xccf3, 0xccfc, 0xccff, 0xcf00, 0xcf03, 0xcf0c, 0xcf0f,
0xcf30, 0xcf33, 0xcf3c, 0xcf3f, 0xcfc0, 0xcfc3, 0xcfcc, 0xcfcf, 0xcff0, 0xcff3,
0xcffc, 0xcfff, 0xf000, 0xf003, 0xf00c, 0xf00f, 0xf030, 0xf033, 0xf03c, 0xf03f,
0xf0c0, 0xf0c3, 0xf0cc, 0xf0cf, 0xf0f0, 0xf0f3, 0xf0fc, 0xf0ff, 0xf300, 0xf303,
0xf30c, 0xf30f, 0xf330, 0xf333, 0xf33c, 0xf33f, 0xf3c0, 0xf3c3, 0xf3cc, 0xf3cf,
0xf3f0, 0xf3f3, 0xf3fc, 0xf3ff, 0xfc00, 0xfc03, 0xfc0c, 0xfc0f, 0xfc30, 0xfc33,
0xfc3c, 0xfc3f, 0xfcc0, 0xfcc3, 0xfccc, 0xfccf, 0xfcf0, 0xfcf3, 0xfcfc, 0xfcff,
0xff00, 0xff03, 0xff0c, 0xff0f, 0xff30, 0xff33, 0xff3c, 0xff3f, 0xffc0, 0xffc3,
0xffcc, 0xffcf, 0xfff0, 0xfff3, 0xfffc, 0xffff,
};
bit EnFont = 0;
void GUI_DispChar(U8 Char,U8 X,U8 Y,bit Nor)
{
    U8 i;
    U8 dot;

    U16 temp;

    if(Y>(128-8)){//换行
        X += 2;
        Y  = 0;
    }
    if(!Nor){
        for(i=0;i<8;i++){
            if(EnFont == 0){
                WriteByte(X,(Y+i),chardot[i+(Char-0x20)*16]);
            }else{
                dot = chardot[i+(Char-0x20)*16];
                temp = Changedot[dot];
//这段代码把字体纵向放大2倍,外部调用时换行需要加倍
                WriteByte(X,(Y+i),(U8)(temp&0x00FF));
                WriteByte(X+1,(Y+i),(U8)((temp&0xFF00)>>8));

/*
//这段代码把字体放大4倍,但由于横向轴被放大,因此外部调用函数时也需要加倍
                WriteByte(X,(Y+i*2),(U8)(temp&0x00FF));
                WriteByte(X,(Y+i*2+1),(U8)(temp&0x00FF));
                WriteByte(X+1,(Y+i*2),(U8)((temp&0xFF00)>>8));
                WriteByte(X+1,(Y+i*2+1),(U8)((temp&0xFF00)>>8));
*/
            }

        }
        for(i=8;i<16;i++){
            if(EnFont == 0){
                WriteByte(X+1,(Y+i-8),chardot[i+(Char-0x20)*16]);
            }else{
                dot = chardot[i+(Char-0x20)*16];
                temp = Changedot[dot];

                WriteByte(X+2,(Y+i-8),(U8)(temp&0x00FF));
                WriteByte(X+3,(Y+i-8),(U8)((temp&0xFF00)>>8));

/*
                WriteByte(X+2,(Y+(i-8)*2),(U8)(temp&0x00FF));
                WriteByte(X+2,(Y+(i-8)*2+1),(U8)(temp&0x00FF));
                WriteByte(X+3,(Y+(i-8)*2),(U8)((temp&0xFF00)>>8));
                WriteByte(X+3,(Y+(i-8)*2+1),(U8)((temp&0xFF00)>>8));
*/
            }
        }
    }else{
        for(i=0;i<8;i++){
            if(EnFont == 0){
                WriteByte(X,(Y+i),0xFF-chardot[i+(Char-0x20)*16]);
            }else{
                dot = 0xFF-chardot[i+(Char-0x20)*16];
                temp = Changedot[dot];
                WriteByte(X*2,(Y+i),(U8)(temp&0x00FF));
                WriteByte(X*2,(Y+i+1),(U8)(temp&0x00FF));
                WriteByte(X*2+1,(Y+i),(U8)((temp&0xFF00)>>8));
                WriteByte(X*2+1,(Y+i+1),(U8)((temp&0xFF00)>>8));

            }
        }
        for(i=8;i<16;i++){
            if(EnFont == 0){
                WriteByte(X+1,(Y+i-8),0xFF-chardot[i+(Char-0x20)*16]);
            }else{
                dot = 0xFF-chardot[i+(Char-0x20)*16];
                temp = Changedot[dot];
                WriteByte((X+1)*2,(Y+i-8+1),(U8)(temp&0x00FF));
                WriteByte((X+1)*2,(Y+i-8),(U8)(temp&0x00FF));
                WriteByte((X+1)*2+1,(Y+i-8+1),(U8)((temp&0xFF00)>>8));
                WriteByte((X+1)*2+1,(Y+i-8),(U8)((temp&0xFF00)>>8));
            }
        }
    }
}
#endif
//--------------------------

#if 0
//-------------显示普通图片
//------(图像 长度 起始位置)
void Display_Image(U8 _CONST_ *IMG)
{
    U8 XAddr;
    U8 YAddr;
    U16 Count;
    Count = 0;
    for(XAddr=0;XAddr<8;XAddr++){//0--7
        for(YAddr=0;YAddr<128;YAddr++){//0--127
            WriteByte(XAddr,YAddr,IMG[Count++]);
        }
    }
}
#endif

/*
*****************************************************************************
* GUI_DrawIcon - 制定位置显示一个32x32的图标
* DESCRIPTION: -
*
* @Param x0:X轴坐标
* @Param y0:Y轴坐标 注意边界!
* @Param Ico:图标数据
* @Return :
* ----
*****************************************************************************
*/
void GUI_DrawIcon(U8 _CONST_ *Ico,U8 x0,U8 y0)
{
    U8 i;
    U16 Count;
    Count = 0;
    if( y0 > 4){//边界保护
        y0 = 4;
    }
    if(x0 >(LCD_XSIZE-32)){
        x0 = LCD_XSIZE-32;
    }
    for(i=0;i<32;i++){//第一行
        Display_Locate(Ico[Count++],x0+i,y0);
    }
    for(i=0;i<32;i++){//第二行
        Display_Locate(Ico[Count++],x0+i,y0+1);
    }
    for(i=0;i<32;i++){//第三行
        Display_Locate(Ico[Count++],x0+i,y0+2);
    }
    for(i=0;i<32;i++){//第四行
        Display_Locate(Ico[Count++],x0+i,y0+3);
    }
}
#if 0

/*
*****************************************************************************
* GUI_DispDecAt - 显示十进制数值
* DESCRIPTION: -
* 处理长度最多5个数字(因为U16--->65536)
* @Param v:显示的数据
* @Param x:X轴坐标
* @Param y:Y轴坐标  XY 均是起点位置坐标 也就是数值最高位的坐标
* @Param Len:指定的显示长度1--5内
* @Return :
*
*****************************************************************************
*/
void GUI_DispDecAt(U16 v, U16 x, U16 y, U8 Len)
{
    U8 i;
    U8 CharBuf[5];
    U8 HighByte;

    HighByte = 0;
    for(i = 0; i < 5; i++){
        CharBuf[i] = (U8)(v%10);
        v = v/10;
        if(CharBuf[i]){
            HighByte = i;
        }
    }
    //第0位无论如何也显示
    i = 0;
    GUI_DispCharAt(CharBuf[i]+'0',x+((Len-1)-i)*Char_XSIZE,y);
    for(i = 1; i < Len; i++){
        if(CharBuf[i]){
            GUI_DispCharAt(CharBuf[i]+'0',x+((Len-1)-i)*Char_XSIZE,y);
        }else if(i > HighByte){
            GUI_DispCharAt(' ',x+((Len-1)-i)*Char_XSIZE,y);
        }
    }

}

/*
*****************************************************************************
* GUI_DispHexAt - 显示一个数据的十六进制值
* DESCRIPTION: -
* 最大长度4个
* @Param v:数据
* @Param x:X轴坐标
* @Param y:Y轴坐标  XY均是起点坐标 也就是数据最高字节坐标
* @Param Len:长度1--4
* @Return :
*
*****************************************************************************
*/
void GUI_DispHexAt(U32 v, U8 x, U8 y, U8 Len)
{
    U8 i;
    U8 HexData;
    if(Len > 8){//限制范围
        Len = 8;
    }
    for(i = 0; i < Len; i++){
        HexData = v&0x0F;
        v = v >>4;
        if(HexData < 0x0A){
            GUI_DispCharAt(HexData+'0',x+Char_XSIZE*(Len-1-i),y);
        }else{
            GUI_DispCharAt(HexData-0x0A+'A',x+Char_XSIZE*(Len-1-i),y);
        }
    }
}
/*
*****************************************************************************
* HBar - 显示一个水平的进度条
* DESCRIPTION: -
* 附加有百分比显示
* @Param x0:进度条起点X轴坐标   0-->127
* @Param x1:进度条结束点X坐标   0-->127  必须大于x0  百分比显示于该坐标之后
* @Param y:进度条Y轴坐标        0--7
* @Param percent:当前百分值     0-->100
* @Return :
*
*****************************************************************************
*/
void HBar(U8 y, U8 x0, U8 x1,U8 percent)
{
    U8 U8Temp;
    U8 i;
    float Center;
    Center = (x1-x0);
    Center *= percent;
    Center /= 100;
//    U8Temp = (x1-x0)*percent/100;//这个计算做法在430上能用，但C51下似乎必须用浮点算
    U8Temp = (U8)Center;
    Display_Locate(0xFF, x0, y);
    Display_Locate(0xFF, x1, y);
    for(i = 1; i < U8Temp; i++){
        Display_Locate(0xBD, x0+i, y);
    }
    for(i = x0+U8Temp+1; i < x1; i++){
        Display_Locate(0x81, i, y);
    }
}
/*                  x1 +3
|-------------------|
|                   ||
|                   |||
|                   ||||
|                   ||||
--------------------
 -------------------
  ------------------
x0--->x1+3
y0--->y1


*/

void TipDisp( U8 x0, U8 y0, U8 x1, U8 y1)
{
    U8 i;
    for(i = 0; i < x1-x0+4; i++){
        Display_Locate(0x01, x0+i, y0);
        Display_Locate(0x0F, x0+i, y1);
    }
    Display_Locate(0x01, x0+0, y1);
    Display_Locate(0x01, x0+1, y1);
    Display_Locate(0x03, x0+2, y1);
    Display_Locate(0x03, x0+3, y1);
    Display_Locate(0x07, x0+4, y1);
    Display_Locate(0x07, x0+5, y1);

    for(i = 0; i < y1-y0; i++){
        Display_Locate(0xFF, x0, y0+i);
        Display_Locate(0xFF, x1, y0+i);
        Display_Locate(0xFF, x1+1, y0+i);
        Display_Locate(0xFF, x1+2, y0+i);
        Display_Locate(0xFF, x1+3, y0+i);
    }
    Display_Locate(0xFC, x1+1, y0);
    Display_Locate(0xF0, x1+2, y0);
    Display_Locate(0xC0, x1+3, y0);
}

/*
清空Tip
坐标应该跟TipDisp一样
*/
void TipClr( U8 x0, U8 y0, U8 x1, U8 y1)
{
    U8 i;
    U8 j;
    for(i = 0; i <= x1+3-x0; i++){
        for(j = 0; j <= y1-y0; j++){
            Display_Locate(0x00, x0+i, y0+j);
        }
    }

}
#endif
/*
// ---- 显示不带符号的整数  (数字 起始位置XY,显示长度) -----------------------------
void Display_Number(U16 Number, U8 X, U8 Y, U8 Lenth)
{
  U8 DispNum;

  X = ( X + Lenth * 8 - 8 );
  for(; Lenth>0; Lenth--)
  {
    DispNum = Number%10 + 0x30;
    Display_ASCII(DispNum, X, Y);
    X -= 8;
    Number = Number / 10;
  }
}

// ---- 显示带符号的整数  (数字 起始位置XY,显示长度) ---------------------------------
void Display_SignedNumber(int Number,U8 X,U16 Y,U8 Lenth)
{
  if(Number < 0)
  {
    Display_ASCII('-', X, Y);
    Display_Number(-Number, X+8, Y, Lenth);
  }
  else
  {
    Display_ASCII(' ', X, Y);
    Display_Number(Number, X+8, Y, Lenth);
  }
}

// ---- 显示不带符号的小数 (数字 起始位置XY,整数位数,小数位数) ------------------------------
void Display_Decimal(unsigned long int Number, char X, U16 Y, U8 INT, U8 DEC)
{
  U8 DispNum, Lenth;
  //Y = Y +(( X + INT * 8 + DEC * 8 ) / 84) * 2;
  X = ( X + ( INT + DEC ) *8);
  // 显示小数部分
  for(Lenth=DEC; Lenth>0; Lenth--)
  {
    DispNum = Number%10 + 0x30;
    Display_ASCII(DispNum, X, Y);
    //if (X < 8) {Y -= 2; X += 84;}
    X -= 8;
    Number = Number / 10;
  }
  // 显示小数点
  Display_ASCII('.', X, Y);
  //if (X < 8) {Y -= 2; X += 84;}
    X -= 8;
  // 显示整数部分
  for(Lenth=INT; Lenth>0; Lenth--)
  {
    DispNum = Number%10 + 0x30;
    Display_ASCII(DispNum, X, Y);
    //if (X < 8) {Y -= 2; X += 84;}
    X -= 8;
    Number = Number / 10;
  }
}

// ---- 显示带符号的小数 (数字 起始位置XY,整数位数,小数位数) ------------------------------
void Display_SignedDecimal(long int Number, char X, U16 Y, U8 INT, U8 DEC)
{
  if(Number < 0)
  {
    Display_ASCII('-', X, Y);
    Display_Decimal(-Number, X+8, Y, INT, DEC);
  }
  else
  {
  Display_ASCII(' ',X,Y);
  Display_Decimal(Number, X+8, Y, INT, DEC);
  }
}
*/


//--------------
/*
Bar的算法
    ___
 |   |
 |   |
| |<-|-----BarLen
 |   L
 |   |
 |   |
 |  _|_
Bar的滑动距离是L-BarLen
为了美观,可以在开始和结尾部分多流出来一些点,那么滑动距离要扣除这些点的长度,并在计算结果
得到0的时候,添加上上端要留出来的点BarRemainDot
2种显示方式:
一种是BarLen是定长的,
一种BarLen是根据显示总共的项数定下来的
*/


//--------------
//Bar的长度
//预留出来的点
#define BarRemainDot  3
//数字显示位置
//#define BarNumPosX (128-8+2)
#define BarNumPosY (7)
//Bar的显示开始/结束位置
//#define BarBeginPosX (126)
#define BarBeginPosY (0*8)
#define BarEndPosX (126)
#define BarEndPosY (6*8)

U8 _CONST_ BarCode0[]={0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00};
U8 _CONST_ BarCode1[]={0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};
extern U8 ItemBackup_i;
extern U8 ItemBackup[];

void Bar(U8 Item_,U8 ItemNum_,U8 BarPosX,U8 BarNumPosX)
{
    U8 U8_temp;
    U8 DispFlag;
    U8 YOffset;
    U16 Temp;
    U8 BarLen;
    U8 Y;
    U8 i;
//    U8 CharBuf[5];
//    Bool HighBit;

    BarLen = (BarEndPosY-BarBeginPosY-BarRemainDot)/(ItemNum_);//BarLen根据ItemNum_得到
    if (BarLen == 0) {
        BarLen = 5;
    }
    BarLen = 8;
    Temp = Item_*(BarEndPosY-BarBeginPosY-BarLen-BarRemainDot);//BarRemainDot是被扣除的部分
    Temp = Temp/(ItemNum_-1);
    YOffset = (U8)Temp;
    if(!Temp){//顶端，把预留的加上
        YOffset = BarRemainDot;
    }

    for(Y = 0;Y < BarEndPosY/8;Y++){
        if((Y != (YOffset/8))&&(Y != (YOffset/8+1))){
            Display_Locate(0x00,BarPosX,Y);//清除 X=125 列
            Display_Locate(0xFF,BarPosX+1,Y);//X=126列画线
            Display_Locate(0x00,BarPosX+2,Y);//清除 X=127 列
        }else{//Y = YOffset/8 Y = YOffset/8＋1
            Display_Locate(BarCode0[YOffset%8],BarPosX,(YOffset/8));
            Display_Locate(0xFF-BarCode0[YOffset%8],BarPosX+1,(YOffset/8));
            Display_Locate(BarCode0[YOffset%8],BarPosX+2,(YOffset/8));
            if((YOffset/8)+1 < (BarEndPosY/8)){//防止下越界
                Display_Locate(BarCode1[YOffset%8],BarPosX,(YOffset/8+1));
                Display_Locate(0xFF-BarCode1[YOffset%8],BarPosX+1,(YOffset/8+1));
                Display_Locate(BarCode1[YOffset%8],BarPosX+2,(YOffset/8+1));
            }
        }
    }

    GUI_SetEnFont(En_5x8);
    Item_ += 1;
    //显示Bar数字
/*
    for(i = 0; i < 5; i++){
        CharBuf[i] = (U8)(Item%10);
        Item = Item/10;
    }
    HighBit = false;
    for(i = 0; i < 5; i++){
        if(CharBuf[4-i]){//从最高位开始显示
            GUI_DispCharAt(CharBuf[4-i]+'0',x+8*i,y);
            HighBit = true;
        }else{
            if(HighBit == true){
                GUI_DispCharAt('0',x+Char_XSIZE*i,y);//如果高位不为0,当前值为0 ,显示0
            }
        }
    }
*/
    DispFlag = false;
    U8_temp = (U8)(Item_/100);//  百位
    if(U8_temp){
        GUI_DispCharAt(U8_temp+'0',BarNumPosX-12,BarNumPosY);
        DispFlag = true;//通知低位显示
    }else{
        GUI_DispCharAt(' ',BarNumPosX-12,BarNumPosY);
    }

    Item_ = (Item_-U8_temp*100);//剔除百位
    U8_temp = (U8)(Item_/10);//  十位
    if(U8_temp||(DispFlag == true)){//本位不为0，或者高位已经显示，那么必须显示
        GUI_DispCharAt(U8_temp+'0',BarNumPosX-6,BarNumPosY);
        DispFlag = 1;
    }else{
        GUI_DispCharAt(' ',BarNumPosX-6,BarNumPosY);
    }

    U8_temp = (U8)(Item_%10);//  个位
    GUI_DispCharAt(U8_temp+'0',BarNumPosX,BarNumPosY);

//----------------------------------
//显示历史索引号
    if(ItemBackup_i > 1){//大于1才是
        for(i = 0; i <ItemBackup_i-1;i++){//最后一个位于1的位置
            Item_ = ItemBackup[ItemBackup_i-1-i]+1;  //从备份数据中得到标号，然后加1显示
            U8_temp = (U8)(Item_%10);//
            GUI_DispCharAt(U8_temp+'0',BarNumPosX-8*(i+1),BarNumPosY);
            Display_Locate(0x10, BarNumPosX-8*(i+1)+8-1, BarNumPosY); //描分隔符
            Display_Locate(0x10, BarNumPosX-8*(i+1)+8-0, BarNumPosY);
        }
    }
//----------------------------------
    GUI_SetEnFont(En_8x16);

   return;

}


