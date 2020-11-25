/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        nmea.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-19
 * Description:      GPS标准协议NMEA-0183以及MTK、泰斗、中科微自定义协议实现《The NMEA 0183 Protocol.pdf》
 * Others:           
 * Function List:    
    1. 
    
 * History: 
    1. Date:         2019-03-19
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */


#ifndef __NMEA_H__
#define __NMEA_H__

#include <time.h>
#include <gm_time.h>
#include "gm_type.h"

#define SENTENCE_MAX_LENGTH 80

//小端模式:高位代表高地址,内存中是buf[0]=0x23,buf[1]=0x3E,下面的也一样
#define TD_SENTENCE_HEAD 0x3E23
#define TD_SENTENCE_TAIL 0x0A
#define TD_AID_TIME_CMD 0x0204
#define TD_AID_POS_CMD 0x0104
#define TD_AID_BDSEPH_CMD 0x1104
#define TD_AID_BDSALM_CMD 0x1204
#define TD_AID_GPSEPH_CMD 0x2104
#define TD_AID_GPSALM_CMD 0x2204
#define TD_AID_ACK_CMD 0x0101
#define TD_AID_NACK_CMD 0x0201
#define TD_AID_VER_CMD 0x0105
#define TD_OPEN_FUNC_CMD 0x5103


#define AT_SENTENCE_HEAD 0xCEBA
#define AT_AID_TIMEPOS_CMD 0x010B
#define AT_AID_BDSEPH_CMD 0x0208
#define AT_AID_GPSEPH_CMD 0x0708
#define AT_AID_GLNEPH_CMD 0x0808
#define AT_AID_ACK_CMD 0x0105
#define AT_AID_NACK_CMD 0x0005
#define AT_AID_VER_CMD 0x040A


typedef enum 
{
    NMEA_INVALID = -1,
    NMEA_UNKNOWN = 0,
    NMEA_SENTENCE_TXT = 1,        // AT 文本
    NMEA_SENTENCE_INF = 2,        // TD 文本
    NMEA_SENTENCE_MTK_START = 3,  // MTK GPS work OK,可以发送EPO
    NMEA_SENTENCE_MTK_ACK = 4,    // MTK AGPS ACK,确认包
    NMEA_SENTENCE_MTK_VER = 5,    // MTK GPS version,版本号
    NMEA_SENTENCE_TD_ACK = 6,     // 泰斗 AGPS ACK,确认包
    NMEA_SENTENCE_TD_VER = 7,     // 泰斗 GPS version,版本号
	NMEA_SENTENCE_AT_ACK = 8,	  // 中科微 AGPS ACK,确认包
	NMEA_SENTENCE_AT_NACK = 9,	  // 中科微 AGPS ACK,确认包
	NMEA_SENTENCE_AT_VER = 10,    // 中科微 GPS version,版本号
    NMEA_SENTENCE_RMC = 11,       // 标准NMEA-0183协议RMC语句
    NMEA_SENTENCE_GGA = 12,       // 标准NMEA-0183协议GGA语句
    NMEA_SENTENCE_GSA = 13,		  // 标准NMEA-0183协议GSA语句
    NMEA_SENTENCE_GLL = 14,		  // 标准NMEA-0183协议GLL语句
    NMEA_SENTENCE_GST = 15,       // 标准NMEA-0183协议GST语句
    NMEA_SENTENCE_GSV = 16,       // 标准NMEA-0183协议GSV语句
    NMEA_SENTENCE_VTG = 17,       // 标准NMEA-0183协议VTG语句
    NMEA_SENTENCE_ZDA = 18,       // 标准NMEA-0183协议ZDA语句
    NMEA_SENTENCE_ACCURACY = 19,  // MTK精度语句
    NMEA_SENTENCE_BDGSV = 20,     // 标准NMEA-0183协议GSV语句
}NMEASentenceID;

typedef struct 
{
    S32 value;
    S32 scale;
}NMEAFloat;

typedef struct  
{
    S32 day;
    S32 month;
    S32 year;
}NMEADate;

typedef struct 
{
    S32 hours;
    S32 minutes;
    S32 seconds;
    S32 microseconds;
}NMEATime;

