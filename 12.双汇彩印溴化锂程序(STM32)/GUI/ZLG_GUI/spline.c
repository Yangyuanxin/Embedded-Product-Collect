/****************************************************************************************
* 文件名：SPLINE.C
* 功能：二次参数样条曲线生成程序。
* 作者：黄绍斌
* 日期：2003.09.09
****************************************************************************************/
#define _SPLINE_C

#include  "head.h"


#define  DIV_FACTOR 		8.0				/* 平滑控制参数 */
/****************************************************************************
* 名称：max()
* 功能：求两个整数中最大的整数值。
* 入口参数：a			整数1
*          b		整数2
* 出口参数：返回最大的整数值
****************************************************************************/
int  max(int a, int b)
{  if(a>b) return(a);
     else  return(b);
}

/****************************************************************************
* 名称：abs()
* 功能：求一个整数的绝对值。
* 入口参数：a			取绝对值的数据
* 出口参数：返回a的绝对值
***************************************************************************
int  abs(int a)
{  if(a>0) return(a);
     else  return(-a);
}
*/


 


/****************************************************************************
* 名称：CURVE_SetCurve()
* 功能：设置三点曲线参数。
* 入口参数：	cl		要操作的曲线对象，CURVE结构
*           ax		a点x坐标值
*           ay		a点y坐标值
*			bx		b点x坐标值
*           by		b点y坐标值
*			cx		c点x坐标值
*           cy		c点y坐标值
* 出口参数：无
* 说明：
****************************************************************************/
void  CURVE_SetCurve(CURVE *cl,
                     float ax, float ay, 
               		 float bx, float by,
               		 float cx, float cy) 
{  cl->Ax = ax;
   cl->Ay = ay;
   cl->Bx = bx; 
   cl->By = by;
   cl->Cx = cx; 
   cl->Cy = cy;
   
   cl->Ndiv = (int)(max( abs((int)ax), abs((int)ay) ) / DIV_FACTOR);
}


/****************************************************************************
* 名称：CURVE_GetCount()
* 功能：取出平滑参数Ndiv+1的值。
* 入口参数：cl		要操作的曲线对象，CURVE结构
* 出口参数：返回Ndiv+1的值
* 说明：若Ndiv为0，则先设置其为1。
****************************************************************************/
int  CURVE_GetCount(CURVE *cl)
{  if(0 == cl->Ndiv) cl->Ndiv=1;

   return(cl->Ndiv+1);
}


/****************************************************************************
* 名称：CURVE_GetCurve()
* 功能：增加曲线点到points。
* 入口参数：cl			要操作的曲线对象，CURVE结构
*          x			新增点的x坐标值
*		   y 			新增点的y坐标值
*		   points		曲线点缓冲区
*          PointCount	缓冲区的当前指针
* 出口参数：无
* 说明：
****************************************************************************/
void  CURVE_GetCurve(CURVE *cl, float x, float y, PointXY points[], int *PointCount)
{  int    X, Y;
   float  t, f, g, h;
   int    i;

   if(cl->Ndiv==0)  cl->Ndiv = 1;

    /* 新增一个点到结构 */
	X = (int)x; 
	Y = (int)y;
	points[*PointCount].x = X;
	points[*PointCount].y = Y;
	(*PointCount)++;

    /* 变换出ndiv个点 */
	for(i=1; i<=cl->Ndiv; i++)
	{  t = 1.0f / (float)cl->Ndiv * (float)i;
	   f = t * t * (3.0f - 2.0f * t);
	   g = t * (t - 1.0f) * (t-1.0f);
	   h = t * t * (t-1.0f);
		
	   X = (int)(x + cl->Ax*f + cl->Bx*g + cl->Cx*h);
	   Y = (int)(y + cl->Ay*f + cl->By*g + cl->Cy*h);
		
	   points[*PointCount].x = X;
	   points[*PointCount].y = Y;
	   (*PointCount)++;
	}
}



/***************************************************************************/


