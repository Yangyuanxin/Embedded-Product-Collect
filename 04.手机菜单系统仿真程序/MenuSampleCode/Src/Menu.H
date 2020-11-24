/******************************************************************************
 * Menu.H - 菜单相关宏开关 函数声明等
 *
 *
 * DESCRIPTION: -
 *
 * modification history
 * --------------------
 * 01a, 04/sep/2007, 梁炎昌 written
 * --------------------
 ******************************************************************************/

/*
LessCPUUseage:
高亮显示当前项的做法是
1.反显整个Bar项文字 提示当前项
2.通过一个'>'来提示当前位置
第1种方法在低速CPU和LCD上比较费时
因此这个情况下用第2种方法,毕竟反显一个'>'比反显整行要省事的多
但没有解决根本问题,如果依然会出现要全部刷新的情况
如果CPU慢#define LessCPUUseage
否则不定义该值
*/
//
//#define LessCPUUseage

//动态生成条项菜单  需要回滚方式的支持
//#define DynamicBar
//是否回滚方式
#define RockBack

//数字键快捷方式
//需要预定义数字键的键值Key_1---Key_9
//并提供一个函数能把键值转化成实际对应数字 Key = chang_code(Key);
//#define DigiKeyLink

//检查如果需要动态条项菜单,就需要回滚方式,这里做检查
#ifdef DynamicBar
    #ifndef RockBack
        #error  "DynamicBar need RockBack!"
        #error  "#define RockBack pls"
    #endif
#endif

//--------------
//显示位置的宏定义
//父Bar项的显示位置
#define FatherXOffset    10     //Father的X偏置
#define FatherYOffset    2      //Father的Y偏置
//Bar项的显示XY偏置
#define BarXOffset    0     //Bar的X偏置
#define BarYOffset    0     //Bar的Y偏置
//Bar项的XY尺寸
#define BarXSize      (7*12)    //Bar的长度
#define BarYSize      2         //Bar的间隔

//语言代码
#define CN    0
#define EN    1

//不初始化Flash
#define FlashMode_NoAction     0
//系统系统负责初始化
#define FlashMode_AutoInit     1
//手动初始化
#define FlashMode_ManualInit   2
//重绘画面
#define FlashMode_ReFlash      3



//也就是连续使用Push的个数
#define MenuSize 4          //菜单深度最大值

#ifdef DynamicBar
#define FixedData       0
#define DynamicData     1
//条项内容的来源是固定的还是动态生成的，通常是固定的，
#endif

//U8 DispMax = 3;     //单屏可以显示的最大项 通常这是个固定值--因为你的液晶的大小决定了你一次可以显示的多少,你也可以在程序中修改,但记得维护它--因为所有的Bar型菜单都是要用到它的
#define DispMax  3

//-----------------------------------------------------
//Bar型菜单的数据结构
struct Menu
{
//    U8 _CONST_ **Text;     //指向显示内容数组的指针
    U8 **Text;     //指向显示内容数组的指针
    U8 TextNum;         //项个数
    void* (*Action)();  //对应的执行函数
};

//----------------------------------------------------
//对外声明定义于Menu.C的内容
extern U8 ReFlash ;             //子项菜单返回 刷新标志
extern U8 Flash ;               //跳转进入子菜单 刷新标志
extern U8 language;           //语言标志
extern U8 KeyFuncIndex ;        //索引

extern U8 FatherIndex[MenuSize+1]; //父菜单备份数组
extern U8 Layer;               //菜单层数

extern U8 ItemBackup[MenuSize];      //当前菜单项备份
extern U8 ItemBackup_i;          //

extern U8 WhereBackup[MenuSize];     //反显位置备份
extern U8 WhereBackup_i;         //


extern U8 KeyPressCount;         //统计连续按键时间,长时间按键则 版本显示

#ifdef DynamicBar
extern U8 BarDataMode;
#endif
extern U8 Item;            //当前Bar项  范围:[0,ItemNum-1]  包括0 包括ItemNum-1  共包括ItemNum个取值
extern U8 ItemNum;         //Bar项数
extern U8 Where;           //当前反显位置,[0,DispMax-1]  反映反显的Bar在屏幕上的位置

extern U8 DispMin;         //ItemNum和DispMax中的小者

//当前Bar显示内容指针
//extern U8 _CONST_ **DispItem; //项显示内容指针
extern U8 **DispItem; //项显示内容指针

#ifdef DynamicBar
extern U8 ** DynamicBarDispItem; //动态项显示内容指针
#endif
//---------------------------------------------------
//返回当前菜单
#define SelfMenuIndex FatherIndex[Layer-1]

//----------------------------------------------------
//extern void Jump2Menu(U8 Index);//,U8 FlashMode);
extern void Jump2Menu(U8 Index,U8 FlashMode);

//图标层
extern _CONST_ struct Menu IcoMenu[];
//功能菜单
extern _CONST_ struct Menu MenuFunc[];
//参数菜单
extern _CONST_ struct Menu MenuPara[];
//测量菜单
extern _CONST_ struct Menu MenuMeasure[];

//语言菜单
extern _CONST_ struct Menu MenuLanguage[];

//Tip
extern _CONST_ struct Menu Tip[];
//图标
extern U8 _CONST_ * _CONST_ icos[];

//菜单资源指针
extern struct Menu * BarMenu;
//Tip指针
extern struct Menu * TipBar;

//--------------------------------------------------------------------
#define Enter_PosX   50      //"进入"显示的位置
#define Enter_PosY    6
#define ICO_PosX     48      //图标显示的位置
#define ICO_PosY     2
#define ICO_XSize    32      //图标的尺寸
#define ICO_YSize    (-2)

#define MenuBarPosX     125
#define MenuBarNumPosX  120




//-----------------------
//如下定义从KeyTab中得到
//调整KeyTab时必须更新如下的定义
//否则菜单链接出错!
#define MenuTopIndex       0
#define MenuIndex          5
#define MenuFuncIndex     10
#define MenuParaIndex     15
#define MenuLanguageIndex 20
#define MenuMeasureIndex  25

#define MenuDataIndex     30



void BarMenuInit(void);
void BarMenuLeft(void);
void BarMenuRight(void);
void PUSH(void);
void POP(void);



void DispMenuTop();
U8 CheckKey(void);
