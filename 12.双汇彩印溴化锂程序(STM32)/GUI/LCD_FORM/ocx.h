#ifndef _OCX_H
#define _OCX_H

#define K_TEXT  0x00
#define K_MENU  0x01
#define K_TIME  0x02
#define K_DATE  0x03
#define K_CAP   0x04
#define K_LIST  0x05
#define K_IP    0x06

typedef struct
{
  UINT8 val;
  INT8 *str;
}name_list_t;


//文本框（仅数字文本）控件结构
typedef struct _text_t
{
  UINT8 Type;       //控件类型
  UINT8 Focus;      //焦点  
  UINT8 Width;      //宽度
  UINT8 	ReadOnly;//是否只读
  
  INT8 *Caption1;  //标题1
  INT8 *Caption2;  //标题2
  
  
  INT8 x,y;        //相对坐标
  INT8       Px,Py;      //绝对坐标
  
  INT32      Value;      //值
  
  UINT8	     location;		//相对于标题1最左侧的位置,最高位决定是否另行显示
  UINT8      dot;			//小数点位置
  int8u       Cursor;     //光标位置
  int8u       MaskChar;   //屏蔽字符
  
  int32s      Max;        //最大值
  int32s      Min;        //最小值
  
  void       (*GetFocus)(struct _text_t *);             //得到焦点
  void       (*LossFocus)(struct _text_t *);            //失去焦点
  void       (*KeyProcess)(struct _text_t *, int16u );  //按键处理
  void       (*qbch)(struct _text_t *);                 //全部重画
  void       (*SetVal)(struct _text_t *,int32s);        //设定值
  void       (*GetVal)(struct _text_t *,int32s*);       //获取值
       
  UINT16 *p_t_data;		//Value值的数据源 ，数据长度在ReadOnly的低半字节                                 
}k_text_t;

typedef struct _menu_t
{
  int8u Type;       //const 
  int8u Focus;      //const
  int8s *Text;      //内容const
  int8s x,y;        //const
  int8s       Px,Py;      //
  //const void *Token;      //确定时执行的容器
  void *Token;      //确定时执行的容器
  void       (*GetFocus)(struct _menu_t *);             //得到焦点
  void       (*LossFocus)(struct _menu_t *);            //失去焦点
  void       (*KeyProcess)(struct _menu_t *, int16u );  //按键处理
  void       (*qbch)(struct _menu_t *);                 //全部重画
}k_menu_t;


typedef struct _rq_t
{
  int8u       df_line;       //基行
  int8u       cur_line;      //显示的当前行
  int8u       Focus;         //焦点值
  int8s       x,y;           //容器起始坐标
  int8u       ThisCont;      //本屏显示的个数
  int8s       Width,Height;  //容器宽高
  int8u       cont;          //控件个数  	
 	
//  list_t      *kj;
  void        *ct;           //操作的容器
  void        *enter;        //确定后操作的容器
  void        *cancel;       //取消后操作的容器
  void        *cur_ocx;      //当前控件
  void        (*SetFocus)(struct _rq_t * );      //向容器上的东东发送焦点
  void        (*Initsize)(struct _rq_t * );      //初始化
  void        (*Execute)(struct _rq_t * );
  void        **kj;
}k_rq_t;

//列表框类结构
typedef struct _list_t
{
  int8u Type;       //控件类型
  int8u Focus;      //焦点
  UINT8 	ReadOnly;                              //是否只读
  int8u       Cursor;     //光标位置
  
  int8s *Caption1;  //标题1
  int8s *Caption2;  //标题2
  int8s **Value;    //值
  
  UINT8	location;		//相对于标题1最左侧的位置,最高位决定是否另行显示
  int8u Width;      //宽度
    
  int8s x,y;        //相对坐标
  int8s       Px,Py;      //绝对坐标  
  
  int32s      Max;        //最大值
  void       (*GetFocus)(struct _list_t *);             //得到焦点
  void       (*LossFocus)(struct _list_t *);            //失去焦点
  void       (*KeyProcess)(struct _list_t *, int16u );  //按键处理
  void       (*qbch)(struct _list_t *);                 //全部重画
  void       (*SetVal)(struct _list_t *, int8u);        //设定值
  void       (*GetVal)(struct _list_t *, int8u *);      //获取值
  //void       *ReadOnly;   
 
  
  UINT16 *p_l_data;		//Value值的数据源 ，数据长度在ReadOnly的低半字节  
}k_list_t;

typedef struct _ipad_t
{
  const int8u Type;       //类型
  const int8u Focus;      //焦点
  const int8s *Caption1;  //标题1
  const int8s x,y;        //相对坐标
  int8s       Px,Py;      //绝对坐标
  int8u       Cursor;     //光标位置
  int8u       wz;         //光标位置
  int16s      Val[4];     //IP地址的4个字节
  void       (*GetFocus)(struct _ipad_t *);             //得到焦点
  void       (*LossFocus)(struct _ipad_t *);            //失去焦点
  void       (*KeyProcess)(struct _ipad_t *, int16u );  //按键处理
  void       (*qbch)(struct _ipad_t *);                 //全部重画
  void       (*SetVal)(struct _ipad_t *,int8u, int8u, int8u, int8u);      //设定值
  void       (*GetVal)(struct _ipad_t *,int8u *,int8u *,int8u *,int8u *); //获取值
  void       *ReadOnly;                                 //是否只读
}k_ipad_t;    

#endif