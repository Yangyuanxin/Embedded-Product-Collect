/**

  ******************************************************************************
  * @file    displayer.c
  * @author  SZQVC
  * @version V0.2.8
  * @date    2015.2.14
  * @brief   灯塔计划.海啸项目 (QQ：49370295)
  *          SSD1306驱动，分辨率128X32
  ******************************************************************************
  * @attention                                                                 *
  *                                                                            *
  * <h2><center>&copy; COPYRIGHT 2015 SZQVC</center></h2>                      *
  *                                                                            *
  * 文件版权归“深圳权成安视科技有限公司”(简称SZQVC）所有。                      *
  *                                                                            *
  *        http://www.szqvc.com                                                *
  *                                                                            *
  ******************************************************************************
  
**/
#include "sys_includes.h"
#include "math.h"

/* define */

/* public */

/* extern */

/* private */
DISPLAY_TSK_MSG_T DisplayConfig;

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* SSD1306 驱动                                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
typedef enum _OLED_WRITE_TYPE
{
  WRITE_CMD,
  WRITE_DATA,
}OLED_WRITE_TYPE;

  
#define LCM_BUS_DATA()    GPIO_SetBits(LCM_DC, LCM_DC_PIN);
#define LCM_BUS_CMD()     GPIO_ResetBits(LCM_DC, LCM_DC_PIN);

#define LCM_RST_LOW()     GPIO_ResetBits(LCM_RST, LCM_RST_PIN);
#define LCM_RST_HIGH()    GPIO_SetBits(LCM_RST, LCM_RST_PIN);

#define LCM_CS_ENABLE()   GPIO_ResetBits(SPI_CS_LCM, SPI_CS_LCM_PIN);
#define LCM_CS_DISABLE()  GPIO_SetBits(SPI_CS_LCM, SPI_CS_LCM_PIN);

uint8_t reflash_flg;

void oled_byte_write(uint8_t type, uint8_t dt)
{
    if( type==WRITE_CMD ){
        LCM_CS_ENABLE();
        LCM_BUS_CMD();
        SPI_WriteByte( dt );
        LCM_CS_DISABLE();
    }else if( type==WRITE_DATA ){
        LCM_CS_ENABLE();
        LCM_BUS_DATA();
        SPI_WriteByte( dt );
        LCM_CS_DISABLE();
    }
}



void oled_multi_write(uint8_t type, uint8_t *p, uint16_t size)
{
    uint16_t i;
    
    if( type==WRITE_CMD ){  //cmd
        LCM_BUS_CMD();
        LCM_CS_ENABLE();
        for(i=0;i<size;i++){
            SPI_WriteByte( *p++ );
        }
        LCM_CS_DISABLE();
    }else if( type==WRITE_DATA ){
        LCM_BUS_DATA();
        LCM_CS_ENABLE();
        for(i=0;i<size;i++){
            SPI_WriteByte( *p++ );
        }
        LCM_CS_DISABLE();
    }
}

void oled_dma_write(uint16_t lcm_addr, uint8_t *p, uint16_t size)
{
}

uint8_t oled_init_tbl[] = 
{   
//    0xFD,0x12,      /*Command lock*/
    0xAE,           /*display off*/

    0x0f,0x1f,
    0x40,           /*set display start line*/
    0x81,0x80,
    
    0xa1,//0xa1,
    0xa6,
    0xa8,0x1f,
    
    0xc8,//0xc8,
    
    0xd3,0x00,
    
    0xd5,0xA0,    //0xf0 display clocks频率(GPS干扰?)
    0xd8,0x05,
    
    0xd9,0xc2,
    
    0xda,0x12,
    
    0xdb,0x08,
    
    //    
//    0xaf,
    /* Setup column start and end address */
//    0x21,0,127,
    /* Setup Page start and end address */
//    0x22,0,3,  

};