/****************************************************************************
* 名称：SPLINE_Spline()
* 功能：样条曲线初始化函数，将特征点输入到样条曲线对象中。
* 入口参数：sl			要操作的样条曲线对象，SPLINE结构
*		   pt			特征点数据		
*          np			特征点个数
* 出口参数：无
* 说明：
****************************************************************************/
void  SPLINE_SetSpline(SPLINE *sl, PointXY pt[], int np)
{  int  i;

   sl->Np = np;

   /* 将点数据复制到sl对象 */
   for(i=0; i<sl->Np; i++) 
   {  sl->Px[i] = (float)pt[i].x;  
	  sl->Py[i] = (float)pt[i].y;
   }
}



/****************************************************************************
* 名称：SPLINE_MatrixSolve()
* 功能：求解矩阵。
* 入口参数：sl		要操作的样条曲线对象，SPLINE结构
*          B		需操作B点数据(数组)
* 出口参数：无
* 说明：由SPLINE_Generate()调用，计算值保存在B返回。
****************************************************************************/
void  SPLINE_MatrixSolve(SPLINE *sl, float B[]) 
{  float  Work[NPMAX];
   float  WorkB[NPMAX];
   int    i, j;
		
   for(i=0; i<=(sl->Np-1); i++) 
   {  Work[i] = B[i] / sl->Mat[1][i];
	  WorkB[i] = Work[i];
   }

   for(j=0; j<10; j++) 
   {  Work[0] = (B[0] - sl->Mat[2][0] * WorkB[1]) / sl->Mat[1][0];
	  for(i=1; i<(sl->Np-1); i++ ) 
  	  {  Work[i] = (B[i] - sl->Mat[0][i] * WorkB[i-1] - sl->Mat[2][i] * WorkB[i+1]) / sl->Mat[1][i];
	  }
	  Work[sl->Np-1] = (B[sl->Np-1] - sl->Mat[0][sl->Np-1] * WorkB[sl->Np-2]) / sl->Mat[1][sl->Np-1];

	  for(i=0; i<=(sl->Np-1); i++) 
	  {  WorkB[i] = Work[i];
	  }
   }
   
   for(i=0; i<=(sl->Np-1); i++) 
   {  B[i] = Work[i];
   }
   
}



/****************************************************************************
* 名称：SPLINE_Generate()
* 功能：产生样条曲线图形。产生的各个端点保存到sl的结构内。
* 入口参数：sl		要操作的样条曲线对象，SPLINE结构
* 出口参数：无
* 说明：sl要先使用SPLINE_SetSpline()设置各个特征点。
****************************************************************************/
void  SPLINE_Generate(SPLINE *sl) 
{  float  k[NPMAX];
   float  AMag , AMagOld;
   int    i;

   /* 设置A点值 */
   for(i=0 ; i<=(sl->Np-2); i++ ) 
   {  sl->Ax[i] = sl->Px[i+1] - sl->Px[i];
	  sl->Ay[i] = sl->Py[i+1] - sl->Py[i];
   }
		
   /* 计算k变量 */
   AMagOld = (float)sqrt(sl->Ax[0] * sl->Ax[0] + sl->Ay[0] * sl->Ay[0]);
   for(i=0 ; i<=(sl->Np-3); i++) 
   {  AMag = (float)sqrt(sl->Ax[i+1] * sl->Ax[i+1] + sl->Ay[i+1] * sl->Ay[i+1]);
	  k[i] = AMagOld / AMag;
	  AMagOld = AMag;
   }
   k[sl->Np-2] = 1.0f;

   /* 矩阵计算 */
   for(i=1; i<=(sl->Np-2); i++) 
   {  sl->Mat[0][i] = 1.0f;
	  sl->Mat[1][i] = 2.0f * k[i-1] * (1.0f + k[i-1]);
	  sl->Mat[2][i] = k[i-1] * k[i-1] * k[i];
   }
   sl->Mat[1][0] = 2.0f;
   sl->Mat[2][0] = k[0];
   sl->Mat[0][sl->Np-1] = 1.0f;
   sl->Mat[1][sl->Np-1] = 2.0f * k[sl->Np-2];
 
   for(i=1; i<=(sl->Np-2);i++) 
   {  sl->Bx[i] = 3.0f*(sl->Ax[i-1] + k[i-1] * k[i-1] * sl->Ax[i]);
	  sl->By[i] = 3.0f*(sl->Ay[i-1] + k[i-1] * k[i-1] * sl->Ay[i]);
   }
   sl->Bx[0] = 3.0f * sl->Ax[0];
   sl->By[0] = 3.0f * sl->Ay[0];
   sl->Bx[sl->Np-1] = 3.0f * sl->Ax[sl->Np-2];
   sl->By[sl->Np-1] = 3.0f * sl->Ay[sl->Np-2];

   SPLINE_MatrixSolve(sl, sl->Bx);
   SPLINE_MatrixSolve(sl, sl->By);

   for(i=0 ; i<=sl->Np-2 ; i++ ) 
   {  sl->Cx[i] = k[i] * sl->Bx[i+1];
	  sl->Cy[i] = k[i] * sl->By[i+1];
   }
}





