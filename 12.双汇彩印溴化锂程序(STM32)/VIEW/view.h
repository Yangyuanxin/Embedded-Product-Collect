#ifndef _VIEW_H_
#define _VIEW_H_

#ifndef _VIEW_C
#define EXT_VIEW extern 
#else
#define EXT_VIEW
#endif

//#include "config.h"

/******************EDIT:chang***********************/
typedef struct
{
	u16 strlen;			//字符串长度
	u16 *str;			//字符串内容
	u16 NextToken;		//字符串确定时所显示的下一个Token值
}MenuStr_t;

typedef struct
{
	u16 FirstItem;		//所显示的第一个选项
	u16 CurrentItem;		//光带所在的选项
	u16 Page;			//菜单的页
        u8 index;                      //在所有的菜单中的位置
	u16 PageItem;		//菜单现在被选中的项
	u16 TurnMode;		//菜单的翻页方式
	u16 count;			//菜单中菜单项的个数
//	uint16 countofpage;
	u32 *Item;			//菜单项
}Menu_t;


typedef struct
{
	u8 number_page;
	u8 number_menu;
	u8 start_line;
	u8 cursor_top;
	u8 cursor_left;
	u8 turn;
}YC_PAGE;

typedef struct
{
	u8 yx_point;
	u8 disp_way;
	u8 state_pos;		//开关量状态显示的位置
	u8 yx_mean;
}YX_PARA;
typedef struct
{
	u8 delaytime;
	u8 soetype;
	u8 reserve;
	u8 yxmenu;
}CORRECT_YX;
typedef struct
{
	u16 yx_cycle;
	u16 next_page;
}YX_PAGE;
typedef struct
{
	u8 fixed_type;
	u8 fixed_name;
	uint8 fixed_start;
	uint8 fixed_unit;
	uint16 fixed_value_min;
	uint16 fixed_value_max;
}FIXED_PARA_STRU;

typedef struct
{
	uint8 para_type;	
	uint8 fixed_num;
	uint8 *fixed_inf;
}PARA_STRUCT;
typedef struct
{
	uint8 type;
	uint8 sign;
	uint8 dot;
	uint8 position;
	uint16 max;
	uint16 min;
}MODUL_PARA;
typedef struct
{
	uint8 port;
	uint8 baud;
	uint8 check;
	uint8 address;
	uint8 rules;
	uint8 reserve;
}PORT_PARA;
typedef struct
{
	uint16 x;
	uint16 y;
	uint16 length;
	uint16 value;
	uint16 value_min;
	uint16 value_max;
}PORT_SHOW;
typedef struct
{
	u16 y;
	u16 x;
	u8 index;//在对应显示界面中要操作的项
}PARACURSOR;
typedef struct
{
	uint8 yksign;
	uint8 yktype;
	uint8 number;
	uint8 ykobject;
	uint8 yuanfang;
	uint8 serever;
}YKPARA_STR;
typedef struct
{
	uint16 x;
	uint16 y;
	uint8 logic;
	uint8 value;
	uint8 strlength;
	uint8 type;
	uint16 *str;
}CHANGEYK;
typedef struct
{
	uint8 code;
	uint8 serever;
}DEVICE_STR;



typedef struct
{
	u16 turn;
	u16 left;
	u16 top;
	u16 count;
	u16 *data;
}inf_char;

#define NUM_YC                  14


EXT_VIEW u32 reset_counter;//键盘无操作到一定时间，返回到主界面


EXT_VIEW void DispMenu( void );
EXT_VIEW void Yc_Disppage (YC_PAGE* yc_information);


EXT_VIEW void MainView(void);

EXT_VIEW void Adj_xiaoshudian(char *buf,u8 dot_num);


#endif

