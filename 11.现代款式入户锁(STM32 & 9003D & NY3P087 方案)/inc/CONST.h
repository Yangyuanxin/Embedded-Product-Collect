#ifndef __CONST_H__
#define __CONST_H__

/*******************************************************************************/
#define	DUGARCH
//#undef	DUGARCH

#define	DUGTMR
//#undef	DUGTMR

#define	DUGFPRINT
//#undef	DUGFPRINT

//#define	CNOBEEP	0
#define	CENABLEVOP	1	//用语音IC取代蜂鸣器

#define	CSETOP_asterisk_pound	1		//set键进入管理操作方式

#define	CVOP_GQPP5
//#undef	CVOP_GQPP5
//#define	CVOP_WT588D
//#undef	CVOP_WT588D
/*******************************************************************************
 *
 * All constant define here
 *
 *******************************************************************************/
#define	STACKSIZE	8
#define	QUEUESIZE	8

#define	OK	0
#define	ERROR	-1

#define	TRUE	1
#define	FALSE	0

#define CSEND_BUF_NUM	8	//16	 //缓冲区个数
#define CSEND_BUF_SIZE	128	//64	 //缓冲区大小

#define TIMER_NUM	3       /** 定时器个数(第二个只用于sysProcess中) **/
#define TIMER_0 	0		/** 8kHz定时, 0等于关闭时钟 **/
#define TIMER_10MS 80		/** 8kHz定时, 10ms计数次数 **/
#define TIMER_20MS 160		/** 8kHz定时, 20ms计数次数 **/
#define TIMER_60MS 480		/** 8kHz定时, 60ms计数次数 **/
#define TIMER_80MS 640		/** 8kHz定时, 80ms计数次数 **/
#define TIMER_100MS 800		/** 8kHz定时, 100ms计数次数 **/
#define TIMER_200MS 1600	/** 8kHz定时, 200ms计数次数 **/
#define TIMER_300MS 2400	/** 8kHz定时, 300ms计数次数 **/
#define TIMER_500MS 4000	/** 8kHz定时, 500ms计数次数 **/
#define TIMER_600MS 4800	/** 8kHz定时, 600ms计数次数 **/
#define TIMER_700MS 5600	/** 8kHz定时, 700ms计数次数 **/
#define TIMER_800MS 6400	/** 8kHz定时, 800ms计数次数 **/
#define TIMER_1SEC  8000	/** 8kHz定时, 每1秒计数次数 **/
#define TIMER_2SEC  16000	/** 8kHz定时, 每2秒计数次数 **/
#define TIMER_3SEC  24000	/** 8kHz定时, 每3秒计数次数 **/
#define TIMER_4SEC  32000	/** 8kHz定时, 每4秒计数次数 **/
#define TIMER_5SEC  40000	/** 8kHz定时, 每5秒计数次数 **/
#define TIMER_10SEC 80000	/** 8kHz定时, 10秒计数次数 **/
#define TIMER_CALIB 800000	/** 8kHz定时, 100秒超时退出 **/
#define TIMER_RGST  800		/** 注册时间间隔 **/

#define TIMER_VOPBUSY  800	/** 语音忙检测延时时间 **/
#define TIMER_VOPBUSYMAX  8000	/** 语音结束检测延时时间自增上限 **/
#define TIMER_VOPFAULTBEGIN  10	/** 语音结束检测异常时间自增上限 **/
#define TIMER_VOPFAULTEND  (TIMER_VOPFAULTBEGIN + 8000) 	/** 语音结束检测异常时间自增上限 **/

#define CTIMER_CALIB (16000)	/** 进入校正状态需要持续按下按键的时间, 斩定为2s **/

/** 按键 **/
#define	CKEY_1	(0x02)
#define	CKEY_2	(0x01)
#define	CKEY_3	(0x05)
#define	CKEY_4	(0x04)
#define	CKEY_5	(0x0c)
#define	CKEY_6	(0x09)
#define	CKEY_7	(0x08)
#define	CKEY_8	(0x0a)
#define	CKEY_9	(0x0b)
#define	CKEY_asterisk	(0x07)
#define	CKEY_0	(0x06)
#define	CKEY_pound	(0x03)

