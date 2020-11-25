#define _QBCH_C

#include "head.h"

//文本框全部重画函数
void T_qbch( struct _text_t *item )
{
  	UINT8 tx,ty;  //绝对坐标
  	INT8 str[20];
  	tx = item->x + item->Px;
  	ty = item->y + item->Py;
	      //  printf("tx=%d,ty=%d\r\n",tx,ty);
	//增加多行的处理，如果是多行显示时，则首行至多显示6个字符，不判最后一个是否是ASC
	//因此，定义字串时需要注意，否则不能正常显示
	if ((item->location & 0x80) && (strlen(item->Caption1) > 12))
	{
		GUI_PutNoStr16_16(tx*8,ty*8,item->Caption1,12,0);
		GUI_PutString16_16((tx+6)*8,(ty+2)*8,item->Caption1+12,0);		
	}
	else
	{
		GUI_PutString16_16(tx*8,ty*8,item->Caption1,0);
	}
	
	//tx += strlen(item->Caption1);
	tx = item->Px + (item->location & 0x7f);
    
    
	if (item->MaskChar == 0)
	{
    	//sprintf(str,(INT8*)wid_dec_strs[item->Width-1],item->Value);
    	
    	//sprintf(str,"%06d",item->Value);   	
    	
    	PrintfFormat(str, item->Value, item->Width);
    	
    }
  	else
  	{
  		UINT8 i;
    	
    	for (i=0; i<item->Width; i++)
    	{
      		str[i] = item->MaskChar;
    	}
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
  	//
  	if (item->location & 0x80)
  	{
  		GUI_PutString16_16(tx*8,(ty+2)*8,str,0);
  		tx += item->Width+1;
		GUI_PutString16_16(tx*8,(ty+2)*8,item->Caption2,0);
  	}
  	else
  	{
  		GUI_PutString16_16(tx*8,ty*8,str,0);
  		tx += item->Width+1;
		GUI_PutString16_16(tx*8,ty*8,item->Caption2,0);
  	}
  	
}
//菜单全部重画函数
void M_qbch( struct _menu_t *item )
{
  int8u tx,ty;
  //tx = item->x + item->Px;
  //ty = item->y + item->Py;
  //outtextxy(tx,ty,item->Text,strlen(item->Text),0);
  
  tx = item->x + item->Px;
  ty = item->y + item->Py;
  GUI_PutString16_16(tx*8,ty*8,(char *)item->Text,0);
  
}

void L_qbch( struct _list_t *item )
{
	INT8 tx,ty;
	tx = item->x + item->Px;
	ty = item->y + item->Py;
	//增加多行的处理，如果是多行显示时，则首行至多显示6个字符，不判最后一个是否是ASC
	//因此，定义字串时需要注意，否则不能正常显示
	if ((item->location & 0x80) && (strlen(item->Caption1) > 12))
	{
		GUI_PutNoStr16_16(tx*8,ty*8,item->Caption1,12,0);
		GUI_PutString16_16((tx+6)*8,(ty+2)*8,item->Caption1+12,0);		
	}
	else
	{
		GUI_PutString16_16(tx*8,ty*8,item->Caption1,0);
	}
	//
	tx = item->Px + (item->location & 0x7f);

	if (item->location & 0x80)
	{
		GUI_PutString16_16(tx*8,(ty+2)*8,item->Value[item->Cursor],0);
	}
	else
	{
		GUI_PutString16_16(tx*8,ty*8,item->Value[item->Cursor],0);
	}
	

}

void IP_qbch( struct _ipad_t *item )
{
//  int8u tx,ty;
//  int8s s[20];
//  tx = item->x+ item->Px;
//  ty = item->y+ item->Py;
//  outtextxy(tx, ty, item->Caption1, strlen(item->Caption1), 0);
//  tx += strlen(item->Caption1);
//  sprintf(s,"%03d.%03d.%03d.%03d",item->Val[0],item->Val[1],item->Val[2],item->Val[3]);
//  outtextxy(tx, ty, s, 15, 0);
}

void T_qbch1( struct _text_t *item )
{
//  int8u tx,ty;  //绝对坐标
//  int8s str[20];
//  tx = item->x + item->Px;
//  ty = item->y + item->Py;
//  outtextxy(tx, ty, item->Caption1, strlen(item->Caption1), 0);
//  tx += strlen(item->Caption1);
//  if (item->MaskChar == 0)
//  sprintf(str,wid_dec_strs[item->Width-1],item->Value+1);
//  outtextxy(tx, ty, str, item->Width, 0);
//  tx += item->Width;
//  outtextxy(tx, ty, item->Caption2, strlen(item->Caption2), 0);
}
