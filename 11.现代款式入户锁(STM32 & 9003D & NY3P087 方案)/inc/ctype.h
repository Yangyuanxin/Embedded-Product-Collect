#ifndef __PTYPE_H__
#define __PTYPE_H__

/*******************************************************************************
 *
 * All data type define here
 *
 *******************************************************************************/

typedef enum {
    FAILED = 0, 
    PASSED = !FAILED
} RetStatus;

typedef	int	(*pfunc_t)(unsigned *arg);

typedef struct func_s {
    pfunc_t		func;       /** function **/
    unsigned int  *arg;    /** argument **/
} func_t;

typedef struct func_stack{
	int		top;
	func_t		func[STACKSIZE]; 
}fstack_t;

typedef enum {
    CMSG_NONE = 0, 
    CMSG_INIT, 
    CMSG_TMR, 
    CMSG_DLAY, 	/** 时钟 **/
    CMSG_BACK,	/** 背光灯 **/
    CMSG_DKEY,
    CMSG_UKEY,
    
    //CMSG_FGON,	/** 检测到指纹头上有手指 **/
    CFIGER_ON,
    CFIGER_INIT,	/** 复位指纹头完成 **/
    CFIGER_ADD,		/** 注册指纹 **/
    CFIGER_DEL,		/** 删除指纹 **/
    CFIGER_TOUT,	/** 指纹头响应超时 **/
    CFIGER_FGPRPRD,	/** 完成准备 **/
    
    
    CMSG_FGINIT,	/** 初始化成功 **/
    CMSG_FGRGST,	/** 注册指纹成功 **/
    CMSG_FGQUERY,	/** 查找指纹成功 **/
    CMSG_FGDERG,	/** 注销指纹成功 **/
    CMSG_FGOPFAIL,	/** 指纹头操作失败 **/
    
	CNOKEY_ENTER,		/** 无钥匙进入 **/
	
    CMSG_COMTX,
    CMSG_COMRX,

	/**  **/
	//CVOP_STRT,		/** voice prompt start **/
	//CVOP_OVER,

	CACT_TOUT,		/** 动作超时 **/
	CACT_OVER,		/** 动作完成 **/
	
	CPMT_TOUT,		/** 提示动作超时 **/
	CPMT_OVER,		/** 提示动作完成 **/

    CMSG_ADC,		/** AD转换 **/
    CMSG_CALC,		/** 重新校验低电参照点 **/
    
    CUART1_RCV,
    CUART2_RCV,
    //CCAN_RCV,
    CMSG_COMTIMEOUT,
} msgType_t;

typedef struct msg_s {
	msgType_t msgType;
	int	msgValue;
}msg_t;

typedef struct msg_queue{
	int	head;
	int	tail;

	msgType_t	type[QUEUESIZE];
	//int	msgValue[QUEUESIZE];
	msg_t msg[QUEUESIZE];
} msgq_t;

typedef struct Timer_s {
    unsigned int tick_bak;
    unsigned int tick;
    msgType_t msgType;                      /** the type of msg when timeout **/
    //unsigned int count;                   /** Is it necessary ? **/
}Timer_t; 

typedef struct key_s {
	unsigned char buf[CKEYBUFSIZE];
	int iCount;
	int iPrivate;			/** 用户定义的私有数据 **/
}key_t;

typedef struct bitmap_s {
    unsigned int bitmap;		    /** 32 bits **/
} bitmap_t;

typedef enum {              /** 用于控制com数据的发送 **/
    CTRL_START = 0,         /** 0 - start transmission **/
    CTRL_CONTI = 1,         /** 1 - continue transmission  **/
                            /** 2 reserved. for other control **/
    CTRL_TMR = 5,           /** 3..200 for delay **/
    CTRL_STOP = 255,        /** 255 - stop transmission**/
} ctrlFlag_t;

typedef struct charData_s{
	unsigned char ucVal;
	ctrlFlag_t ctrl;         /** 控制字段，用于表达当前的data是有效数据还是延迟或其它功能 **/
} charData_t;

typedef struct charBuf_queue_s {
    int	head;
	int	tail;
    int len;
    unsigned char buf[CHARQUEUESIZE];     /** 128 bytes **/
    //ctrlFlag_t ctrl[CHARQUEUESIZE];     /** 128 bytes **/
} charBuf_queue_t;


typedef struct action_s {
	/*****************
	 * MOT-ON
	 * MOT-OFF
	 * LED-ON
	 * LED-OFF
	 * VOP-ON
	 * VOP-OFF
	 * Delay
	 *****************/
#define CACT_UNLOCK	0xff
#define CACT_LOCK	0xfe
#define CACT_LEDPMTON	0xfd
#define CACT_LEDPMTOFF	0xfc
#define CACT_LEDWRNON	0xfb
#define CACT_LEDWRNOFF	0xfa
#define CACT_VOPON	0xf9
//#define CACT_VOPOFF	0xf8
#define CACT_DELAY	0xf7

#define CACT_BEEPON	0xf6
#define CACT_BEEPOFF	0xf5

    unsigned int actionTime;	/** 持续时间 **/
    unsigned char actionType;	/** 动作类型 **/
    unsigned char actionPara;	/** 动作需要的其它参数，如VPidx索引号 **/
    unsigned char __pad1;		/** 字节对齐 **/
    unsigned char __pad2;		/** 字节对齐 **/
} action_t;

typedef struct actionQueue_s {
    int	head;
	int	tail;
    int flag;
	msgType_t stepMsgType;			/** 阶段事件完成消息 **/
	msgType_t overMsgType;			/** 队列事件全部完成消息 **/
	Timer_t *timer;
    action_t buf[ACTIONQUEUESIZE];     /** 16 bytes **/
} actionQueue_t;

/*******************************************************************************/
typedef union flashPage_u {
	unsigned char arrChar[CFLASH_PAGE_SIZE];
	u32  arrInt[CFLASH_PAGE_SIZE/4];
} flashPage_t;
/*******************************************************************************/
#endif /** ifndef end **/