typedef struct 
{
	char ver[64];
	char release_str[12];
	U32 build_id;
}NMEASentenceVER;

typedef struct 
{
	S32 system_message_type;
}NMEASentenceStart;


//$GNRMC,081347.00,A,2232.51905,N,11357.10992,E,2.353,,190319,,,A,V*12
typedef struct 
{
    NMEATime time;
	
    bool valid;
	
    NMEAFloat latitude;
	
    NMEAFloat longitude;
	
	//速度（单位knots（节））
    NMEAFloat speed;

	//方位角（单位度°）
    NMEAFloat course;

	//日期
    NMEADate date;
	
    NMEAFloat variation;
}NMEASentenceRMC;

//Global Positioning System Fix Data.
//$GNGGA,081347.00,2232.51905,N,11357.10992,E,1,08,1.13,-121.3,M,-2.4,M,,*7C
typedef struct  
{
    NMEATime time;
	
    NMEAFloat latitude;
	
    NMEAFloat longitude;

	//0——未定位；1——已定位；2——差分定位
    S32 fix_quality;

	//可见卫星数0-12
    S32 satellites_tracked;

	//水平分量精度因子:为纬度和经度等误差平方和的开根号值（[0.5--99.9]数值越小精度越高）
	//直接反映GPS卫星的分布情况,当值较大时,表明空中的4颗GPS卫星几何分布不是太理想,他们构成的图形周长太短,定位精度就低,反之亦然。
    NMEAFloat hdop;

	//天线高度（相对于平均海平面）
    NMEAFloat altitude; 

	//天线高度单位,‘M’为M
	char altitude_units;
	
	//Geoidal separation, the difference between the WGS-84 earth ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
    NMEAFloat height; 
	
	char height_units;
	
	//Age of differential GPS data, time in seconds since last SC104 type 1 or 9 update, null field when DGPS is not used
    NMEAFloat dgps_age;
}NMEASentenceGGA;

typedef enum 
{
    NMEA_GLL_STATUS_DATA_VALID = 'A',
    NMEA_GLL_STATUS_DATA_NOT_VALID = 'V',
}NMEAGLLStatus;

// FAA mode added to some fields in NMEA 2.3.
typedef enum 
{
    NMEA_FAA_MODE_AUTONOMOUS = 'A',
    NMEA_FAA_MODE_DIFFERENTIAL = 'D',
    NMEA_FAA_MODE_ESTIMATED = 'E',
    NMEA_FAA_MODE_MANUAL = 'M',
    NMEA_FAA_MODE_SIMULATED = 'S',
    NMEA_FAA_MODE_NOT_VALID = 'N',
    NMEA_FAA_MODE_PRECISE = 'P',
}NMEAFAAMode;

//Geographic Position – Latitude/Longitude
typedef struct
{
    NMEAFloat latitude;
    NMEAFloat longitude;
    NMEATime time;
    char status;
    char mode;
}NMEASentenceGLL;

//GPS Pseudorange Noise Statistics
typedef struct
{
    NMEATime time;
    NMEAFloat rms_deviation;
    NMEAFloat semi_major_deviation;
    NMEAFloat semi_minor_deviation;
    NMEAFloat semi_major_orientation;
    NMEAFloat latitude_error_deviation;
    NMEAFloat longitude_error_deviation;
    NMEAFloat altitude_error_deviation;
}NMEASentenceGST;

typedef enum
{
    NMEA_GPGSA_MODE_AUTO = 'A',
    NMEA_GPGSA_MODE_FORCED = 'M',
}NMEAGSAMode;

typedef enum
{
    NMEA_GPGSA_FIX_NONE = 1,
    NMEA_GPGSA_FIX_2D = 2,
    NMEA_GPGSA_FIX_3D = 3,
}NMEAGSAFixType;

//GPS DOP and active satellites
typedef struct
{
    char mode;
    S32 fix_type;
    S32 satellites[12];
    NMEAFloat pdop;
    NMEAFloat hdop;
    NMEAFloat vdop;
}NMEASentenceGSA;

typedef struct
{
    S32 nr;
	//海拔
    S32 elevation;
	
	//地平经度，方位角(用以找出恒星、行星等的方位);
    S32 azimuth;

	//SNR in dB
    S32 snr;
}NMEASatelliteInfo;

