#include "stdlib.h"
#include "stdio.h"
#include "my_string.h"

#pragma pack(1)

/*----------------------------------------------------------------------------*/
/* 函数名称: char isdec(char in)                                              */
/* 函数描述: 判断是否是十进制字符                                               */
/*----------------------------------------------------------------------------*/
char isdec(char in)
{
	if( (in>=0x30)&&(in<=0x39) )
		return 1;
	else
		return 0;
}

/*----------------------------------------------------------------------------*/
/* 函数名称: int isfloatstring(const char *in)                                 */
/* 函数描述: 判断是否是正确的浮点数字符                                         */
/*----------------------------------------------------------------------------*/
int isfloatstring(const char *in)
{
    char *p= (char*)in;
    char tmp;
    int len=0;

    do{
        tmp = *p++;
        //结束符号
        if( tmp==0 ){
            break;
        //中间参杂非数字字符
        }else if( (isdec(tmp))||(tmp=='.')||(tmp=='-')||(tmp=='+') ){
            len++;
            if( len>20 ){
                len = 0;    //超过20位数字,unsigned long 0~4394967295
            }
        }else{
            len = 0;
            break;        
        }
    }while(1);
    
    return len;
}

/*----------------------------------------------------------------------------*/
/* 函数名称: char ischar(char in)                                              */
/* 函数描述: 判断是否是数字,字母,引号                                           */
/*----------------------------------------------------------------------------*/
char ischar(char in)
{
    if( ((in>='0')&&(in<='9'))  //是数字
      ||((in>='a')&&(in<='z'))  //是小写字母
      ||((in>='A')&&(in<='Z'))  //是大写字母
      ||((in=='"'))             //
      ||((in=='.')) )           //小数点
    {
        return 1;
	}

    return 0;        
}


/*----------------------------------------------------------------------------*/
/* 函数名称: bool DecChar2Int(char *in, int *re_int, int k)                   */
/* 函数描述: 将十进制的字符转换成int                                          */
/*           *in: 输入字符缓冲指针                                            */
/*           *re_int: 返回int型数据到这个指针位置                             */
/*           k: 转换的字符位数, 如需要分析"12", 则K=2                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
int isdecstring(const char *in)
{
    char *p= (char*)in;
    char tmp;
    int len=0;
    
    do{
        tmp = *p++;
        //结束符号
        if( tmp==0 ){
            break;
        //中间参杂非数字字符
        }else if( isdec(tmp)==0 ){
            len = 0;
            break;
        }else{
            len++;
            if( len>10 ){
                len = 0;    //超过5位数字,unsigned long 0~4394967295
            }
        }
    }while(1);
    
    return len;
}

long DecStr2Int(char *in, unsigned char k)
{
  char i=0;
	unsigned char B[9];
  long c_int=0;

    if( (k<1)||(k>8) )
        return 0;

	B[0] = *in++;
	B[1] = *in++;
	B[2] = *in++;
	B[3] = *in++;
	B[4] = *in++;
	B[5] = *in++;
	B[6] = *in++;
	B[7] = *in++;

  for( i=0; i<8; i++ ){
    if( isdec(B[i])==0 )
      break;
  }
  //
  if( i<1 )
    return 0;
  //
  if( k>i )
		k = i;

  //
  i = 0;

  if( k==8 )
      c_int = (B[i++]&0x0f)*10000000;

  if( k>=7 )
      c_int += (B[i++]&0x0f)*1000000;

  if( k>=6 )
      c_int += (B[i++]&0x0f)*100000;

  if( k>=5 )
      c_int += (B[i++]&0x0f)*10000;

  if( k>=4 )
	  c_int += (B[i++]&0x0f)*1000;

  if( k>=3 )
	  c_int += (B[i++]&0x0f)*100;

  if( k>=2 )
	  c_int += (B[i++]&0x0f)*10;

  if( k>=1 )
	  c_int += (B[i++]&0x0f);


    return c_int;
}

char* Int2DecStr(int num)
{	
    static char str[6];
    char *p=str;
    int k;
    
    
    k = num/10000;
    if(k>0){
        *p++ = k+'0';
        num = num-k*10000;
    }
    
    k = num/1000;
    if(k>0){
        *p++ = k+'0';
        num = num-k*1000;
    }
    
    k = num/100;
    if(k>0){
        *p++ = k+'0';
        num = num-k*100;
    }
    
    k = num/10;
    if(k>0){
        *p++ = k+'0';
        num = num-k*10;
    }
    
    *p++ = num%10+'0';
    
    *p = '\0';

	return str;    
}





#if 0
void test_arm_lib(void)
{
    char str_tmp[100];
    char *str_p="1.23456789";
    double double_i;
    float float_i;
    
    sprintf(str_tmp,"%s%d%s%d","bytes of double:",sizeof(double_i)," bytes of float:",sizeof(float_i));
    printf(str_tmp);    printf("\r\n");
	//
	double_i = 1.23456789;
	sprintf(str_tmp,"%s%f","double 1.23456789=",double_i);	printf("\r\n");
	printf(str_tmp);
	//
	sprintf(str_tmp,"%f",double_i);
	double_i = atof(str_tmp);
	sprintf(str_tmp,"%s%f","atof(double)->",double_i);	printf("\r\n");
	printf(str_tmp);

	float_i = 1.23456789;
	sprintf(str_tmp,"%s%f","float 1.23456789=",float_i);	printf("\r\n");
	printf(str_tmp);
	float_i = atof(str_p);
	sprintf(str_tmp,"%s%f","atof(float_i)->",float_i);	printf("\r\n");
	printf(str_tmp);
}
#endif
