/******************************************************************************
 * MenuMeasure.C - "测量"菜单
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

//测量
//Bar型菜单
void DispMenuMeasureInit()
{
//----------------------
    GUI_Clear();
//--------------------
    BarMenu = &MenuMeasure[language];
//--------------------

    ItemNum = (*BarMenu).TextNum;
    DispItem = (*BarMenu).Text;

    //用户的初始化代码请放在这里

    //显示按键对应的文字提示,1-->"进入"

    GUI_DispStringAtBar(*(Tip[language].Text+1),0,Enter_PosY,126,GUI_TA_HCENTER);

    BarMenuInit();
}



void DispMenuMeasureUp()
{
//-----------
    PUSH();
//-------------------
    GUI_Clear();
    switch(Item){
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
           break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            break;
        case 16:
            break;
        case 17:
            break;
        default:
            break;
    }
    GUI_Clear();
//-------------------
    POP();
//-------------------

}
void DispMenuMeasureDown()
{
    POP();
}

void DispMenuMeasureLeft()
{
    BarMenuLeft();
}
void DispMenuMeasureRight()
{
    BarMenuRight();
}