//Satellites in view
typedef struct
{
    S32 total_msgs;
    S32 msg_number;
    S32 total_satellites;
    NMEASatelliteInfo satellites[4];
}NMEASentenceGSV;

//Track Made Good and Ground Speed
typedef struct
{
    NMEAFloat true_track_degrees;
    NMEAFloat magnetic_track_degrees;
    NMEAFloat speed_knots;
    NMEAFloat speed_kph;
    NMEAFAAMode faa_mode;
}NMEASentenceVTG;

//Time & Date – UTC, Day, Month, Year and Local Time Zone
typedef struct
{
    NMEATime time;
    NMEADate date; 
    S32 hour_offset;
    S32 minute_offset;
}NMEASentenceZDA;

typedef enum 
{
	MTK_TIME_ACK = 740,
	MTK_POS_ACK = 713,
	MTK_DATA_ACK = 721,
	MTK_TTFF_ACK = 257,
}MTKAckType;

typedef enum
{
	MTK_INVALID_CMD = 0,     //Invalid command / packet.
	MTK_UNSUPPORTED_CMD = 1, //Unsupported command / packet type
	MTK_ACTION_FAILED = 2,   //Valid command / packet, but action failed
	MTK_ACTION_SUCCEED = 3,  //Valid command / packet, and action succeeded
}MTKAckResult;

typedef struct
{
    MTKAckType ack_type;
	MTKAckResult ack_result;
}NMEASentenceMTKACK;




/**
 * Function:   确定语句
 * Description:
 * Input:	   p_sentence:语句；strict——是否严格模式
 * Output:	   语句类型
 * Return:	   true——有效；false——无效
 * Others:	   
 */
NMEASentenceID nmea_sentence_id(const char* p_sentence, const U16 len, bool strict);

/**
 * Function:   解析特定类型的语句
 * Description:
 * Input:	   p_sentence:二进制形式的语句
 * Output:	   p_frame:结构体形式的语句内容
 * Return:	   true——成功；false——失败
 * Others:	   			   
 */

bool nmea_parse_txt(NMEASentenceVER* p_frame, const char* p_sentence);
bool nmea_parse_inf(NMEASentenceVER* p_frame, const char* p_sentence);
bool nmea_parse_rmc(NMEASentenceRMC* p_frame, const char* p_sentence);
bool nmea_parse_gga(NMEASentenceGGA* p_frame, const char* p_sentence);
bool nmea_parse_gsa(NMEASentenceGSA* p_frame, const char* p_sentence);
bool nmea_parse_gll(NMEASentenceGLL* p_frame, const char* p_sentence);
bool nmea_parse_gst(NMEASentenceGST* p_frame, const char* p_sentence);
bool nmea_parse_gsv(NMEASentenceGSV* p_frame, const char* p_sentence);
bool nmea_parse_vtg(NMEASentenceVTG* p_frame, const char* p_sentence);
bool nmea_parse_zda(NMEASentenceZDA* p_frame, const char* p_sentence);
bool nmea_parse_mtk_ack(NMEASentenceMTKACK* p_frame, const char* p_sentence);
bool nmea_parse_mtk_ver(NMEASentenceVER* p_frame, const char* p_sentence);
bool nmea_parse_mtk_start(NMEASentenceStart* p_frame, const char* p_sentence);
bool nmea_parse_td_ack(U16* p_cmd, const char* p_sentence, const U16 len);
bool nmea_parse_td_ver(NMEASentenceVER* p_frame, const char* p_sentence, const U16 len);
bool nmea_parse_at_ack(U16* p_cmd, const char* p_sentence, const U16 len);
bool nmea_parse_at_ver(NMEASentenceVER* p_frame, const char* p_sentence, const U16 len);


/**
 * Function:   创建内置（MTK）一般语句
 * Description:
 * Input:	   cmd：命令；p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
void nmea_create_common_mtk_sentence(const char* cmd,U8* p_sentence);

/**
 * Function:   创建内置（MTK）查询版本号语句
 * Description:
 * Input:	   p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_mtk_qeury_version_sentence(U8* p_sentence, U8* p_len);

/**
 * Function:   创建内置（MTK）辅助时间语句
 * Description:
 * Input:	   st_time:UTC时间（注意不是本地时间）；p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   无
 * Others: 	
 */
