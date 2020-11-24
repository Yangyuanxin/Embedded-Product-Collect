/**

  ******************************************************************************
  * @file    graphical.c
  * @author  SZQVC
  * @version V0.2.8
  * @date    2015.2.14
  * @brief   灯塔计划.海啸项目 (QQ：49370295)
  ******************************************************************************
  * @attention                                                                 *
  *                                                                            *
  * <h2><center>&copy; COPYRIGHT 2015 SZQVC</center></h2>                      *
  *                                                                            *
  * 文件版权归“深圳权成安视科技有限公司”(简称SZQVC）所有。                   *
  *                                                                            *
  *        http://www.szqvc.com                                                *
  *                                                                            *
  ******************************************************************************
  
**/
#include "sys_includes.h"
#include "font.c"       //字库
#include "my_ico.c"     //自定义图标

#define PI  3.14159265358979324
#define TWOPI (PI*2)

/*----------------------------------------------------------------------------*/
void Glph_ClrArea(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high)
{
  DrawFill(x1,y1,x1+width-1,y1+high-1,0);
} 


/*******************************************************************************
* Function Name  : GetASCIICode
* Description    : 读取ASCII字符点阵数据
* Input          : 字体(font_t),字符(a)
* Output         : 
* Return         : 数据起始指针
* Attention      : None
*******************************************************************************/
const uint8_t *GetAsciiBitmap(uint8_t font_t, char a)
{
  char tmp;
  const uint8_t *bitmap_p;
  
  tmp = a;
  
  if( font_t==LED_14X32 ){
    if( (tmp>='0')&&(tmp<='9') ){
      bitmap_p = (uint8_t*)&Bitmap_LED_14X32[tmp-'0'];
    }else{
      switch(tmp){
        case ':':
          bitmap_p = (uint8_t*)&Bitmap_LED_14X32[10];
          break;
        case '+':
          bitmap_p = (uint8_t*)&Bitmap_LED_14X32[11];
          break;
        case '-':
          bitmap_p = (uint8_t*)&Bitmap_LED_14X32[12];
          break;
        default:
          bitmap_p = (uint8_t*)&Bitmap_LED_14X32[8];
          break;
      }
    }
  }else if( font_t==MS_GOTHIC_8X16 ){
    if( (tmp>=' ')&&(tmp<='~') ){  
      bitmap_p = (uint8_t*)&Bitmap_MsGothic_8X16[tmp-32];
    }
  }else if( font_t==LED_8X16 ){
    if( (tmp>='0')&&(tmp<='9') ){
      bitmap_p = (uint8_t*)&Bitmap_LED_8X16[tmp-'0'];
    }else{
      switch( tmp ){
        case ' ': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[10];  break;
        case ':': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[13];  break;
        case '.': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[11];  break;
        case '`': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[12];  break;
        case '/': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[14];  break;
        case '-': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[15];  break;
        case '_': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[16];  break;
        case '^': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[17];  break;
        case 'K': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[18];  break;
        case 'O': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[19];  break;
        case 'G': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[20];  break;
        case '*': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[21];  break;
        case '!': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[22];  break;
        case '%': bitmap_p = (uint8_t*)&Bitmap_LED_8X16[23];  break;
      }
    }
  }else if( font_t==ASCII_5X7 ){
    tmp -= 32;
    if( tmp<sizeof(Bitmap_ASCII_5X7)/5 )
      bitmap_p = (uint8_t*)&Bitmap_ASCII_5X7[tmp];
    else
      bitmap_p = (uint8_t*)&Bitmap_ASCII_5X7['?'-32];
  }else if( font_t==LED_5X7 ){
    tmp -= 32;
    if( tmp<sizeof(Bitmap_LED_5X7)/5 )
      bitmap_p = (uint8_t*)&Bitmap_LED_5X7[tmp];
    else
      bitmap_p = (uint8_t*)&Bitmap_LED_5X7['?'-32];
  }  
  return bitmap_p;
}