/****************************************************************************
* 名称：SPLINE_GetCurveCount()
* 功能：取得样条曲线上端点个数值。
* 入口参数：sl		要操作的样条曲线对象，SPLINE结构
* 出口参数：返回值即是样条曲线各个端点个数
* 说明：sl要先使用SPLINE_SetSpline()设置各个特征点。
****************************************************************************/
int  SPLINE_GetCurveCount(SPLINE *sl)
{  CURVE  curve_bak;
   int    count = 0;
   int    i;
   
   for(i=0; i<(sl->Np - 1); i++)	// 历遍所有特征点 
   {  /* 设置样条曲线(三点)curve_bak */
      CURVE_SetCurve(&curve_bak, 
                     sl->Ax[i], sl->Ay[i], 
                     sl->Bx[i], sl->By[i], 
                     sl->Cx[i], sl->Cy[i]);
      /* 取得曲线curve_bak上端点的个数 */
	  count += CURVE_GetCount(&curve_bak);
   }
   
   return(count);
}



/****************************************************************************
* 名称：SPLINE_GetCurve()
* 功能：取出样条曲线上各个端点，以便于使用GUI_Line()函数画出曲线。
* 入口参数：sl			要操作的样条曲线对象，SPLINE结构
*          points		用于接收曲线点的缓冲区，PointXY数组
*          PointCount	用于接收曲线点个数的指针
* 出口参数：无
* 说明：接收曲线点由points返回，曲线点个数由PointCount返回；
*      sl要先使用SPLINE_SetSpline()设置各个特征点。
****************************************************************************/
void  SPLINE_GetCurve(SPLINE *sl, PointXY points[], int *PointCount)
{  CURVE  curve_bak;
   int    i;

   *PointCount = 0;						// 初始化点计数器为0
   for(i=0; i<(sl->Np-1); i++) 			// 历遍所有特征点
   {  /* 设置样条曲线(三点)curve_bak */
   	  CURVE_SetCurve(&curve_bak, 
   					 sl->Ax[i], sl->Ay[i],
                     sl->Bx[i], sl->By[i], 
                     sl->Cx[i], sl->Cy[i]);
                     
      /* 取得样条曲线curve_bak的端点数据及个数 */               
	  CURVE_GetCurve(&curve_bak, 
	                 sl->Px[i], sl->Py[i], 
	                 points, 
	                 PointCount);
   }
}


/****************************************************************************
* 名称：GUI_Spline()
* 功能：作出多个特征点(3个以上)样条曲线。
* 入口参数points		特征点数组
*          no			特征点的个数
* 出口参数：无
****************************************************************************/
void  GUI_Spline(PointXY points[], int no, TCOLOR color)
{  SPLINE   sl;
   PointXY  sl_points[NPMAX * (int)DIV_FACTOR];
   
   if( (no<3)||(no>NPMAX) ) return;
   
   SPLINE_SetSpline(&sl, points, no);
   SPLINE_Generate(&sl);
   SPLINE_GetCurve(&sl, sl_points, &no);

   GUI_LineS((uint32 *)sl_points, no, color);
}



	