bool nmea_creat_mtk_aid_time_sentence(const ST_Time st_time, U8* p_sentence, U8* p_len);

/**
 * Function:   创建内置（MTK）辅助位置语句
 * Description:
 * Input:	   ref_lat:辅助位置纬度；ref_lng:辅助位置经度；p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_mtk_aid_pos_sentence(const float ref_lat, const float ref_lng, U8* p_sentence, U8* p_len);

/**
 * Function:   创建内置（MTK）EPO数据语句
 * Description:
 * Input:	   seg_index:分片ID；p_data:数据分片指针；data_len:数据长度；p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_mtk_epo_sentence(const U16 seg_index, const U8* p_data, const U16 data_len, U8* p_sentence, U16* p_len);

/**
 * Function:   创建泰斗（TD）查询版本号语句
 * Description:
 * Input:	   p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_td_qeury_version_sentence(U8* p_sentence, U8* p_len);

/**
 * Function:   创建泰斗（TD）辅助时间语句
 * Description:
 * Input:	   st_time:UTC时间（注意不是本地时间）；leap_sencond——闰秒值；p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_td_aid_time_sentence(const ST_Time st_time, const U8 leap_sencond, U8* p_sentence, U8* p_len);

/**
 * Function:   创建泰斗（TD）辅助位置语句
 * Description:
 * Input:	   ref_lat:辅助位置纬度；ref_lng:辅助位置经度；p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_td_aid_pos_sentence(const float ref_lat, const float ref_lng, U8* p_sentence, U8* p_len);

/**
 * Function:   创建泰斗（TD）打开VTG语句
 * Description:
 * Input:	   p_len:最大长度
 * Output:	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return:	   true——成功；false——失败
 * Others:	   
 */
bool nmea_creat_td_open_vtg_sentence( U8* p_sentence, U8* p_len);


/**
 * Function:   创建中科微（AT）查询版本号语句
 * Description:
 * Input:	   p_len:最大长度
 * Output: 	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return: 	   true——成功；false——失败
 * Others: 	
 */
bool nmea_creat_at_qeury_version_sentence(U8* p_sentence, U8* p_len);


/**
 * Function:   创建中科微（AT）辅助信息语句
 * Description:把辅助位置和辅助时间打包成专用的二进制数据格式
 * Input:	   st_time:UTC时间（注意不是本地时间）；leap_sencond——闰秒值；ref_lat:辅助位置纬度；ref_lng:辅助位置经度；p_len:最大长度
 * Output:	   p_sentence:输出的辅助信息语句;p_len:语句长度
 * Return:	   true——成功；false——失败
 * Others:	   
 */
bool nmea_creat_at_aid_info_sentence(const ST_Time st_time,const U8 leap_sencond, const float ref_lat, const float ref_lng, U8* p_sentence, U8* p_len);

/**
 * Function:   把GPS日期+时间转换为UNIX时间戳
 * Description:
 * Input:	   p_date:GPS日期；p_time:GPS时间
 * Output:	   无
 * Return:	   UNIX时间戳
 * Others:	   			   
 */
time_t nmea_get_utc_time(const NMEADate* p_date, const NMEATime* p_time);

/**
 * Function:   改变分母
 * Description:
 * Input:	   p_value:分数；new_scale:新的分母
 * Output:	   无
 * Return:	   新的分子
 * Others:				   
 */
S32 nmea_rescale(const NMEAFloat* p_fraction, S32 new_scale);

/**
 * Function:   分数转换成浮点数
 * Description:
 * Input:	   p_fraction:分数
 * Output:	   无
 * Return:	   转换后的浮点数
 * Others:				   
 */
float nmea_tofloat(const NMEAFloat* p_fraction);

/**
 * Function:   将原生经纬度转换成浮点经纬度:XX.YYY——XX度YYY分
 * Description:
 * Input:	   p_fraction:分数
 * Output:	   无
 * Return:	   浮点左标
 * Others:				   
 */
float nmea_tocoord(const NMEAFloat *p_fraction);



#endif