void oled_init(void)
{
    int i,n;
    
    LCM_PWR_OFF();
    LCM_RST_LOW();
    delay_us(100);
    LCM_RST_HIGH();
    delay_us(10);   //3us delay minimum
    //
    oled_multi_write( WRITE_CMD,oled_init_tbl,sizeof(oled_init_tbl) );
    //oled显示频率
    oled_byte_write(WRITE_CMD,0xd5);
    oled_byte_write(WRITE_CMD,iLookCfg.oled_fosc);
    //oled对比度
    oled_byte_write(WRITE_CMD,0x81);
    oled_byte_write(WRITE_CMD,iLookCfg.oled_contrast);
    DisplayConfig.contrast = iLookCfg.oled_contrast;
    //oled显示到装
    if( iLookCfg.flip_onoff==0 )
      oled_byte_write(WRITE_CMD,0xC8);
    else
      oled_byte_write(WRITE_CMD,0xC0);
    //
    for( i=0; i<4; i++ ){
      oled_byte_write(WRITE_CMD,0xb0+i);
      oled_byte_write(WRITE_CMD,0x04);
      oled_byte_write(WRITE_CMD,0x10);
      for( n=0; n<128; n++ ){
        FrameBuffer[n][i]=0x0;
        oled_byte_write(WRITE_DATA,0x00);
      }
    }
    //
    LCM_PWR_ON();
    delay_ms(20);
    //
    oled_byte_write( WRITE_CMD,0xAF );    /* display ON */
}

void oled_power_off(void){
  oled_byte_write(WRITE_CMD,0xAE);
  LCM_PWR_OFF();
}

void oled_power_on(void)
{
  oled_byte_write(WRITE_CMD,0xAF);
  LCM_PWR_ON();
}

/*----------------------------------------------------------------------------*/
const char oled_reflash_cmd_tbl[4]={0x20,0x02};
uint32_t reflash_t_msk;

