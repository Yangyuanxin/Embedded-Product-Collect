#define _KEY_PROCESS_C

#include "head.h"
static int8u control_vl;
int32s getvalue1 (int8u bt)
{
  switch(bt)
  {
  case 0:
    return 1L;
  case 1:
    return 10L;
  case 2:
    return 100L;
  case 3:
    return 1000L;
  case 4:
    return 10000L;
  case 5:
    return 100000L;
  case 6:
    return 1000000L;
  case 7:
    return 10000000L;
  case 8:
    return 100000000L;
  }
return 0;
}

int8u getbit(int32s vl, int8u d)
{
  switch(d)
  {
  case 0:
    return (vl % 10l);
  case 1:
    return ((vl % 100l) / 10l);
  case 2:
    return ((vl % 1000l) / 100l);
  case 3:
    return ((vl % 10000l) / 1000l);
  case 4:
    return ((vl % 100000l) / 10000l);
  case 5:
    return ((vl % 1000000l) / 100000l);
  case 6:
    return ((vl % 10000000l) / 1000000l);
  case 7:
    return ((vl % 100000000l) / 10000000l);
  default:
    break;
  }
  return 0xff;
}



//文本框按键处理数
void T_KeyProcess( struct _text_t *item, int16u ky )
{
	int8u tx,ty;  //绝对坐标
    //此处若不初始化，在进行字符显示的时候，有可能显示不正确
	int8s str[30]="0";
	int32s vl, tmp;
	int8u cr;
	int8u wid;
	
	UINT8 refurbish_lcd_flag = 0;
	
//	if (item->ReadOnly & 0xf0)
//	{
//		return;
//	}
	
	vl = item->Value;
	cr = item->Cursor;	

		
	wid = item->Width;	
	
	tx = (item->location & 0x7f) + item->Px;	
	ty = item->y + item->Py;
    u32 temp_v1;
	switch(ky)
	{
		case KEY_DEC://右移
            if(item->ReadOnly ==1)
            {
                if (cr == 0 )
                {
                    cr = wid;
                }
                cr --;	
            }
            else
            {
                ;
            }
		//	printf("0000a");
			break;
		case KEY_ADD:
       //    printf("5678");

            if(item->ReadOnly ==1)
            {
                
            	temp_v1 = vl;
                tmp = getvalue1(cr);
                if(((temp_v1/tmp)%10) == 9)
				{
					vl = vl - tmp*9;
				}
				else
				{
					vl += tmp;
				}
//                 if ( (vl+tmp) > item->Max)
//                 {
//                     vl = item->Value;
//                 }
//                 vl += tmp;
            }
            else
            {
                ;
            }            
			break;

		case KEY_SET:
	//	printf("k_set");
	//	printf("value=%d\r\n",item->Value);
			if ((UINT32)item->GetVal != 0)
			{
			//	printf("tedst");
				item->SetVal(item, 0);
			}
			break;
		default:
			break;
	}
	
	//如果值发生了改变，要刷新显示
	if (item->Value != vl)
	{
		item->Value = vl;
		if (item->MaskChar == 0)
		{		
			//sprintf(str,(INT8*)wid_dec_strs[item->Width-1],item->Value);			
			PrintfFormat(str, item->Value, item->Width);
		}
		else
		{
			int8u i;
			for (i=0; i<item->Width; i++)
			{
				str[i] = item->MaskChar;
			}
			str[item->Cursor] = getbit(item->Value, item->Width-item->Cursor-1)+0x30;
		}
		//进行小数点填入
	  	if (item->dot)
	  	{
	  		UINT8 temp1,temp2;
	  		//
	  		temp1 = strlen(str);

	  		for (temp2=0; temp2<item->dot; temp2++)
	  		{
	  			str[temp1] = str[temp1-1];
	  			temp1--;
	  		}  		
	  		str[temp1] = '.';
	  	} 	  		
	  	//
	  	CurHideCur();		//写文本框之前需要先隐藏光标
	  	
	  	if (item->location & 0x80)
	  	{
	  		GUI_PutString16_16(tx*8,(ty+2)*8,str,0);
	  	}
	  	else
	  	{
	  		GUI_PutString16_16(tx*8,ty*8,str,0);
            
            //GUI_PutString16_16(tx*8,ty*8,"test",0);
	  	}
	  	
	  	refurbish_lcd_flag = 1;
        
	}
	//	
	if (item->Cursor != cr)
	{
        
		CurHideCur();
		item->Cursor = cr;		
		//进行小数点修正
		if (item->dot)
		{
			if (cr < item->dot)
			{
				tx++;
			}
		}
		
		if (item->location & 0x80)
		{
			CurDisplayCur((tx + item->Width-1 - cr)*8,(ty+2)*8,1*8,16,C_CUR_NO_FLASH);
		}
		else
		{
			CurDisplayCur((tx + item->Width-1 - cr)*8,ty*8,1*8,16,C_CUR_NO_FLASH);
		}
		 
		refurbish_lcd_flag = 1;	
       
	}    
	else if (refurbish_lcd_flag)
	{
		CurShowCur();		//显示光标函数内部会刷新LCD
	}
    
}
//文本框多行选择按键处理数
void T_KeyProcess1( struct _text_t *item, int16u ky )
{
   ;

}
//菜单按键处理数
void M_KeyProcess( struct _menu_t *item, int16u ky )
{
  return;
}

