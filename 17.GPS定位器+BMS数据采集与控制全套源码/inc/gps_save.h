#ifndef __GPS_SAVE_H__
#define __GPS_SAVE_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"

typedef struct
{
    u8 *buf;
    u8 len;
}LocationSaveData;

/*它是个双头stack, 两头都可以pop(以保证写入历史文件时顺序写), 一头可以push*/
typedef struct
{
    LocationSaveData *his;
    u16 size;  /*总空间*/
    u16 read_idx;  /*当前读到哪一条, read==write 表示队列空*/
    u16 write_idx;  /*当前写到哪一条, 新消息往write处写*/
}LocationCurData;


#define  GOOME_HIS_FILE        L"Z:\\goome\\GmHisFile\0"
#define HIS_FILE_OFFSET_CNT    16   //头部几个字节不写
#define HIS_FILE_HEAD_FLAG     0xABCDDCBA     
#define HIS_FILE_FRAME_SIZE    100  //保存的文件时 , 第一字节是长度, 所以数据最长不超过(HIS_FILE_FRAME_SIZE-1)
#define SAVE_HIS_MAX_NUM       720
#define SAVE_CATCH_MAX_NUM     10


/**
 * Function:   数据插入缓存中
 * Description:缓存满失败
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_save_to_cache(u8 *data, u8 len);


/**
 * Function:   根据len决定数据是否从缓存中删除
 * Description:
 * Input:	   ack_place: socket ack.
 * Output:	   *from: which place to onfirm again.
 * Return:	   GM_SUCCESS——删除；其它错误码——未改变
 * Others:	   
 */
void gps_service_confirm_cache(u32 *from, u32 ack_place);


/**
 * Function:   数据插入stack缓存中
 * Description:缓存满会自动写文件,并重新插入
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_push_to_stack(u8 *data, u8 len);

/**
 * Function:   只看不删除数据,方便发送失败时, 重新发送
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gps_service_peek_one(LocationSaveData *one, bool from_head);
    

/**
 * Function:   与gps_service_peek_one配合, 删除一条已peek过的数据
 * Description:
 * Input:      无
 * Output:     无
 * Return:     GM_SUCCESS——成功；其它错误码——失败
 * Others:     
 */
GM_ERRCODE gps_service_commit_peek(bool from_head);
    


/**
 * Function:   删除历史数据文件
 * Description:
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     
 */
void gps_service_his_file_delete(void);


/**
 * Function:   历史数据存入文件,防止丢失
 * Description:
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     
 */
void gps_service_save_to_history_file(void);


#endif