/*******************************************************************************
* Function Name  : Glph_Print
* Description    : 字符打印输出
* Input          : 屏幕坐标(x,y), 字体(font_t), 字符串( *dt)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
void Glph_Print(uint8_t x, uint8_t y, uint8_t font_t, char *dt) 
{
  char byte_cnt=0,tmp;
  const uint8_t *p;
  uint8_t font_width,font_high,bitmap_t;
  
  switch(font_t&0x7f){
    case LED_14X32:
      font_width=14;  font_high=32;
      bitmap_t = BIT_MAP_VERT;
      break;
      
    case LED_8X16:
      font_width=8;   font_high=16;
      bitmap_t = BIT_MAP_HORI;
    break;

    case LED_5X7:
      font_width=5;   font_high = 8;
      bitmap_t = BIT_MAP_VERT;
    break;
    
    case MS_GOTHIC_8X16:
      font_width=8;   font_high=16;
      bitmap_t = BIT_MAP_HORI;
    break;
    

    default:
      font_t = ASCII_5X7;
    case ASCII_5X7:
      font_width=5;   font_high=8;
      bitmap_t = BIT_MAP_VERT;
    break;
  }
    
  byte_cnt = 0;
  do{
    tmp = *dt++;
    if( (tmp!=0)&&(tmp!='\r')&&(tmp!='\n')&&(tmp<0x80) ){ 
      p = GetAsciiBitmap( font_t&0x7f,tmp);
      if( font_t&FONT_HIGHLIGHT )
        DrawBitmap(x+byte_cnt*font_width,y,bitmap_t|BIT_MAP_REVERSE,font_width,font_high,(uint8_t*)p);
      else
        DrawBitmap(x+byte_cnt*font_width,y,bitmap_t,font_width,font_high,(uint8_t*)p);
      byte_cnt++;
    }else
      break;
  }while(1);
}


/*******************************************************************************
* Function Name  : Glph_Wave
* Description    : 绘制波形曲线
* Input          : 屏幕坐标(x,y), 波形数据WAVE_INFO(*cfg)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
void Glph_Wave(uint8_t x, uint8_t y, WAVE_INFO *cfg)
{
  uint16_t i;
  int16_t y_value;
  int16_t *p=(int16_t*)cfg->buffer;
  int16_t val_mid,tmp;
  int cnt;
  //
  cfg->max_value=-32768,cfg->min_value=32767;
  if( cfg->buffer_cnt>cfg->buffer_size )
    cnt = cfg->buffer_size;
  else
    cnt = cfg->buffer_cnt;
  
  //寻找最大值,最小值
  if( cnt>1 ){
    for( i=0; i<cnt; i++){
      if( *p>cfg->max_value ) 
        cfg->max_value = *p;
      if( *p<cfg->min_value )
        cfg->min_value = *p;
      p++;
    } 
  }else if(cnt==1) {
    cfg->min_value = *p;
    cfg->max_value = *p;
  }else
     return;

  //Y坐标中心
  cfg->minification = (cfg->max_value-cfg->min_value)/cfg->img_high+1;
  val_mid = (cfg->max_value+cfg->min_value)/2;

  //绘制波形
  DrawFill(x,y,x+cfg->img_width,y+cfg->img_high,0);
  //
  if( cfg->wave_type==0 ){
    i = cnt;
    p = cfg->buffer;
    do{
      tmp = (*p++-val_mid)/cfg->minification;
      //中心坐标偏值
      y_value = cfg->img_high/2-tmp;
      //是否超出坐标
      if( y_value<0 )
        y_value = 0;
      else if( y_value>(cfg->img_high-1) )
        y_value = cfg->img_high-1;
  
      DrawPoint(x+i,y+y_value,1);
      i--;
    }while(i>0);
  }else{
    //柱状图
    i = cnt;
    p = cfg->buffer;
    do{
      y_value = (*p++-cfg->min_value)/cfg->minification;
      //是否超出坐标
      if( y_value<0 )
        y_value = 0;
      else if( y_value>(cfg->img_high-1) )
        y_value = cfg->img_high-1;
      //
      //DrawPoint(x+i,y+y_value,1);
      DrawFill(x+i,y+cfg->img_high-y_value-1,x+i,y+cfg->img_high-1,1);
      i--;
    }while(i>0);
  }
}

/*******************************************************************************
* Function Name  : Glph_DrawBattery
* Description    : 绘制电池电量图标
* Input          : 屏幕坐标(x,y), 电池电量等级(st)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
void Glph_DrawBattery(uint8_t x, uint8_t y, uint8_t type, BAT_STATE st)
{
  uint8_t i = 0;
  if( type==1 ){
    DrawBitmap(x,y,BIT_MAP_VERT,28,16,(uint8_t*)ICO_V_BAT_28X16);
    if( st==ICO_BAT_CHARGE ){
      DrawBitmap(x+8,y+4,BIT_MAP_VERT,10,8,(uint8_t*)ICO_V_BAT_CHG_10X8);
    }else if( (st>=ICO_BAT_1)&&(st<=ICO_BAT_4) ){
      for( i=0; i<=st ;i++ ){
        DrawBitmap((x+5)+(i-1)*4,y+4,BIT_MAP_VERT,4,8,(uint8_t*)ICO_V_BAT_LEVEL_4X8);
      }
    }
  }else{
    if( st==ICO_BAT_CHARGE ){
      DrawBitmap(x,y,BIT_MAP_VERT,16,8,(uint8_t*)ICO_V_CHARGE_16X8);      
    }else if( st<=ICO_BAT_4 ){
      DrawBitmap(x,y,BIT_MAP_VERT,16,8,(uint8_t*)ICO_V_BAT_16X8);
      for( i=0; i<st ;i++ ){
        DrawBitmap((x+2)+i*3,y+3,BIT_MAP_VERT,3,3,(uint8_t*)ICO_V_BAT_LEVEL_3X3);        
      }
    }
  }
}
/*******************************************************************************
* Function Name  : Glph_DrawLine
* Description    : 绘制直线（从起点到终点）
* Input          : 屏幕开始坐标(x1,y1), 屏幕开始坐标(x2,y2)，白点/黑点(t)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
__inline void swap_int(int16_t *a, int16_t *b)
{
  *a ^= *b;
  *b ^= *a;
  *a ^= *b;
}

void Glph_DrawLine(int16_t x1,int16_t y1, int16_t x2, int16_t y2, uint8_t t )
{
  int16_t ix, iy, cx,cy,n2dy,n2dydx,d, 
      dx = abs(x2-x1),
      dy = abs(y2-y1),
      yy=0;
  
  if( dx<dy ){
    yy = 1;
    swap_int(&x1,&y1);
    swap_int(&x2,&y2);
    swap_int(&dx,&dy);
  }
  
  ix = (x2-x1)>0 ? 1:-1;
  iy = (y2-y1)>0 ? 1:-1;
  cx = x1;
  cy = y1;
  n2dy = dy*2;
  n2dydx = (dy-dx)*2;
  d = dy*2 - dx;
  
  //如果直线与X轴的夹角大于45度  
  if( yy ){
    while( cx!=x2 ){
      if(d<0){
        d += n2dy;
      }else{
        cy += iy;
        d += n2dydx;
      }
      DrawPoint(cy, cx, t);
      cx += ix;
    }
  }
  //如果直线与X轴的夹角小于45度
  else{
    while( cx!=x2 ){
      if( d<0 ){
        d += n2dy;
      }else{
        cy += iy;
        d += n2dydx;
      }
      DrawPoint(cx, cy, t);
      cx += ix;
    }
  }
}

/*******************************************************************************
* Function Name  : Glph_DrawXLine
* Description    : 绘制X射线
* Input          : 屏幕开始坐标(x1,y1), 射线长度(r)，射线角度(angle弧度),白点/黑点(t)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
void Glph_DrawXLine(uint8_t x1, uint8_t y1, uint8_t r, float angle, uint8_t t)
{
  int16_t x2,y2;
  float tmp,tt,radian;
  
  radian = angle;
  tmp = cos(radian);
  tt = tmp*r+0.5;
  x2 = x1 + tt;
  tmp = sin(radian);
  tt = tmp*r+0.5;
  y2 = y1 + tt;
  
  Glph_DrawLine(x1,y1,x2,y2,t);
}

/*******************************************************************************
* Function Name  : Glph_Drawcircle
* Description    : 依据圆点和半径画圆
* Input          : 屏幕开始坐标(x1,y1), 半径(r)，是否填充(fill),白点/黑点(c)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
__inline void draw_circle_8( u16 xc, u16 yc, u16 x, u16 y, u16 c)   
 {    
     // 参数 c 为颜色值    
     DrawPoint( xc + x, yc + y, c);    
     DrawPoint( xc - x, yc + y, c);    
     DrawPoint( xc + x, yc - y, c);   
     DrawPoint( xc - x, yc - y, c);    
     DrawPoint( xc + y, yc + x, c);   
     DrawPoint( xc - y, yc + x, c);    
     DrawPoint( xc + y, yc - x, c);    
     DrawPoint( xc - y, yc - x, c);    
 }       
//Bresenham's circle algorithm    
void Glph_Drawcircle( u16 xc, u16 yc, u16 r, u16 fill, u16 c)  
 {    
     // (xc, yc) 为圆心，r 为半径    
     // fill 为是否填充    
     // c 为颜色值       
     // 如果圆在图片可见区域外，直接退出    
   
    int x = 0, y = r, yi, d;    
        d = 3 - 2 * r;

     if(xc + r < 0 || xc - r >= 240 || yc + r < 0 || yc - r >= 320)   
     {  
         return;  
     }     
     if(fill)  
     {    
         // 如果填充（画实心圆）    
         while(x <= y)   
         {    
             for(yi = x; yi <= y; yi ++)    
             {  
                 draw_circle_8(xc, yc, x, yi, c);    
             }  
             if(d < 0)   
             {    
                 d = d + 4 * x + 6;    
             }   
             else   
             {    
                 d = d + 4 * (x - y);    
                 y --;    
             }    
   
             x++;    
         }    
     }   
     else   
     {    
         // 如果不填充（画空心圆）    
         while (x <= y)   
         {    
             draw_circle_8(xc, yc, x, y, c);    
             if(d < 0)   
             {    
                 d = d + 4 * x + 6;    
             }   
             else   
             {    
                 d = d + 4 * (x - y);    
                 y --;    
             }    
             x ++;    
         }    
     }    
 }  

/*******************************************************************************
* Function Name  : Glph_DrawRectangle
* Description    : 画长方型
* Input          : 第一点坐标(x1,y1), 第二点坐标(x2,y2),白点/黑点(c)
* Output         : 
* Return         : None
* Attention      : None
*******************************************************************************/
void Glph_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t fill, uint8_t c )
{
  int i,j;
  uint8_t fill_c;
  
  if( fill>0 ){
    fill_c = c;
  }else{
    if( c>0 )
      fill_c = 0;
    else
      fill_c = 1;
  }
  //
  if(x1>x2){
    swap_int(&x1,&x2);
  }  
  if(y1>y2){
    swap_int(&y1,&y2);
  }
  //
  if( y1==y2 ){
    for( i=x1; i<=x2; i++ )
      DrawPoint(i,y1,c);
  }else{
    for( i=x1; i<=x2; i++ ){
      DrawPoint(i,y1,c);
      DrawPoint(i,y2,c);
    }
  }
  //
  if( x1==x2 ){
    for( i=y1+1; i<y2; i++ ){
      DrawPoint(x1,i,c);
    }
  }else{
    for( i=y1+1; i<y2; i++ ){
      DrawPoint(x1,i,c);
      DrawPoint(x2,i,c);
    }
  }
  //fill
  if( (fill==1)&&(x1!=x2)&&(y1!=y2) ){
    for(i=x1+1;i<x2;i++){
      for(j=y1+1;j<y2;j++){
        DrawPoint(i,j,fill_c);
      }
    }
  }
}
/*******************************************************************************
* Function Name  : Glph_LoadGif
* Description    : 加载GIF
* Input          : 图象区域坐标(x1,y1), 是否循环(loop), GIF图象指针(*p),
* Output         : GIF_INFO *gif_play
* Return         : None
* Attention      : None
*******************************************************************************/
void Glph_LoadGif(uint8_t x, uint8_t y, uint8_t loop, GIF_INFO *gif_play, uint8_t *p)
{
    gif_play->scr_pos_x = (uint16_t)x;
    gif_play->scr_pos_y = (uint16_t)y; 
    gif_play->img_width = (uint8_t)p[0];
    gif_play->img_high = (uint8_t)p[1];
    gif_play->frame_num = (uint8_t)p[2];
    gif_play->speed = (uint8_t)p[3];
    gif_play->play_no = 0;
    gif_play->loop = loop;
    gif_play->p = (uint8_t*)(p+gif_play->img_width);
}

