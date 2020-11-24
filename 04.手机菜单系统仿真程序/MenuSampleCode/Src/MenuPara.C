/******************************************************************************
 * MenuPara.C - "参数"菜单  菜单函数使用模版
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


//参数
void DispMenuParaInit()
{
//----------------------
    GUI_Clear();
//--------------------
    //切换Bar显示内容
//---------------------
    BarMenu = &MenuPara[language];
    ItemNum = (*BarMenu).TextNum;
    DispItem = (*BarMenu).Text;

    //用户的初始化代码请放在这里
    //显示按键对应的文字提示,1-->"进入"
    GUI_DispStringAtBar(*(Tip[language].Text+1),0,Enter_PosY,126,GUI_TA_HCENTER);
    //公共初始化部分
    BarMenuInit();
}

/*
*****************************************************************************
菜单使用例程
0.Jump2Menu + FlashMode_AutoInit/FlashMode_ManualInit
  跳转到任意菜单,FlashMode_AutoInit-->在BarMenuInit函数中执行PUSH;
  FlashMode_ManualInit-->子菜单的Init函数中PUSH,看例子:DispMenuLanguageInit
1.PUSH + POP
  执行一些功能,并使用LCD后,回到原有界面,回到原有界面--包括反显位置都恢复
2.Jump2Menu(SelfMenuIndex,FlashMode_ReFlash);
  执行一些功能,并使用LCD后,执行如上调用,回到原有界面--包括反显位置都恢复
3.POP
  执行一些功能,并使用LCD后,回到上层菜单,恢复上层界面
4.Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
  执行一定功能后,但不占用LCD的话,直接界面不动

*****************************************************************************
*/
void DispMenuParaUp()
{
    switch(Item){
        case 0:{//Jump2Menu
            GUI_Clear();
            GUI_DispStringAt("Do Something \nthen Back to \nTop Menu!",0,0);
            GUI_Delay(4000);
            Jump2Menu(MenuTopIndex,FlashMode_AutoInit);
            break;
        }
        case 1:{//PUSH  -->POP
            PUSH();
            GUI_Clear();
            GUI_DispStringAt("Do Something \nthen Back to \nThis Menu!",0,0);
            GUI_Delay(4000);
            POP();
            break;
        }
        case 2:{//ReFlash = 1
            GUI_Clear();
            GUI_DispStringAt("Do Something \nthen Back to \nThis Menu!",0,0);
            GUI_Delay(4000);
            Jump2Menu(SelfMenuIndex,FlashMode_ReFlash);
            break;
        }
        case 3:{//POP
            GUI_Clear();
            GUI_DispStringAt("Do Something \nthen Back to \nFather Menu!",0,0);
            GUI_Delay(4000);
            POP();
            break;
        }
        case 4:{//No ReFlash
            //No Action Here!
            //Or The Action no need the LCD Disp
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
            break;
        }
        default:{
            Jump2Menu(SelfMenuIndex,FlashMode_NoAction);
            return;
        }
    }
}
void DispMenuParaDown()
{
    POP();
}

void DispMenuParaLeft()
{
    BarMenuLeft();
}
void DispMenuParaRight()
{
    BarMenuRight();
}




