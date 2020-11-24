/******************************************************************************
 * MenuFunc.C - "功能"菜单 条项动态生成代码
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
#include "Menu.H"
#include "KeyScan.H"


//功能
//Bar型菜单
void DispMenuFuncInit()
{
//--------------------
    GUI_Clear();
//--------------------
    BarMenu = &MenuFunc[language];
    ItemNum = (*BarMenu).TextNum;
    DispItem = (*BarMenu).Text;

    //显示按键对应的文字提示,1-->"进入"
    GUI_DispStringAtBar(*(Tip[language].Text+1),0,Enter_PosY,126,GUI_TA_HCENTER);

    //用户定义的初始化代码请放在这里
    BarMenuInit();//调用公共初始化的代码
}

void DispMenuFuncUp()
{

    switch(Item){
        case 0:{
#ifdef  DynamicBar
            Jump2Menu(MenuDataIndex,FlashMode_AutoInit);
#else
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
#endif                        

            break;
        }
        case 1:{
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
            break;
        }
        default:{
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
            break;
        }
    }

}
void DispMenuFuncDown()
{
    POP();
}

void DispMenuFuncLeft()
{
    BarMenuLeft();
}
void DispMenuFuncRight()
{
    BarMenuRight();
}





#ifdef DynamicBar


//------------------------------------------------
#define DataItemSize 8
U8 DataItem[DispMax][DataItemSize]=
{
{"DataX  "},
{"DataX  "},
{"DataX  "},
};


U8 *_CONST_ DataItem_p[]=
{
DataItem[0],
DataItem[1],
DataItem[2]
};


U8 DynamicItem;
U8 MaxDynamicItem = 0;

/*
实现动态数据显示的最简单的方式设定一定大小的缓冲，而后填充数据，但这样把可以显示的项限制了。这个方法对于维护来说是最为简单的
2006/08/27
如何利用有限缓冲来实现动态数据的条项层显示？
比如仅仅缓冲液晶可以显示的条项数目，然后发现上/下越界了，就刷新缓冲？
但这样就要重新另外写类似BarMenuLeft等函数来维护显示
*/
/*
这里使用一个仅仅跟LCD显示条项数目一致的缓冲区来缓冲动态生成的条项
当实际条项数目MaxDynamicItem<=DispMax时 函数BarMenuLeft BarMenuRight工作正常
而当MaxDynamicItem>DispMax时，我们设ItemNum = DispMax，在原本的代码中，这意味着LCD刚刚能显示所有的项
于是一旦到了顶部上翻时就反显底部，到了底部下翻时就反显顶部 但是事实上的项的数目不是ItemNum!而是MaxDynamicItem
我们期望的行为是顶部上翻时，修改底部的项为新的项的数据，屏幕整个刷新(底部下翻类似)
做法是设ItemNum = DispMax 并让上下翻的行为跟 ItemNum > DispMax时的行为一致，跟踪位置动态修改显示内容
*/
void DispMenuDataInit(void)
{
    U8 Menu_j;          //计数变量
    U8 *ChangeDispItem; //项显示内容指针
//----------------------
    GUI_Clear();
//--------------------
    GUI_Delay(100);
//    do{
//        MaxDynamicItem = InputNum(4,4,9);//要求输入一个动态菜单的值,我们根据这个值生成菜单
        MaxDynamicItem = 8;
//        if(MaxDynamicItem == 0xFF){//退出
//            Jump2Menu(SelfMenuIndex,FlashMode_AutoInit);
// //           ReFlash = true;
//            return;
//        }
//    }while(MaxDynamicItem ==0x00);//大于0才有效

//    GUI_Clear();
    DynamicItem = 0;

    if(MaxDynamicItem >DispMax){//需要显示的项数比液晶可以显示的多
        ItemNum = DispMax;
        //08/29
        BarDataMode = DynamicData;//通知BarMenuLeft和BarMenuRight函数现在的数据是动态数据
        //当ItemNum == DispMax时，默认的显示方式是到头了直接跳到尾部的(到尾部了直接跳到头部的)
        //但是既然这是动态内容的话，而且实际上的项是MaxDynamicItem不少于DispMax，应当翻滚显示的，通过修正BarDataMode这个标志
        //通知BarMenuLeft和BarMenuRight
    } else {
        ItemNum = MaxDynamicItem;
    }


    for(Menu_j = 0;Menu_j < ItemNum; Menu_j++){//初始化动态内容
       ChangeDispItem = (U8 *)&DataItem[Menu_j];
        *(ChangeDispItem + 4) = Menu_j+'0';
//        *(DataItem_p[Menu_j]+4)       = Menu_j+'0';
    }


    DispItem = (U8 **)DataItem_p;


    //显示按键对应的文字提示,2-->"OK"
    GUI_DispStringAtBar(*(Tip[language].Text+1),0,Enter_PosY,126,GUI_TA_HCENTER);
    BarMenuInit();
    Bar(DynamicItem,MaxDynamicItem,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
}

void DispMenuDataUp()
{
    //没有动作，停留在本层菜单
    Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
}
void DispMenuDataDown()
{
    //08/29
    BarDataMode = FixedData;
    POP();
}

void DispMenuDataLeft()
{
    U8 *ChangeDispItem; //项显示内容指针
    U8 ItemBackup;
    if(DynamicItem == 0){//
        DynamicItem = MaxDynamicItem-1;
    } else {
        DynamicItem--;
    }

    if(Where == 0){//顶部上移，需要修改数据
        if (Item == 0) {//获得Item
            ItemBackup = ItemNum-1;
        } else {
            ItemBackup = Item-1;
        }
        ChangeDispItem = (U8*)&DataItem[ItemBackup];
        *(ChangeDispItem + 4) = DynamicItem+'0';
    }
    BarMenuLeft();//

    Bar(DynamicItem,MaxDynamicItem,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
}
void DispMenuDataRight()
{
    U8 *ChangeDispItem; //项显示内容指针
    U8 ItemBackup;
    if(DynamicItem == MaxDynamicItem-1){//维护实际的Item值
        DynamicItem = 0;
    } else {
        DynamicItem++;
    }

    if(Where == (DispMin-1)){//底部下移，需要修改数据
        if (Item == (ItemNum-1)) {//获得Item
            ItemBackup = 0;
        } else {
            ItemBackup = Item + 1;
        }
        ChangeDispItem = (U8*)&DataItem[ItemBackup];//获得修改位置
        *(ChangeDispItem + 4) = DynamicItem+'0';
    }
    BarMenuRight();
    Bar(DynamicItem,MaxDynamicItem,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
}
#endif