#define	CKEY_X1	(0x00)
#define	CKEY_X2	(0x0d)
#define	CKEY_X3	(0x0e)
#define	CKEY_X4	(0x0f)


#define	CKEY_GOOUT	(0x10)
#define	CKEY_GOIN	(0x11)
#define CKEY_SET 	(0x12)	//门后的设置键

//#define CTOKEY 16	//go out
//#define CTIKEY 17	//go in
//#define CSETKEY 18	//门后的设置键


#define CDEV_TYPE	2

#define CHARQUEUESIZE   128
#define CKEYBUFSIZE   12

#define ACTIONQUEUESIZE 16

#define X10QUEUESIZE   16

#define CANQUEUESIZE    (128 >> 2)

/*******************************************************************************/
#define FILTER_SID  (0x01)
#define FILTER_EID  (0x0)
#define FILTER_ID    ((FILTER_SID << 18) | (FILTER_EID))
//#define FILTER_ID   ((u32)((FILTER_SID << 18) | (FILTER_EID)) << 3) | CAN_ID_EXT | CAN_RTR_DATA)

#define MASK_SID    (0x7f)
#define MASK_EID    (0x0)
#define FILTER_MASK    ((MASK_SID << 18) | (MASK_EID))

/*******************************************************************************/
#define ASCII_STX   0x02
#define ASCII_ETX   0x03

/*******************************************************************************/
#define	CTOTALADDRESS	1024


/*******************************************************************************/

#define CFP_INIT					0
#define CFP_ENROLL     			2
#define CFP_MATCH     			3
#define CFP_DELETE    			4

//#define	CTOTALFINGER	58
#define	CTOTALFINGER	118	/** 指纹索引条目数, 也是指纹模板总数 **/
#define	C1STMANAGEE		20	/** 第一管理者，能添加的编号上限(减去1就是管理的总数) **/

	/** 第二管理者管理指纹的起始索引号(第一管理者的索引号) **/
#define	C2NDMANAGES 	(C1STMANAGEE + 1)
	/** 第二管理者管理指纹的结束索引号(取剩下的总数的一半，加上第一管理者的管理总数就得到索引号上限) **/
#define	C2NDMANAGEE 	(((CTOTALFINGER - C1STMANAGEE) >> 1) + C1STMANAGEE)

#define	C3RDMANAGES 	(C2NDMANAGEE + 1)	/** 第三管理者管理指纹的起始索引号(第二管理者的结尾索引号+1) **/
#define	C3RDMANAGEE 	(CTOTALFINGER - 1)	/** 第三管理者管理指纹的结束索引号(总索引数-1) **/


#define CADC1_DR_ADDRESS    ((u32)0x4001244C)

#define	CFLASHSTARTADDR (0x0801FC00)		/** the 127th Flash Page **/
//#define	CFLASHSTARTADDR (0x0801F800)	/** the 126th Flash Page **/

#define CMAX_PSERR 6		//最大密码失败次数
/*******************************************************************************/
#define	CFLASH_PAGE_SIZE		1024

#define	CFLASH_ADC_VALUE		4			/** 4..5 **/
//#define	CFLASH_ADC_FLAG			6
#define	CFLASH_RESET_VALUE		8
#define	CFLASH_INDUCTION_POWER		12		/** 指纹头感应上电标志A5/A0作为有或无，其它为无效也视作有感应上电标志 **/
		#define CINDUCTION_POWER_DISABLE	0xA0		/** 无指纹上电功能(默认有) **/
		#define CINDUCTION_POWER_ENABLE		0xA5		/** 有指纹上电功能(默认有) **/
#define	CFLASH_PASSWD_ERR		16		/** 输入错误密码次数 **/
#define	CFLASH_PASSWD_START		30
#define	CFLASH_FPIndex_START	130

