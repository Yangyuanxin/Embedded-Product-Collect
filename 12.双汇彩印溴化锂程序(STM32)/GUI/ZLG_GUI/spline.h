/****************************************************************************************
* 文件名：SPLINE.H
* 功能：二次参数样条曲线生成程序。头文件
* 作者：黄绍斌
* 日期：2003.09.09
****************************************************************************************/
#ifndef _SPLINE_H
#define _SPLINE_H

#ifndef _SPLINE_C
#define EXT_SPLINE extern
#else 
#define EXT_SPLINE
#endif

/* 样条曲线类变量及相关函数 */
#define  NPMAX				10			/* 最大特征点个数 */
		
typedef  struct
{  float	Px[NPMAX];			
   float	Py[NPMAX];
 
   float	Ax[NPMAX];		// 定义指向A、B、C三点的指针
   float	Ay[NPMAX];
   float	Bx[NPMAX];
   float	By[NPMAX];
   float	Cx[NPMAX];
   float	Cy[NPMAX];

   float	Mat[3][NPMAX];

   int  	Np; 			// 曲线点个数
} SPLINE; 

/* 曲线类变量及相关函数 */
typedef  struct
{  float  	Ax, Ay;			// 定义A、B、C点坐标变量
   float  	Bx, By;
   float  	Cx, Cy;
   
   int    	Ndiv;			// 定义曲线平滑参数变量     
}CURVE; 

/****************************************************************************
* 名称：SPLINE_Spline()
* 功能：样条曲线初始化函数，将特征点输入到样条曲线对象中。
* 入口参数：sl			要操作的样条曲线对象，SPLINE结构
*		   pt			特征点数据		
*          np			特征点个数
* 出口参数：无
* 说明：
****************************************************************************/
EXT_SPLINE  void  SPLINE_SetSpline(SPLINE *sl, PointXY pt[], int np);


/****************************************************************************
* 名称：SPLINE_Generate()
* 功能：产生样条曲线图形。产生的各个端点保存到sl的结构内。
* 入口参数：sl		要操作的样条曲线对象，SPLINE结构
* 出口参数：无
* 说明：sl要先使用SPLINE_SetSpline()设置各个特征点。
****************************************************************************/
EXT_SPLINE  void  SPLINE_Generate(SPLINE *sl);


/****************************************************************************
* 名称：SPLINE_GetCurveCount()
* 功能：取得样条曲线上端点个数值。
* 入口参数：sl		要操作的样条曲线对象，SPLINE结构
* 出口参数：返回值即是样条曲线各个端点个数
* 说明：sl要先使用SPLINE_SetSpline()设置各个特征点。
****************************************************************************/
EXT_SPLINE  int  SPLINE_GetCurveCount(SPLINE *sl);


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
EXT_SPLINE  void  SPLINE_GetCurve(SPLINE *sl, PointXY points[], int *PointCount);

/****************************************************************************
* 名称：GUI_Spline()
* 功能：作出多个特征点(3个以上)样条曲线。
* 入口参数points		特征点数组
*          no			特征点的个数
* 出口参数：无
****************************************************************************/
EXT_SPLINE  void  GUI_Spline(PointXY points[], int no, TCOLOR color);


#endif