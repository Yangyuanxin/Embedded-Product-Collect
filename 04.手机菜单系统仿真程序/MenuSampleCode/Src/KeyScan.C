#include "CPU.H"
extern void Delayms(U16 Ms);

/*
*****************************************************************************
* KeyScan - 键盘扫描函数
* DESCRIPTION: -
* 判4个按键那个有按下
* @Param void:
* @Return :有按键 返回键值 无按键 返回false 因此键值定义不能为false
* ----
*****************************************************************************
*/
U8 KeyScan(void)
{
    Key0 = 1;
    Key1 = 1;
    Key2 = 1;
    Key3 = 1;
    if((Key0==0)||(Key1==0)||(Key2==0)||(Key3==0)){
        Delayms(20);
        if((Key0==0)||(Key1==0)||(Key2==0)||(Key3==0)){
            if((Key0==0)&&(Key1==1)&&(Key2==1)&&(Key3==1)){      //Key0
                return Key_Up;
            }else if((Key0==1)&&(Key1==0)&&(Key2==1)&&(Key3==1)){//Key1
                return Key_Right;
            }else if((Key0==1)&&(Key1==1)&&(Key2==0)&&(Key3==1)){//Key2
                return Key_Left;
            }else if((Key0==1)&&(Key1==1)&&(Key2==1)&&(Key3==0)){//Key3
                return Key_Down;
            }else{
                return false;
            }

        }else {
            return false;
        }//no key press
    }else{
        return false;
    }
}
#if 0
/*
*****************************************************************************
* WaitKey - 死等一个有效按键输入
* DESCRIPTION: -
*
* @Param void:
* @Return : 按键键值
* ----
*****************************************************************************
*/
U8 WaitKey(void)
{

while(1){
    switch(KeyScan()){
        case Key_Up:{
            return Key_Up;
            break;
        }
        case Key_Down:{
            return Key_Down;
            break;
        }
        case Key_Left:{
            return Key_Left;
            break;
        }
        case Key_Right:{
            return Key_Right;
            break;
        }
        default:
            break;
    }
}

}
#endif