#define	CENTRYFLAG_BUSY	0xa5	/** 表示指定的项已被使用 **/
#define	CENTRYFLAG_IDLE	0xff	/** 表示指定的项未被使用 **/

#define	MAGIC_SIZE 4
/*******************************************************************************/
//语音控制索引
#define	CVOPID_BASE	1
#define	CVOPID_1	(CVOPID_BASE + 1)
#define	CVOPID_2	(CVOPID_BASE + 2)
#define	CVOPID_3	(CVOPID_BASE + 3)
#define	CVOPID_4	(CVOPID_BASE + 4)
#define	CVOPID_5	(CVOPID_BASE + 5)
#define	CVOPID_6	(CVOPID_BASE + 6)
#define	CVOPID_7	(CVOPID_BASE + 7)
#define	CVOPID_8	(CVOPID_BASE + 8)
#define	CVOPID_9	(CVOPID_BASE + 9)
#define	CVOPID_asterisk	(CVOPID_BASE + 10)
#define	CVOPID_0	(CVOPID_BASE + 0)
#define	CVOPID_pound	(CVOPID_BASE + 11)

#define	CVOPID_EROLLPSWORDFORYOURSECURITY	(CVOPID_BASE + 12)
#define	CVOPID_BEEP			(CVOPID_BASE + 13)
#define	CVOPID_LBEEP		(CVOPID_BASE + 14)
#define	CVOPID_EROLL		(CVOPID_BASE + 15)	//录入
#define	CVOPID_ADD		(CVOPID_BASE + 16)		//添加
#define	CVOPID_DEL		(CVOPID_BASE + 17)		//删除
#define	CVOPID_ALL		(CVOPID_BASE + 18)		//全部
#define	CVOPID_FINGER		(CVOPID_BASE + 19)	//指纹
#define	CVOPID_PSWORD		(CVOPID_BASE + 20)	//密码
#define	CVOPID_PLEASEPRESS	(CVOPID_BASE + 21)	//请按
#define	CVOPID_SUCESS		(CVOPID_BASE + 22)	//成功
#define	CVOPID_FAILED		(CVOPID_BASE + 23)	//失败
#define	CVOPID_PASSED		(CVOPID_BASE + 24)	//通过
#define	CVOPID_CLEARED		(CVOPID_BASE + 25)	//已清空
#define	CVOPID_TIMEOUT		(CVOPID_BASE + 26)	//超时
#define	CVOPID_INVALID		(CVOPID_BASE + 27)	//无效
#define	CVOPID_RECOVERED	(CVOPID_BASE + 28)	//恢复出厂成功
#define	CVOPID_PLEASEEROLLAGAIN		(CVOPID_BASE + 29)	//请再录一次
#define	CVOPID_COMPARISON	(CVOPID_BASE + 30)	//验证(比对)
#define	CVOPID_SET			(CVOPID_BASE + 31)	//设置
#define	CVOPID_OK			(CVOPID_BASE + 32)	//请按把手开门
#define	CVOPID_PLEASEINPUTPSWORD		(CVOPID_BASE + 33)	//请输入密码
#define	CVOPID_PRESSFINGER		(CVOPID_BASE + 34)		//请按手指
#define	CVOPID_REGISTER		(CVOPID_BASE + 35)	//注册
#define	CVOPID_MANAGE		(CVOPID_BASE + 36)	//管理
#define	CVOPID_INPUT		(CVOPID_BASE + 37)	//输入
#define	CVOPID_CONFIRM		(CVOPID_BASE + 38)	//确认
#define	CVOPID_POWERLOW		(CVOPID_BASE + 39)	//电力不足，请更换电池或充电

#define	CVOPID_FINAL		(CVOPID_POWERLOW)	//结尾




#define	CVOP_VOL0		0xe0
//...
#define	CVOP_VOL7		0xe7
#define	CVOP_LOOP		0xf2	//play voice again and again
#define	CVOP_STOP		0x00	//stop playing

/*******************************************************************************/
#endif