/*******************************************************************************
* Function Name  : Glph_PlayGif
* Description    : 播放GIF
* Input          : gif播放指针
* Output         : None
* Return         : None
* Attention      : 该函数需要不断运行，依据内部的定时器判断换帧（最小间隔每帧/10ms）
*******************************************************************************/
void Glph_PlayGif(GIF_INFO *gif)
{
    uint16_t p_offset;
    
    if( TimeOutCheck_ms(gif->tick_msk+gif->speed*10) ){
      gif->tick_msk = GetSysTick_ms();
      if( ( gif->loop == 1)&&(gif->play_no >= gif->frame_num) ){
          gif->play_no = 0;
      }
      //
      if( gif->play_no<gif->frame_num ){
        p_offset = gif->play_no*(gif->img_width*gif->img_high/8);
        DrawBitmap(gif->scr_pos_x,gif->scr_pos_y,BIT_MAP_VERT,gif->img_width,gif->img_high,(uint8_t*)(gif->p+p_offset)); 
        gif->play_no++;
      }
    }
}


/*******************************************************************************
* Function Name  : Glph_DrawBar
* Description    : 状态条
* Input          : 开始坐标(x1,y1),状态条像素（width,high),状态条类型（=0 左侧开始，=1中心开始），进度数字（与图象宽度有关）
* Output         : None
* Return         : None
* Attention      : 该函数需要不断运行，依据内部的定时器判断换帧（最小间隔每帧/10ms）
*******************************************************************************/
void Glph_DrawBarInit(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high)
{
  Glph_DrawRectangle(x1,y1,x1+width-1,y1+high-1,0,1);
  DrawFill(x1+1,y1+1,x1+width-2,y1+high-2,0);
}