void L_KeyProcessControl( struct _list_t *item, int16u ky )
{
	int8u tx,ty;  //绝对坐标
//	int32u vl;
	int8u cr;
//	int8u wid;    
	
	if (item->ReadOnly & 0xf0)
	{
		return;
	}
	//
	cr = item->Cursor;

	tx = (item->location & 0x7f) + item->Px;
	ty = item->y + item->Py;
	switch(ky)
	{
		case KEY_DEC:

			if (cr == 0)
			{
				cr = item->Max-1;
				break;
			}
			cr --;
			break;
		case KEY_ADD:

			if (cr >= item->Max-1)
			{
				cr = 0;
				break;
			}
			cr ++;
			break;
		case KEY_SET:
			if (item->SetVal != 0)
			//item->SetVal(item, control_vl);
            usr_SetVal_L(item, control_vl);
			break;
	}

	if (cr != item->Cursor)
	{
		CurHideCur();
		
		if (item->location & 0x80)
		{
			GUI_PutString16_16(tx*8,(ty+2)*8,item->Value[cr],0);
		}
		else
		{
			GUI_PutString16_16(tx*8,ty*8,item->Value[cr],0);
		}
		CurShowCur();
		//item->Cursor = cr;
        control_vl = cr;
  }
}
void L_KeyProcess( struct _list_t *item, int16u ky )
{
	int8u tx,ty;  //绝对坐标
//	int32u vl;
	int8u cr;
//	int8u wid;
    
	
	if (item->ReadOnly & 0xf0)
	{
		return;
	}
	//
	cr = item->Cursor;

	tx = (item->location & 0x7f) + item->Px;
	ty = item->y + item->Py;
	switch(ky)
	{
		case KEY_DEC:

			if (cr == 0)
			{
				cr = item->Max-1;
				break;
			}
			cr --;
			break;
		case KEY_ADD:

			if (cr >= item->Max-1)
			{
				cr = 0;
				break;
			}
			cr ++;
			break;
		case KEY_SET:
			if (item->SetVal != 0)
			item->SetVal(item, control_vl);
			break;
	}
	if (cr != item->Cursor)
	{
		CurHideCur();
		
		if (item->location & 0x80)
		{
			GUI_PutString16_16(tx*8,(ty+2)*8,item->Value[cr],0);
		}
		else
		{
			GUI_PutString16_16(tx*8,ty*8,item->Value[cr],0);
		}
		CurShowCur();
		//item->Cursor = cr;
        control_vl = cr;
  }
}

