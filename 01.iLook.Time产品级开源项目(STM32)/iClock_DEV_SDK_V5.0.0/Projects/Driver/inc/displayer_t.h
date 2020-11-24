/***********************************************************************/
/*                         SZQVC.Lighthouse                            */
/*                           www.szqvc.com                             */
/***********************************************************************/


#ifndef __OLED_SSD1306_H
#define __OLED_SSD1306_H

#define BIT_MAP_REVERSE   0x80    //反显
#define BIT_MAP_HORI      0x01    //字节水平排列
#define BIT_MAP_VERT      0x02    //字节垂直排列
#define BMP_FILE          0x04    //bmp文件图象格式

//
typedef enum _DISPLAY_TASK_STATE
{
    DISPLAY_TASK_NULL     = T_NULL,
    DISPLAY_PWR_ON,
    DISPLAY_WORKING,
    DISPLAY_PWR_SAVE,
    DISPLAY_WAKE_UP,
    DISPLAY_CONFIG,
    //
    DISPLAY_PWR_OFF       = T_PWR_OFF,
    DISPLAY_HW_ERR        = T_HW_ERR,
}DISPLAY_TASK_STATE;


typedef struct _DISPLAY_TSK_MSG_T{
  uint8_t state;
  int8_t  contrast;
  uint8_t flip_onoff;       //=0 正常, =1 反转
}DISPLAY_TSK_MSG_T;


//显示驱动函数
void display_reflash(char type);
void ClrScreen(void);
void DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void DrawFill(uint8_t x1,uint8_t y1, uint8_t x2, uint8_t y2, u8 dot);
void DrawFillSize(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high, u8 dot);
void DrawBitmap(uint8_t x,uint8_t y, uint8_t type, uint8_t width, uint8_t high, uint8_t *p);


//显示任务
void DisplayTask_Init(void);
char DisplayTask(void);


#endif

/*********************** (C) COPYRIGHT SZQVC **************************/
/*                          END OF FILE                               */
/**********************************************************************/


