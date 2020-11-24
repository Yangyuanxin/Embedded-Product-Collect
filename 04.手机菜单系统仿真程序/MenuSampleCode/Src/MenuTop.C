/******************************************************************************
 * MenuTop.C - 顶层菜单 条项菜单核心函数 图标层函数等
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


//void Bar(U8 Item_,U8 ItemNum_);
void DispBuildTime(void);//编译时间

/*
*****************************************************************************
* InitMenuVal - 初始化菜单系统
* DESCRIPTION: -
* 该函数必须在进入菜单前被调用   初始化全局变量
* @Para void:
* Return :
*
*****************************************************************************
*/
void InitMenuVal(void)
{
    WhereBackup_i = 0;
    ItemBackup_i = 0;
    Layer = 0;
    FatherIndex[Layer++] = MenuTopIndex;  //push index  待机界面作为起点
}
/*
*****************************************************************************
* DispMenuTop - 待机界面层5个函数
* DESCRIPTION: -
* DispMenuTop   DispMenuTopUp  DispMenuTopDown  DispMenuTopLeft DispMenuTopRight
*
*
*
*****************************************************************************
*/
void DispMenuTop(void)//待机界面
{
    GUI_Clear();
    GUI_DispStringAtBar("Desktop",0,0,126,GUI_TA_HCENTER);
    //显示按键对应的文字提示,0-->"菜单"
    GUI_DispStringAtBar(*(Tip[language].Text),0,Enter_PosY,126,GUI_TA_HCENTER);
}
//待机界面下Up键的处理
void DispMenuTopUp(void)
{
//-------------------
    KeyPressCount = 0;
//-------------------
    GUI_Clear();
//-------------------
//这里是顶层菜单,因此在进入子菜单前需要初始化变量
    InitMenuVal();
//--------------------------
//状态跳转,进入子菜单
    //jump to Menu index
    Jump2Menu(MenuIndex,FlashMode_AutoInit);
    return;
}
//待机界面下Down键的处理
void DispMenuTopDown(void)
{

    if((KeyPressCount++)==0x0F){
        GUI_Clear();
        DispBuildTime();
        GUI_Delay(6500);
        KeyPressCount = 0;
        // we need reflash screen
        Jump2Menu(MenuTopIndex,FlashMode_ReFlash);
        return;

    }
    //return to MenuTop index
    Jump2Menu(MenuTopIndex,FlashMode_NoAction);
    return;
}
//待机界面下Left键的处理
void DispMenuTopLeft(void)
{
    KeyPressCount = 0;
/*
*   这里可以做这个按键的功能定义,因为这是在待机界面下
*   比如,做某个子菜单的快捷链接--按下该按键就直接跳转到某个子菜单
*/
//-------------------
//这里是顶层菜单,因此在进入子菜单前需要初始化变量
//    InitMenuVal();
//--------------------------
//状态跳转,进入子菜单
    Jump2Menu(MenuTopIndex,FlashMode_NoAction);
    return;
}
//待机界面下Right键的处理
void DispMenuTopRight(void)
{
    KeyPressCount = 0;
//----------------------
//    ClearLCD();
//    DispRate(0);  //功能函数的"快捷方式"  到测量显示
//功能函数执行完毕后重新回到待机界面
//    Jump2Menu(MenuTopIndex);
    Jump2Menu(MenuTopIndex,FlashMode_NoAction);
    return;
}
/*
********************************************************
********************************************************
*/
//-----------------------
//菜单备份函数
//备份当前菜单的各个参数
void PUSH(void)
{
    WhereBackup[WhereBackup_i++] = Where;      //备份Where--->高亮显示的位置
    ItemBackup[ItemBackup_i++] = Item;         //备份Item--->当前项的位置
    FatherIndex[Layer++] = KeyFuncIndex;      //备份KeyFuncIndex--->状态机的状态
//-------------------------------

//-------------------------------

}
//菜单还原函数
void POP(void)
{
    Layer--;      //先弹出一层,因为使用的是++,因此当前Layer指向下一个数组位置
    KeyFuncIndex = FatherIndex[Layer-1];     //恢复KeyFuncIndex
    Where = WhereBackup[--WhereBackup_i];    //恢复高亮显示位置Where
    Item = ItemBackup[--ItemBackup_i];       //恢复项位置Item
//-------------------------------

//-------------------------------
    Flash = FlashMode_ReFlash;                     //子项返回,请求刷新
}

