/************************************************************
杭州中科微电子有限公司，参考例程（仅供参考！！！）
AGNSS使用参考方案（针对短时间内经常重启北斗模块的应用），该方案的源代码仅供参考！客户根据自己的需求可以修改代码！

1）应用终端定时向AGNSS服务器请求数据。只请求星历数据就可以。

应用终端向AGNSS服务器请求数据的数据格式示例:
cmd=eph;user=xxxxx;pwd=xxxxxx;lat=47.28;lon=8.56;pacc=1000;
请求模式为eph, 则AGNSS服务器仅提供星历, UTC和电离层信息。

从AGNSS服务器中获得星历数据包（暂称为EPH数据包），该数据包的有效期2小时。

2）应用终端通过GSM模块（或者其他无线通信）获取粗略时间和位置
注意：例程中用到的时间是UTC时间，北京时间需要减去8小时。
一般通过GSM模块获取基站的CELL ID，需要转换为纬度和经度信息。

特别的，如果应用终端无法获取正确的UTC时间，可以把时间标志设为无效（DATETIME_STR结构体中的valid字段）
如果应用终端无法获取正确的位置，也可以把位置标志设为无效（POS_LLA_STR结构体中的valid字段）

应用终端自己构建的初始信息数据包（暂称为INI数据包），该数据包的有效期（仅当前有效）。

3）依次发送INI数据包和EPH数据包。
INI数据包：每次重启北斗模块，都重构该数据包，并且发送给北斗模块。
EPH数据包：定时从AGNSS获取，每次重启北斗模块，发送EPH数据包。

注意：为了更好的性能，先发送INI数据包。

4）如果辅助的时间或者位置出错。会影响北斗模块的定位速度。
正确的辅助信息（最佳性能）；
无效的辅助信息（一般性能）；
错误的辅助信息（最差性能）。

5）本例程中实现了构建INI数据包。（只实现了主要的功能，仅供参考）
具体的AID_INI语句格式，参考CASIC协议手册。

*************************************************************/

#ifndef CASIC_AGNSS_AIDINI_H
#define CASIC_AGNSS_AIDINI_H

// 位置结构体
typedef struct
{
	double 							lat;		// 纬度，正数表示北纬，负数表示南纬
	double 							lon;		// 经度，正数表示东经，负数表示西经
	double 							alt;		// 高度，如果高度无法获取，可以设置为0
	int								valid;

} POS_LLA_STR;

// 时间结构体(注意：这里是UTC时间！！！与北京时间有8个小时的差距，不要直接使用北京时间！！！)
// 比如北京时间2016.5.8,10:34:23，那么UTC时间应该是2016.5.8,02:34:23
// 比如北京时间2016.5.8,03:34:23，那么UTC时间应该是2016.5.7,19:34:23
typedef struct
{
	int								valid;		// 时间有效标志，1=有效，否则无效
	int								year;
	int								month;
	int								day;
	int								hour;
	int								minute;
	int								second;
	float							ms;

} DATETIME_STR;

// 辅助信息（位置，时间，频率）
typedef struct  
{
	double							xOrLat, yOrLon, zOrAlt;
	double							tow;
	float							df;
	float							posAcc;
	float							tAcc;
	float							fAcc;
	unsigned int					res;
	unsigned short int				wn;
	unsigned char					timeSource;
	unsigned char					flags;

} AID_INI_STR;

/*************************************************************************
函数名称：casicAgnssAidIni
函数功能：把辅助位置和辅助时间打包成专用的数据格式。二进制信息格式化，并输出
函数输入：dateTime，日期与时间，包括有效标志（1有效）
		  lla, 经纬度标志，包括有效标志（1有效）
函数输出：aidIniMsg[66]，字符数组，辅助信息数据包，长度固定
*************************************************************************/
void casicAgnssAidIni(DATETIME_STR dateTime, POS_LLA_STR lla, char aidIniMsg[66]);

#endif