//通用
void L_KeyProcessGern( struct _list_t *item, int16u ky )
{
	int8u tx,ty;  //绝对坐标
//	int32u vl;
	int8u cr;
//	int8u wid;
	
	if (item->ReadOnly & 0xf0)
	{
		return;
	}
	//
	cr = item->Cursor;
	tx = (item->location & 0x7f) + item->Px;
	ty = item->y + item->Py;
	switch(ky)
	{
		case KEY_DEC:

			if (cr == 0)
			{
				//cr = item->Max-1;
				break;
			}
			cr --;
			break;
		case KEY_ADD:

			if (cr >= item->Max-1)
			{
				//cr = 0;
				break;
			}
			cr ++;
			break;
		case KEY_SET:
			if (item->SetVal != 0)
			item->SetVal(item, 0);
			break;
	}
	if (cr != item->Cursor)
	{

		CurHideCur();
		
		if (item->location & 0x80)
		{
			GUI_PutString16_16(tx*8,(ty+2)*8,item->Value[cr],0);
		}
		else
		{
			GUI_PutString16_16(tx*8,ty*8,item->Value[cr],0);
		}
		CurShowCur();
		item->Cursor = cr;
    }
}

void IP_KeyProcess( struct _ipad_t *item, int16u ky )
{
//  int8u tx,ty;
//  int8s s[10];
//  int8u bcr, btt;
//  int16s Vl[4];
//  btt= item->wz;
//  bcr= item->Cursor;
//  Vl[0] = item->Val[0];
//  Vl[1] = item->Val[1];
//  Vl[2] = item->Val[2];
//  Vl[3] = item->Val[3];
//  switch(ky)
//  {
//  case KEY_LEFT:
//    if ((btt==0) && (bcr==0))break;
//    if (bcr==0)
//    {
//      bcr = 2;
//      btt --;
//    }
//    else
//      bcr --;
//    break;
//  case KEY_RIGHT:
//    if ((btt==3) && (bcr==2))break;
//    if (bcr==2)
//    {
//      bcr = 0;
//      btt ++;
//    }
//    else
//      bcr ++;
//    break;
//  case KEY_INC:
//    if ((getvalue1(2-bcr)+Vl[btt]) > 254)break;
//    Vl[btt] += getvalue1(2-bcr);
//    break;
//  case KEY_DEC:
//    if ((Vl[btt]-getvalue1(2-bcr)) < 1)break;
//    Vl[btt] -= getvalue1(2-bcr);
//    break;
//  }
//  tx = item->x+ item->Px+ strlen(item->Caption1);
//  ty = item->y+ item->Py;
//  if (Vl[0]!=item->Val[0])
//  {
//    sprintf(s,"%03d",Vl[0]);
//    outtextxy(tx, ty, s, 3, 1);
//    item->Val[0] = Vl[0];
//  }
//  if (Vl[1]!=item->Val[1])
//  {
//    sprintf(s,"%03d",Vl[1]);
//    outtextxy(tx+4, ty, s, 3, 1);
//    item->Val[1] = Vl[1];
//  }
//  if (Vl[2]!=item->Val[2])
//  {
//    sprintf(s,"%03d",Vl[2]);
//    outtextxy(tx+8, ty, s, 3, 1);
//    item->Val[2] = Vl[2];
//  }
//  if (Vl[3]!=item->Val[3])
//  {
//    sprintf(s,"%03d",Vl[3]);
//    outtextxy(tx+12, ty, s, 3, 1);
//    item->Val[3] = Vl[3];
//  }
//  if ((btt!=item->wz) || (bcr!=item->Cursor))
//  {
//    sprintf(s,"%03d",Vl[item->wz]);
//    outtextxy(tx+(item->wz)*4, ty, s, 3, 0);
//    sprintf(s,"%03d",Vl[btt]);
//    outtextxy(tx+btt*4, ty, s, 3, 1);
//    gotoxy(tx+btt*4+bcr, ty);
//    item->wz = btt;
//    item->Cursor = bcr;
//  }
}