/*
*****************************************************************************
* BarDisp - Bar型菜单显示
* DESCRIPTION: -
*
* @Para s:BAR显示的文字内容
* @Para X:X轴坐标
* @Para Y:Y轴坐标
* @Para HighLight:1--->高亮显示当前项  0--->普通显示当前项
//HightLight = 1--->HightLight Disp
//HightLight = 0--->Normal Disp
* Return :
*
*****************************************************************************
*/
//void BarDisp(U8 _CONST_ *s,U8 X,U8 Y,U8 HighLight)
void BarDisp(U8 *s,U8 X,U8 Y,U8 HighLight)
{

#ifdef LessCPUUseage
    if (HighLight == 1) { //高亮显示
        GUI_DispStringAtBar(s,X+8,Y,120,GUI_TA_LEFT);
        GUI_SetEnFont(En_8x16);
        GUI_DispCharAt('>',X,Y);
    } else {  //通常显示
        //显示Bar的Text
        GUI_DispStringAtBar(s,X+8,Y,120,GUI_TA_LEFT);
        GUI_SetEnFont(En_8x16);
        GUI_DispCharAt(' ',X,Y);
    }
#else
    if (HighLight == 1) { //高亮显示
//------------------------------------
        GUI_SetTextMode(GUI_TEXTMODE_REVERSE);//更改字体颜色来突出显示
        GUI_DispStringAtBar(s,X,Y,120,GUI_TA_LEFT);
        GUI_SetTextMode(GUI_TEXTMODE_NORMAL);        //恢复原有字体颜色和背景颜色
//------------------------------------
    } else {  //通常显示
        //显示Bar的Text
        GUI_DispStringAtBar(s,X,Y,120,GUI_TA_LEFT);
    }
#endif
}
/*
FIXME!!!
如何修改BarMenuInit和如何处理Item Where的初始化来达到如此一个目的？
当前系统语言设定是英语，而英语这个项在第2项位置，因此如何能做到当菜单进入语言选择时，反显位置就在英语那里？
1。修改BarMenuInit代码，处理第一次进入时的处理方式，也许就可以仅仅使用从子菜单退出时的处理方法
*/
//Bar型菜单初始化  公共部分
void BarMenuInit(void)
{
U8 Menu_i;          //计数变量 在菜单函数中用到
U8 Menu_j;          //计数变量
//-------------------
//比较"显示屏可以显示的项的数目(DispMax)"跟"需要显示的数目(ItemNum)"
//找出小者,以小者为显示上限
//例如显示屏可以显示3项,需要显示的项有2项,那么显示2项,有3项,显示3项,有4项的话,显示3项
    if (ItemNum > DispMax) {
        DispMin = DispMax;
    } else {
        DispMin = ItemNum;
    }
//--------------------
//    if (Flash) {//从父菜单进入 初始化
        if(Flash == FlashMode_AutoInit){//常规进入 做备份
            PUSH();//在修改Item Where之前备份这些参数数据！！
            Item = 0;
            Where = 0;
        } else if (Flash == FlashMode_ManualInit){    //非常规进入  在之前已经备份了，这里做边界检查
            if(Where > DispMin-1){//检查是否出界
                //Where = DispMin-1;//A.最底部的显示位置
                Where = 0;          //B.最顶部的显示位置
            }
            if(Item > ItemNum-1){//检查是否出界
                //Item = ItemNum-1;//A.最后的一个项
                Item = 0;          //B.最开始的一个项
            }
        }
//    }
//-------------------
//既然Item Where均是未定的，那么我们使用从子菜单退出的方法来找到应该再Where=0位置显示的Item项
//----------------
//方法2
    if (Item >= Where) {//未跨越零点
        Menu_j = Item -Where;
    } else {            //跨越零点
        Menu_j = ItemNum  - (Where - Item);
    }
    //此时Menu_J的值就是顶部(Where=0)显示的Item值
//----------------
    //显示全部的子菜单项
    for (Menu_i = 0; Menu_i < DispMin; Menu_i++) {
        if (Menu_j == Item){ //反显第Item项,位置是Where
            BarDisp(*(DispItem+Item),BarXOffset,Where*BarYSize+BarYOffset,1);
        } else {             //其它项 正常显示
            BarDisp(*(DispItem+Menu_j),BarXOffset,Menu_i*BarYSize+BarYOffset,0);
        }
        if (Menu_j == (ItemNum-1)) {//到了最后一项，回环到最开始的一项
            Menu_j = 0;
        } else {                    //下一项
            Menu_j++;
        }
    }
    Bar(Item,ItemNum,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
}


//Bar项菜单向左按键的行为
/*
可能的几种情况:
A.原有Bar的顶部,按键是向上,那么就需要刷新全部的显示
A1.若是需要显示的Bar多于单屏可以显示的Bar,那么重新显示全部
A2.若是需要显示的Bar不多于单屏可以显示的Bar,取消顶部高亮显示,高亮显示最后一项Bar
B.原有Bar不是在顶部,按键向上,那么取消原有Bar的高亮,高亮显示上一个Bar项
当然还需要维护:高亮显示的位置where/高亮显示Bar项的Item
*/

//维护Bar显示
void BarMenuLeft(void)
{
    U8 Menu_i;          //计数变量 在菜单函数中用到
    U8 Menu_j;          //计数变量
    U8 ItemBack;
    ItemBack = Item;//备份原有的Item,取消原有反显时需要使用
    //维护Item,得到需要反显的Item
    if (Item == 0) {
        Item = ItemNum-1;
    } else {
        Item--;
    }
    //维护显示
    if (Where) {//当前位置不为0,就是说不是在顶部,两次反显

#ifdef LessCPUUseage
        GUI_SetEnFont(En_8x16);
        GUI_DispCharAt(' ',BarXOffset,(Where--)*BarYSize+BarYOffset);
        GUI_DispCharAt('>',BarXOffset,Where*BarYSize+BarYOffset);
#else
        //原来位置取消反显   项是ItemBack  位置Where
        BarDisp(*(DispItem+ItemBack),BarXOffset,(Where--)*BarYSize+BarYOffset,0);
        //上移后的位置反显   项是Item      位置Where-1 (Where--实现)
        BarDisp(*(DispItem+Item),BarXOffset,Where*BarYSize+BarYOffset,1);//高亮显示
#endif

#ifdef RockBack
    } else {//当前位置为0,     顶部  上移  需要刷新全部子菜单项显示   //1.回滚方式,到了顶部后回滚到底部
    #ifdef DynamicBar
        if ((ItemNum > DispMax)||(BarDataMode == DynamicData)){//需要显示的Bar多于单屏可以显示的Bar,那么重新显示//08/29
    #else
        if (ItemNum > DispMax) {//需要显示的Bar多于单屏可以显示的Bar,那么重新显示
    #endif
#else
    }else if(ItemBack != 0){
        if (ItemNum > DispMax) {//需要显示的Bar多于单屏可以显示的Bar,那么重新显示全部
#endif

            Menu_j = Item;
            Menu_i = 0;
            //项是 Item 位置 0
            BarDisp(*(DispItem+Menu_j),BarXOffset,Menu_i*BarYSize+BarYOffset,1);
            if (Menu_j == (ItemNum-1)) {   //维护Menu_j
                Menu_j=0;
            } else {
                Menu_j++;
            }
            //显示其他项
            for (Menu_i = 1; Menu_i < DispMax; Menu_i++) {
                BarDisp(*(DispItem+Menu_j),BarXOffset,Menu_i*BarYSize+BarYOffset,0);
                if (Menu_j == (ItemNum-1)) {
                    Menu_j=0;
                } else {
                    Menu_j++;
                }
            }
            Where = 0;//当前位置依然在开头的0位置
        } else {//单屏能显示完全部菜单项,"顶部 上移"应当跳转到底部(或者什么都不做)
            //当仅仅只有一项的时候，应当一直在Where=0的地方反显的！！！这里也能实现，因为最后是反显的函数
            Where = ItemNum-1;
            //取消顶部的原有的反显  项ItemBack  位置0
            BarDisp(*(DispItem+ItemBack),BarXOffset,0*BarYSize+BarYOffset,0);
            //反显根部  项Item  位置ItemNum-1
            BarDisp(*(DispItem+Item),BarXOffset,Where*BarYSize+BarYOffset,1);//反显
        }
#ifdef RockBack
    }
#else
    }else{
        Item = ItemBack;//Restore
    }

#endif

    Bar(Item,ItemNum,125,120);                  //维护滚动条
    Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
}
//Bar项菜单向右按键的行为
//维护Bar显示
void BarMenuRight(void)
{
    U8 Menu_i;          //计数变量 在菜单函数中用到
    U8 Menu_j;          //计数变量
    U8 ItemBack;
    ItemBack = Item;
//---维护Item
    if (Item == (ItemNum-1)) {
        Item = 0;
    } else {
        Item++;
    }
//------------
//---维护显示
    if (Where != (DispMin-1)) {//当前位置不为最末,两次反显


#ifdef LessCPUUseage
        /*
        如果是使用">"这样的符号标志当前反显项，那么直接反显该标志好了 2007、1、8
        */
        GUI_SetEnFont(En_8x16);
        GUI_DispCharAt(' ',BarXOffset,(Where++)*BarYSize+BarYOffset);
        GUI_DispCharAt('>',BarXOffset,Where*BarYSize+BarYOffset);
#else
        //原来位置取消反显   项是ItemBack  位置Where
        BarDisp(*(DispItem+ItemBack),BarXOffset,(Where++)*BarYSize+BarYOffset,0);
        //下移后的位置反显   项是Item      位置Where+1(Where++实现)
        BarDisp(*(DispItem+Item),BarXOffset,Where*BarYSize+BarYOffset,1);//反显
#endif

#ifdef RockBack
    } else {    //当前位置为DispMax    底部  下移
    #ifdef DynamicBar
        if ((ItemNum > DispMax)||(BarDataMode == DynamicData)){//需要显示的Bar多于单屏可以显示的Bar,那么重新显示//08/29
    #else
        if (ItemNum > DispMax) {//需要显示的Bar多于单屏可以显示的Bar,那么重新显示
    #endif
#else
    }else if(ItemBack != (ItemNum-1)){//当前位置为DispMax    如果原来显示位置不是最后一项，则 底部  下移
        if (ItemNum > DispMax) {//需要显示的Bar多于单屏可以显示的Bar,那么重新显示
#endif
            Menu_j = Item;
            Menu_i = DispMax;
            //项是 Item  位置是 DispMax
            BarDisp(*(DispItem+Menu_j),BarXOffset,(Menu_i-1)*BarYSize+BarYOffset,1);
            if (Menu_j == 0) {
                Menu_j = ItemNum-1;
            } else {
                Menu_j--;
            }
            //显示其他项
            for (Menu_i = DispMax-1; Menu_i > 0; Menu_i--) {
                BarDisp(*(DispItem+Menu_j),BarXOffset,(Menu_i-1)*BarYSize+BarYOffset,0);
                if (Menu_j == 0) {
                    Menu_j = ItemNum-1;
                } else {
                    Menu_j--;
                }
            }
            Where = DispMax-1;//当前位置依然末尾位置
        } else {//单屏能显示完全部菜单项,"底部 下移"应当跳转到顶部反显(或者什么都不做)
            Where = 0;
            //取消底部原有反显   项ItemBack  位置ItemNum-1
            BarDisp(*(DispItem+ItemBack),BarXOffset,(ItemNum-1)*BarYSize+BarYOffset,0);
            //反显顶部 项Item  位置0
            BarDisp(*(DispItem+Item),BarXOffset,Where*BarYSize+BarYOffset,1);//反显
        }
#ifdef RockBack
    }
#else
    }else{
        Item = ItemBack;//Restore
    }
#endif

    Bar(Item,ItemNum,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
    Jump2Menu(SelfMenuIndex,FlashMode_NoAction);

}
//*********************************************************


/*
*****************************************************************************
* DispIcoMenuInit - 图标层菜单界面
* DESCRIPTION: -
* 共有5个函数
 DispIcoMenuInit    DispIcoMenuUp   DispIcoMenuDown
 DispIcoMenuLeft    DispIcoMenuRight
* @Return :
*
*****************************************************************************
*/
void DispIcoMenuInit()
{

    ItemNum = IcoMenu[language].TextNum;//多少数据项
    DispItem = IcoMenu[language].Text;  //数据


    GUI_Clear();
//-------------------
    if(Flash == FlashMode_AutoInit){//从0进入,初始化Item等值
        PUSH();
        Item = 0;
        Where = 0;
    }
    GUI_DispStringAtBar(*(DispItem+Item),0,ICO_PosY+ICO_YSize,126,GUI_TA_HCENTER);//菜单名字
    GUI_DrawIcon(icos[Item],ICO_PosX,ICO_PosY);             //图标

    //显示按键对应的文字提示,1-->"进入"
    GUI_DispStringAtBar(*(Tip[language].Text+1),0,Enter_PosY,126,GUI_TA_HCENTER);

    Bar(Item,ItemNum,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
    return;
}

/*
*****************************************************************************
* DispIcoMenuUp - 图标层下确定键的行为
* DESCRIPTION: -
* 分发处理各个跳转
* 第一层菜单的Up键处理
* Return :
*
*****************************************************************************
*/
void DispIcoMenuUp()
{

    switch(Item){
        case 0:{
            GUI_Clear();
            //功能
            Jump2Menu(MenuFuncIndex,FlashMode_AutoInit);
            return;
        }
        case 1:{
            GUI_Clear();
            //参数
            Jump2Menu(MenuParaIndex,FlashMode_AutoInit);
            return;
        }
        case 2:{
            GUI_Clear();
            //测量
            Jump2Menu(MenuMeasureIndex,FlashMode_AutoInit);
            return;
        }
        case 3:{
            GUI_Clear();
            //语言
            Jump2Menu(MenuLanguageIndex,FlashMode_ManualInit);
            //在MenuLanguage.C中的DispMenuLanguageInit函数中修改Item Where!
            return;
        }
        case 4:{
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
            return;
        }
        case 5:{
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
            return;
        }
        default:{
            Jump2Menu(MenuTopIndex,FlashMode_NoAction);
            return;
        }
    }

}

void DispIcoMenuDown()
{
    POP();
}

void DispIcoMenuLeft()
{
    if(Item==0){
        Item = ItemNum-1;
    }else{
        Item--;
    }

    GUI_DispStringAtBar(*(DispItem+Item),0,ICO_PosY+ICO_YSize,126,GUI_TA_HCENTER);//菜单名字
    GUI_DrawIcon(icos[Item],ICO_PosX,ICO_PosY);             //图标
    Bar(Item,ItemNum,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
    Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
}

void DispIcoMenuRight()
{
    if(Item==(ItemNum-1)){
        Item = 0;
    }else{
        Item++;
    }

    GUI_DispStringAtBar(*(DispItem+Item),0,ICO_PosY+ICO_YSize,126,GUI_TA_HCENTER);//菜单名字
    GUI_DrawIcon(icos[Item],ICO_PosX,ICO_PosY);             //图标
    Bar(Item,ItemNum,MenuBarPosX,MenuBarNumPosX);                  //维护滚动条
    Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
}
//---------------------------------------------------------



//---------------------------

void DispBuildTime(void)//编译时间
{

    GUI_DispStringAt("Build Time:",0,0);
    GUI_DispStringAt(__DATE__,0,2);
    GUI_DispStringAt(__TIME__,0,4);
}