void Glph_DrawBar(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high, uint8_t type, int num)
{
  int x2,y2;
  
  width = width-2;
  high = high-2;
  x1 = x1+1;
  y1 = y1+1;
  DrawFill(x1,y1,x1+width-1,y1+high-1,0);
  //
  if( type==0 ){
    if( num>0 ){
      if( num>width )
      num = width;
      //
      x2 = x1+num;
      y2 = y1+high;
      DrawFill(x1,y1,x2,y2,1);
    }
  }else if( type==1 ){
    if( num>(width-1)/2 )
      num = (width-1)/2;
    else if( num<(-1*(width-1)/2) )
      num = -1*(width-1)/2;
    //
    x1 = x1+(width-1)/2;
    x2 = x1+num;
    y2 = y1+high-1;
    DrawFill(x1,y1,x2,y2,1);
  }else if( type==2 ){
    if( num>high )
      num = high;
    //
    x2 = x1+width;
    y2 = y1+num;
    DrawFill(x1,y1,x2,y2,1);
  }
}

#if 0
void test_glph(void)
{
  int i,j;
  float angle=0.0;
/*  
  do{
    Glph_DrawXLine(15,15,15,angle,1);
    OLED_ReflashTask();
    angle +=30.0;
  }while(angle<360);
  Glph_Drawcircle(32,16,5,0,1);
*/
  Glph_DrawLine(0,0,127,31,1);
  //
  display_reflash(1);
  while(1);
}
#endif


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


