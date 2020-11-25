/****************************************************************************************
* 文件名：GUI_CONFIG.H
* 功能：用于裁剪GUI代码。
* 作者：黄绍斌
* 日期：2004.03.11
* 备注：将宏设置为1时使能相应功能函数，为0或其它值时禁止。
****************************************************************************************/
#ifndef  GUI_CONFIG_H
#define  GUI_CONFIG_H


/* 有宽度的直线函数 */
#define   GUI_LineWith_EN		1
/* 画圆函数 */
#define   GUI_CircleX_EN		1
/* 椭圆函数 */
#define   GUI_EllipseX_EN		0
/* 填充函数 */
#define   GUI_FloodFill_EN		1
	/* 向上及向下节点个数(填充函数) */
	#define   DOWNP_N			20
	#define   UPP_N				20
/* 圆弧函数 */
#define   GUI_ArcX_EN			0
/* 扇形函数 */
#define   GUI_Pieslice_EN		0



/* 窗口管理 */
#define   GUI_WINDOW_EN			1



/* 图标菜单 */
#define  GUI_MenuIco_EN			1
	

/* 下拉菜单 */
#define  GUI_MenuDown_EN		1



/* 5*7字体 */
#define  FONT5x7_EN				1


/* 8*8字体 */
#define  FONT8x8_EN				1


/* 16*16字体 */
#define  FONT16x16_EN			1


/* 24*32字体 */
#define  FONT24x32_EN			0


/* 汉字显示函数 */
#define  GUI_PutHZ_EN			1


/* 单色图形显示函数 */
#define	 GUI_LoadPic_EN			1


/* 颜色转换 */
#define  CONVERTCOLOR_EN		1


#endif
