#ifndef __GRAPHICAL_H
#define __GRAPHICAL_H

#include "font.h"
#include "my_ico.h"

typedef struct _WAVE_INFO{
  uint8_t  flg;           //更新标志
  int16_t  *buffer;       //只支持浮点数，缓冲长度最好和img_width一致。
  uint16_t buffer_size;   //缓冲大小
  uint16_t buffer_cnt;    //缓冲长度，只用来搜索最大最小值
  int16_t  max_value;     //缓冲中数据最大值
  int16_t  min_value;     //缓冲中数据最小值
  int8_t   wave_type;     //0点波形，1柱状图
  int16_t  minification;  //缩小倍数
  uint16_t img_width;     //生成的图象宽度
  uint16_t img_high;      //生成的图象高度
}WAVE_INFO;


typedef struct _GIF_INFO{
  uint8_t load;
  uint8_t loop;
  uint8_t speed;
  uint32_t tick_msk;
  uint32_t frame_num;
  uint32_t play_no;
  uint16_t scr_pos_x;
  uint16_t scr_pos_y;
  uint16_t img_width;
  uint16_t img_high;
  uint8_t *p;
}GIF_INFO;


typedef struct _PROGRESS_BAR_INFO{
  uint8_t  type;
  uint16_t scr_pos_x;
  uint16_t scr_pos_y;
  uint16_t img_width;
  uint16_t img_high;
  uint16_t value;
  uint16_t max_value;
}PROGRESS_BAR_INFO;


typedef enum _BAT_STATE{
  ICO_BAT_0,
  ICO_BAT_1,
  ICO_BAT_2,
  ICO_BAT_3,
  ICO_BAT_4,
  ICO_BAT_CHARGE,
}BAT_STATE;


typedef enum _GIF_TYPE{
  GIF_ROTATE_LINE_R8,
}GIF_TYPE;

//

#define Glph_DrawPoint  DrawPoint
#define Glph_DrawBitmap DrawBitmap
void Glph_ClrArea(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high);
void Glph_Print(uint8_t x, uint8_t y,uint8_t font_t, char *dt);                                   //字符串打印
void Glph_DrawLine(int16_t x1,int16_t y1, int16_t x2, int16_t y2, uint8_t t );                    //画直线
void Glph_DrawRectangle(int16_t x1,int16_t y1, int16_t x2, int16_t y2, uint8_t fill, uint8_t c ); //画矩形
void Glph_DrawXLine(uint8_t x1, uint8_t y1, uint8_t r, float angle, uint8_t t);                   //极坐标画直线
void Glph_Drawcircle( u16 xc, u16 yc, u16 r, u16 fill, u16 c);                                    //画圆
void Glph_Wave(uint8_t x, uint8_t y, WAVE_INFO *cfg);                                             //画波形
void Glph_DrawBattery(uint8_t x, uint8_t y, uint8_t type, BAT_STATE st);                          //画电池电量图标
void Glph_DrawGif(uint8_t x, uint8_t y, uint8_t type);                                            //画动画图标
//
void Glph_DrawBarInit(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high);
void Glph_DrawBar(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high, uint8_t type, int num);
//
void Glph_LoadGif(uint8_t x, uint8_t y, uint8_t loop, GIF_INFO *gif_play, uint8_t *p);
void Glph_PlayGif(GIF_INFO *gif);

#endif






