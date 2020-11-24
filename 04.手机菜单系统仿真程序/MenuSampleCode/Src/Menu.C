#include "CPU.H"
#include "Menu.H"

#include "PIC.C"//图标
/*
----------------------------------------------------------------------------
菜单相关变量
----------------------------------------------------------------------------
*/

U8 Flash = 0;               //跳转进入子菜单 刷新标志  >1刷新
U8 language = EN;           //语言标志
U8 KeyFuncIndex = 0;        //索引
//3个备份数组
U8 FatherIndex[MenuSize+1]; //父菜单备份数组
U8 Layer = 0;               //菜单层数

U8 ItemBackup[MenuSize];      //当前菜单项备份
U8 ItemBackup_i = 0;          //

U8 WhereBackup[MenuSize];     //反显位置备份
U8 WhereBackup_i = 0;         //


U8 KeyPressCount = 0;               //用于版本显示

#ifdef DynamicBar
U8 BarDataMode = FixedData;
#endif


U8 Item;            //当前Bar项  范围:[0,ItemNum-1]  包括0 包括ItemNum-1  共包括ItemNum个取值
U8 ItemNum;         //Bar项数
U8 Where;           //当前反显位置,[0,DispMax-1]  反映反显的Bar在屏幕上的位置
//U8 DispMax = 3;     //单屏可以显示的最大项 通常这是个固定值--因为你的液晶的大小决定了你一次可以显示的多少,你也可以在程序中修改,但记得维护它--因为所有的Bar型菜单都是要用到它的
#define DispMax  3

U8 DispMin;         //ItemNum和DispMax中的小者
//当前Bar显示内容指针
//U8 _CONST_ **DispItem; //项显示内容指针
U8 **DispItem; //项显示内容指针

#ifdef DynamicBar
U8 ** DynamicBarDispItem; //动态项显示内容指针
#endif

//菜单资源指针
struct Menu * BarMenu;
//Tip指针
struct Menu * TipBar;


/*
----------------------------------------------------------------------------
      |---->指针对象存放地址,也就是{"Function"},{"Parameter"},{"Measure"},等等这些文本存放在哪里
      |         |---->指针存放的位置,是这些字符的首地址应当存放在哪里  因为这是个指针数组存放的内容就是每个字符串的开始地址
      |         |
U8 _CONST_ * _CONST_ TopItemEN[]=
{
{"Function"},
{"Parameter"},
{"Measure"},
{"Language"},
{"Test"},
};
我们均把这些存放在代码区,以减少对RAM的占用

----------------------------------------------------------------------------
*/


//---------------------------------------------------
//各个菜单的资源
//命名方式是:
//---------------------------------------------------
//顶层菜单


//U8 _CONST_ * _CONST_ TopItemCN[]=
U8 * _CONST_ TopItemCN[]=
{
{"功能"},
{"参数"},
{"测量"},
{"语言"},
{"测试"},
};


//U8 _CONST_ * _CONST_ TopItemEN[]=
U8 * _CONST_ TopItemEN[]=
{
{"Function"},
{"Parameter"},
{"Measure"},
{"Language"},
{"Test"},
};


//"功能"菜单项的内容和长度

//U8 _CONST_ * _CONST_ FuncItemCN[]=
U8 * _CONST_ FuncItemCN[]=
{
{"读出数据"},
{"背光控制"},
{"出厂设置"},
{"修改密码"},

};

//U8 _CONST_ * _CONST_ FuncItemEN[]=
U8 * _CONST_ FuncItemEN[]=
{
{"Read Data"},
{"BackLight"},
{"Factory Set"},
{"Change CODE"}
};
//"参数"菜单项的内容和长度

//U8 _CONST_ * _CONST_ ParaItemCN[]=
U8 * _CONST_ ParaItemCN[]=
{
{"阈值"},
{"死时间T"},
{"灵敏度K"},
{"测量时间"}
};

//U8 _CONST_ * _CONST_ ParaItemEN[]=
U8 * _CONST_ ParaItemEN[]=
{
{"Menu0"},
{"Menu1"},
{"Menu2"},
{"Menu3"},
{"Menu4"},
{"Menu5"},
{"Menu6"},
{"Menu7"},
};
//"语言"菜单项的内容和长度

//U8 _CONST_ * _CONST_ LanguageItemCN[]=
U8 * _CONST_ LanguageItemCN[]=
{
{"中文"},
{"English"},
};

//U8 _CONST_ * _CONST_ LanguageItemEN[]=
U8 * _CONST_ LanguageItemEN[]=
{
{"中文"},
{"English"},
};


//测量

//U8 _CONST_ * _CONST_ MeasureItemCN[]=
U8 * _CONST_ MeasureItemCN[]=
{
{"检测上拉电阻"},
{"清空出错记录"},
{"在线命令"},
{"输入口测试"},
{"模拟主轴测试"},
{"轴信号测试"},
{"继电器测试"},
};

//U8 _CONST_ * _CONST_ MeasureItemEN[]=
U8 * _CONST_ MeasureItemEN[]=
{
{"PushUp Res"},
{"ClearErrCount"},
{"OnlineCMD "},
{"SysInputTest"},
{"SVC TEST"},
{"AXis TEST"},
{"Relay TEST"},
};
//提示键的文本

//U8 _CONST_ * _CONST_ TipCN[]=
U8 * _CONST_ TipCN[]=
{
{"Menu"},
{"选择"},
{"确认"},
{"取消"},
{"?确认?"},
};



//U8 _CONST_ * _CONST_ TipEN[]=
U8 * _CONST_ TipEN[]=
{
{"Menu"},
{"Select"},
{"Ok"},
{"Cancel"},
{"? OK ?"},
};

//----------------------------------------
//宏定义:

#define MenuWhat(n) {(U8 **)n,(sizeof(n)/sizeof(U8 *)),0}
//图标层
_CONST_ struct Menu IcoMenu[] =
{
MenuWhat(TopItemCN),
MenuWhat(TopItemEN),
};

//功能菜单
_CONST_ struct Menu MenuFunc[] =
{
MenuWhat(FuncItemCN),
MenuWhat(FuncItemEN),
};
//参数菜单
_CONST_ struct Menu MenuPara[] =
{
MenuWhat(ParaItemCN),
MenuWhat(ParaItemEN),
};
//测量菜单
_CONST_ struct Menu MenuMeasure[] =
{
MenuWhat(MeasureItemCN),
MenuWhat(MeasureItemEN),
};

//语言菜单
_CONST_ struct Menu MenuLanguage[] =
{
MenuWhat(LanguageItemCN),
MenuWhat(LanguageItemEN),
};

//Tip
_CONST_ struct Menu Tip[] =
{
MenuWhat(TipCN),
MenuWhat(TipEN),
};


//更改密码时询问的图标索引号
#define WarningICO  5
#define PassICO     0
//图标索引
U8 _CONST_ * _CONST_ icos[]=
{
 Function,   //功能
 Para,       //参数
 Time,       //时钟
 Action,     //测量
 Menu,       //语言
 Question,   //询问
 Warning,
 Sound,
};


//跳转到指定菜单
/*
void Jump2Menu(U8 Index)
{
    KeyFuncIndex = Index;             //jump to Menu index
}
*/
void Jump2Menu(U8 Index,U8 FlashMode)
{
    KeyFuncIndex = Index;             //jump to Menu index
    if(FlashMode){
        Flash = FlashMode;
    }
}

