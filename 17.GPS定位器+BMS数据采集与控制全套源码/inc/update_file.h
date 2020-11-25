#ifndef __UPDATE_FILE_H__
#define __UPDATE_FILE_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"

#define UPDATE_PING_TIME 43200
#define UPDATE_RETRY_TIME 180
#define UPDATE_UPGRADE_FILE                 L"Z:\\goome\\GmUpGrade\0"
#define UPDATE_TARGET_IMAGE                 L"Z:\\goome\\GmAppMain.ex\0"
#define UPDATE_MINOR_IMAGE                L"Z:\\goome\\GmAppMinor.ex\0"

#define UPDATE_MAX_PACK   600
#define UPD_RESERVER_LEN 128
#define UPDATE_PAKET_MAX_LEN 500   //不能小于等于 UPD_RESERVER_LEN
#define UPDATAE_FILE_MAX_SIZE    (UPDATE_MAX_PACK * UPDATE_PAKET_MAX_LEN)
#define UPDATE_MAX_PACK_ONE_SEND       10 //每次发送包数      //不可超过30，remember fifo size should also modified

/*
2字节	1字节	2字节	8字节	N字节	1字节	1字节
包头      	命令字	报文长度	终端ID	数据内容	校验	结束符
0x68 0x68	CMD	    LEN	      ID	DATA	CHK	0x0D
*/
#define PROTOCOL_HEADER_UPDATE   0x68
#define PROTOCOL_TAIL_UPDATE   0x0D
typedef enum
{
    PROTOCCOL_UPDATE_REQUEST = 0x11,  //终端向服务器查询是否有升级
    PROTOCCOL_UPDATE_REQUEST_FILE = 0x12,  //终端向服务器请求文件数据
    PROTOCCOL_UPDATE_REPORT = 0x13,  //终端向服务器上报升级结果
    PROTOCCOL_UPDATE_RESPONSE = 0x91,  //服务器向终端返回是否有升级
    PROTOCCOL_UPDATE_FILE_DATA = 0x92,  //服务器向终端下发文件数据
}ProtocolUpdateCmdEnum;

typedef enum
{
    UPDATE_NONE = 0x00,
    UPDATE_QUE = 0x01,
    UPDATE_READY = 0x02,
}ProtocolUpdateResultEnum;


typedef enum
{
    REPORT_RESULT_FINISH = 0x00,  //已接收完成,尚未更新
    REPORT_RESULT_UPDATED = 0x01, //已接收完成,并更新
    REPORT_RESULT_FAILED = 0x02  //更新失败
}ReportResultEnum;


/*
记录服务器发来的升级响应

以及发给服务器的请求包状态
*/
typedef struct
{
    bool use_new_socket;  //是否新建连接
    
    u32 check_sum;    //升级文件的校验码
    u32 total_len;    //升级文件的总长度
    u8 custom_code[7];   //客户代码
    u8 model[6];    //终端型号编码
    u8 file_id[9];   //升级文件的ID
    u8 task_id[9];   //升级任务的ID

    //发给服务器的请求包状态
    u16 total_blocks;  //包数
    u16 block_size;  //每包数据大小
    u16 block_current; //当前包序号
    u32 block_status; // 从低到高, 每一位代表从当前包开始,往后的回收情况,收到回包为1,未收到为0

    int handle; // file handle
    ReportResultEnum result; // 升级结果 
    u8 result_info[50];
    u32 state_sum;   // state file的check sum
}UpdateFileExtend;

/*
只给update_service使用
由于 s_file_socket是s_update_socket的伴生socket, 
所以 update_file中能访问的, update_service一般也要能访问
*/
UpdateFileExtend *get_file_extend(void);
void init_file_extend(bool boot_init);

/**
 * Function:   创建update_file模块
 * Description:创建update_file模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE update_filemod_create(void);

/**
 * Function:   销毁update_file模块
 * Description:销毁update_file模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE update_filemod_destroy(void);

/**
 * Function:   update_file模块定时处理入口
 * Description:update_file模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE update_filemod_timer_proc(void);


void update_filemod_connection_failed(void);
void update_filemod_connection_ok(void);
void update_filemod_close_for_reconnect(void);
u32 update_filemod_get_checksum(const u16 *FileName);



/*以下函数与update_service模块共用*/
void update_msg_receive(SocketType *socket);
void update_msg_parse(u8 *pdata, u16 len);
void update_msg_parse_response(u8 *pdata, u16 len);

void update_msg_pack_head(u8 *pdata, u16 *idx, u16 len);
void update_msg_pack_id_len(u8 *pdata, u8 id, u16 len);
void update_msg_pack_request(u8 *pdata, u16 *idx, u16 len);


void update_msg_start_data_block_request(SocketType *socket);
GM_ERRCODE update_msg_send_data_block_request(SocketType *socket);
GM_ERRCODE update_msg_send_result_to_server(SocketType *socket);

#endif