void display_reflash(char type)
{
  int i,n;

  if( (type==1)
    ||((reflash_flg)&&(TimeOutCheck_ms(reflash_t_msk))) ){

    if( iLookCfg.flip_onoff==0 ){
      for( i=0;i<4;i++ ){
        
        oled_byte_write(WRITE_CMD,0xb0+i);/* b0~b8, page address */
        oled_byte_write(WRITE_CMD,0x04);  /* 00~0f, the lower start column adress,only in page mode */
        oled_byte_write(WRITE_CMD,0x10);  /* 10~1f, the upper start column adress,only  */
        for( n=0; n<128; n++)
          oled_byte_write(WRITE_DATA,FrameBuffer[n][i]);
      }
    }else{
      for( i=0;i<4;i++ ){ 
        oled_byte_write(WRITE_CMD,0xb0+i);/* b0~b8, page address */
        oled_byte_write(WRITE_CMD,0x04);  /* 00~0f, the lower start column adress,only in page mode */
        oled_byte_write(WRITE_CMD,0x10);  /* 10~1f, the upper start column adress,only  */
        for( n=127; n>=0; n--)
          oled_byte_write(WRITE_DATA,FrameBuffer[n][i]);
      }
    }
    reflash_t_msk = GetSysTick_ms()+50;
    reflash_flg = 0;
  }
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* 显示器基础驱动API                                                           */
/*----------------------------------------------------------------------------*/
/* 
   void ClrScreen(void)           清空frame buffer，立即更新显示器
   void DrawPoint(...)                画点
   void DrawBitmap(...)               画位图
   void DrawFill(...)                 清空指定区域(起始坐标+终点坐标)
   void DrawFillSize(...)             清空指定位置的区域(坐标+区域大小)
*/
/*----------------------------------------------------------------------------*/
const uint8_t BIT_MASK[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

__inline void swap_int(uint8_t *a, uint8_t *b)
{
  *a ^= *b;
  *b ^= *a;
  *a ^= *b;
}


void ClrScreen(void)
{
  uint16_t i;
  uint8_t *p = &FrameBuffer[0][0];
  
  for(i=0; i<sizeof(FrameBuffer); i++)
    *p++ = 0x00;
  //
  display_reflash(1);
}

void DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
  uint8_t page_id,tmp;
  
  if( (x>LCM_WIDTH-1)||(y>LCM_HIGH-1) )
    return;

  page_id = y/8;
  tmp = BIT_MASK[y%8];

  if(t)
    FrameBuffer[x][page_id]|=tmp;
  else 
    FrameBuffer[x][page_id]&=~tmp;
  //
  reflash_flg = 1;
}

void DrawBitmap(uint8_t x,uint8_t y, uint8_t type, uint8_t width, uint8_t high, uint8_t *p)
{
  int i,j;
  uint8_t tmp;
  uint8_t on,off;
  uint16_t bit_offset;

  //是否需要反显
  if( type&BIT_MAP_REVERSE ){
    on = 0; off = 0xff;
  }else{
    on = 0xff; off = 0x00;
  }
  //
  if( (type&BIT_MAP_HORI)==BIT_MAP_HORI ){
    for( i=0; i<high; i++){
      j = 0;
      do{
        if( j%8==0 ){
          tmp = *p++;
          bit_offset = 7;
        }
        if( tmp&BIT_MASK[bit_offset] )
          DrawPoint(x+j,y+i,on);
        else
          DrawPoint(x+j,y+i,off);
        j++;
        bit_offset--;
      }while(j<width);
    }
  }else if( (type&BIT_MAP_VERT)==BIT_MAP_VERT ){
    for( j=0; j<width; j++ ){
      i=0;
      do{
        if( i%8==0 ){
          tmp = *p++;
          bit_offset = 0;
        }
        if( tmp&BIT_MASK[bit_offset])
          DrawPoint(x+j,y+i,on);
        else
          DrawPoint(x+j,y+i,off);
        i++;
        bit_offset++;
      }while(i<high);
    }
  }else if( (type&BMP_FILE)==BMP_FILE ){
    for(i=high-1; i>=0; i--){
      j = 0;
      do{
        if( j%8==0 ){
          tmp = *p++;
          bit_offset = 7;
        }
        if( tmp&BIT_MASK[bit_offset] )
          DrawPoint(x+j,y+i,on);
        else
          DrawPoint(x+j,y+i,off);
        j++;
        bit_offset--;
      }while(j<width);

    }
  }
}


void DrawFill(uint8_t x1,uint8_t y1, uint8_t x2, uint8_t y2, u8 dot)
{
  uint8_t x,y;
  
  if( x1>x2 )
    swap_int(&x1,&x2);
  if( y1>y2 )
    swap_int(&y1,&y2);
  
  for( x=x1;x<=x2;x++ ){
    for( y=y1;y<=y2;y++ )
      DrawPoint(x,y,dot);
  }
}

void DrawFillSize(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high, u8 dot)
{
  uint8_t x,y;
  uint8_t x2 = x1+width-1, y2 = y1+high-1;
  
  for( x=x1;x<=x2;x++ ){
    for( y=y1;y<=y2;y++ )
      DrawPoint(x,y,dot);
  }
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* 显示器任务                                                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
TASK_CTRL_INFO DisplayTskInfo;

void DisplayTask_Init(void)
{
    //初始化任务信息
    DisplayTskInfo.Msg = (unsigned char*)&DisplayConfig;
    DisplayTskInfo.MsgFlg = 0;
    DisplayTskInfo.Process = DisplayTask;
    //
    QstCtrl(&DisplayTskInfo,DISPLAY_PWR_OFF);
}

char DisplayTask(void)
{
  char tsk_cmd = QstGetCmd(&DisplayTskInfo);
  //处理命令
  if( tsk_cmd != T_NULL ){
    switch(DisplayTskInfo.Ctrl){
      case DISPLAY_PWR_ON:
        oled_init();
        reflash_flg = 0;
        reflash_t_msk = 0;
        QstEnter(&DisplayTskInfo,DISPLAY_WORKING);
        break;
      //
      case DISPLAY_PWR_OFF:
        if( DisplayTskInfo.State!=DISPLAY_PWR_OFF ){
          oled_power_off();
          QstEnter(&DisplayTskInfo,DISPLAY_PWR_OFF);
        }
        break;
      //
      case DISPLAY_PWR_SAVE:
        if( DisplayTskInfo.State!=DISPLAY_PWR_SAVE ){
          oled_power_off();
          QstEnter(&DisplayTskInfo,DISPLAY_PWR_SAVE);
        }
        reflash_flg = 0;
        reflash_t_msk = 0;
        break;
      //
      case DISPLAY_WAKE_UP:
        if( DisplayTskInfo.State==DISPLAY_PWR_SAVE ){
          oled_power_on();
          QstEnter(&DisplayTskInfo,DISPLAY_WORKING);
        }else if( DisplayTskInfo.State==DISPLAY_PWR_OFF ){
          QstCtrl(&DisplayTskInfo,DISPLAY_PWR_ON);
        }
        reflash_flg = 0;
        reflash_t_msk = 0;
        break;
      //
      case DISPLAY_CONFIG:
        //亮度设定
        oled_byte_write(WRITE_CMD,0x81);
        oled_byte_write(WRITE_CMD,DisplayConfig.contrast);
        break;
    }
  }
  //任务进程
  switch(DisplayTskInfo.State){
    case DISPLAY_WORKING:
      display_reflash(0);
    break;
  }
  
  return 1;
}




