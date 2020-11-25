#include "gps_save.h"
#include <stdio.h>
#include "gm_stdlib.h"
#include "gm_memory.h"
#include "log_service.h"
#include "gm_fs.h"
#include "utility.h"

typedef struct  // 6460
{
    int handle; // file handle
    u16 record_num;  // total records in file
}HisDatFileType;

#define CLOSE_FILE_HANDLE( handle ) \
{ \
    if((handle) >= 0) \
    { \
        GM_FS_Close((handle)); \
        (handle) = -1; \
    } \
}

static HisDatFileType s_his_dat; /* 对应历史文件数据 */

static LocationCurData s_gps_data = {NULL, 0, 0, 0};
static LocationCurData s_gps_sending = {NULL, 0, 0, 0};

static GM_ERRCODE gps_data_init(LocationCurData *pdata, HisDatFileType *phis, u16 size, bool read_from_file);
static u16 gps_data_get_len(LocationCurData *pdata);
static u16 gps_data_get_free_size(LocationCurData *pdata);
static GM_ERRCODE gps_data_peek_one(LocationCurData *pdata, HisDatFileType *phis, LocationSaveData *one, bool from_head);
static GM_ERRCODE gps_data_commit_peek(LocationCurData *pdata, HisDatFileType *phis,bool from_head,bool write_log);
static void gps_data_save_to_history_file(LocationCurData *pdata, HisDatFileType *phis);


static void gps_his_file_delete(HisDatFileType *phis);
static u32 gps_his_file_write_head(HisDatFileType *phis, u8 *head_buff);
static void gps_his_file_delete_record(HisDatFileType *phis, u16 del_num);
static void gps_data_release(LocationSaveData *one);
static u32 gps_his_file_write(HisDatFileType *phis, void * DataPtr, UINT Length, int Offset);
static GM_ERRCODE gps_his_file_create(HisDatFileType *phis);
static u32 gps_his_file_read(HisDatFileType *phis, void * DataPtr, UINT Length, int Offset);
static GM_ERRCODE gps_data_save_file(LocationCurData *pdata, HisDatFileType *phis);
static GM_ERRCODE gps_data_read_file(LocationCurData *pdata, HisDatFileType *phis,bool read_from_file);

/*add to tail*/
static GM_ERRCODE gps_data_insert_one(LocationCurData *pdata, HisDatFileType *phis, u8 *data, u8 len, bool log);



GM_ERRCODE gps_data_init(LocationCurData *pdata, HisDatFileType *phis, u16 size, bool read_from_file)
{
    // the last block have to empty to keep read==write means empty.
    size +=1;
    
	/* allocate gps space. */
	pdata->his = (LocationSaveData *) GM_MemoryAlloc(size * sizeof(LocationSaveData));
	if (pdata->his == NULL)
	{
        LOG(WARN,"gps_data_init(%p,%p) assert(GM_MemoryAlloc(%d) failed", pdata,phis, size * sizeof(LocationSaveData));
		return GM_MEM_NOT_ENOUGH;
	}
	
	GM_memset((u8 *)pdata->his, 0x00, sizeof(LocationSaveData) * size);

	pdata->read_idx = 0;
	pdata->write_idx = 0;
	pdata->size = size;

    if(phis)
    {
        phis->handle = -1;
        phis->record_num = 0;

        //if file exist, read from history file.
        gps_data_read_file(pdata,phis,read_from_file);
    }

	return GM_SUCCESS;
}


u16 gps_data_get_len(LocationCurData *pdata)
{
    int read_point = 0;
    int write_point = 0;
    int LeftDataSize = 0;
	
    read_point = pdata->read_idx;
    write_point = pdata->write_idx;

    LeftDataSize = write_point - read_point;
    if(LeftDataSize < 0)
    {
        LeftDataSize += pdata->size;
    }
    
    return (u16)LeftDataSize;
}


u16 gps_data_get_free_size(LocationCurData *pdata)
{
    // the last block have to empty to keep read==write means empty.
    return (pdata->size - gps_data_get_len(pdata)) - 1;
}


void gps_data_save_to_history_file(LocationCurData *pdata, HisDatFileType *phis)
{
    if(gps_data_get_len(pdata) > 0)
    {
        gps_data_save_file(pdata, phis);
    }
}



GM_ERRCODE gps_data_insert_one(LocationCurData *pdata, HisDatFileType *phis, u8 *data, u8 len, bool log)
{
    int read_point = 0;
    int write_point = 0;
    int write_idx;
	
    read_point = pdata->read_idx;
    write_idx = write_point = pdata->write_idx;
    write_point ++;

    if(write_point >= pdata->size)
    {
        write_point -= (int)pdata->size;
    }

    if(write_point == read_point)
    {
        return GM_MEM_NOT_ENOUGH;
    }

    pdata->his[write_idx].buf = (u8 *)GM_MemoryAlloc(len);
    if(NULL == pdata->his[write_idx].buf)
    {
        LOG(WARN,"gps_data_init(%p,%p) assert(GM_MemoryAlloc(%d) failed",pdata, phis, len);
        return GM_SYSTEM_ERROR;
    }
    
    pdata->his[write_idx].len = len;
    GM_memcpy(pdata->his[write_idx].buf, data, len);

    pdata->write_idx = write_point;

    if(log)  // when read form file , no need log
    {
        LOG(DEBUG,"clock(%d) gps_data_insert_one(%p,%p)(r:%d,w:%d,size:%d, len:%d) his_num(%d).", util_clock(),
            pdata,phis,pdata->read_idx,pdata->write_idx,pdata->size,len,(phis?phis->record_num:0));
    }
    return GM_SUCCESS;
}


GM_ERRCODE gps_data_peek_one(LocationCurData *pdata, HisDatFileType *phis, LocationSaveData *one, bool from_head)
{
    int read_point = 0;
    int write_point = 0;
    int idx;

    if(pdata->his == NULL)
    {
        return GM_EMPTY_BUF;
    }

	
    read_point = pdata->read_idx;
    write_point = pdata->write_idx;

    
    if(write_point == read_point)
    {
        if((! from_head) && (phis != 0) && phis->record_num > 0)
        {
            //从尾部拿最新的gps数据的情况,只发生在发送数据给平台时,
            //这时如果无数据,要补发历史数据
            if(GM_SUCCESS == gps_data_read_file(pdata, phis, true))
            {
                return gps_data_peek_one(pdata, phis, one, from_head);
            }
        }
        return GM_EMPTY_BUF;
    }

    //data is between read_idx(include) and write_idx(not include)
    write_point --;
    if(write_point < 0)
    {
        write_point += (int)pdata->size;
    }
    
    idx = from_head ? read_point : write_point;
    
    *one = pdata->his[idx];
    
    return GM_SUCCESS;
}


GM_ERRCODE gps_data_commit_peek(LocationCurData *pdata, HisDatFileType *phis,bool from_head,bool write_log)
{
    int read_point = 0;
    int write_point = 0;
    int idx = 0;
	int len = 0;
    
    read_point = pdata->read_idx;
    write_point = pdata->write_idx;

    if(write_point == read_point)
    {
        return GM_EMPTY_BUF;
    }

    if(from_head)
    {
        //删除头部一个元素
        idx = read_point;
        read_point ++;
        len = pdata->his[idx].len;
        gps_data_release(&pdata->his[idx]);
        if(read_point >= pdata->size)
        {
            read_point -= (int)pdata->size;
        }
        pdata->read_idx = read_point;
    }
    else
    {
        //data is between read_idx(include) and write_idx(not include)
        write_point --;
        if(write_point < 0)
        {
            write_point += (int)pdata->size;
        }
        idx = write_point;
        len = pdata->his[idx].len;
        gps_data_release(&pdata->his[idx]);
        pdata->write_idx = write_point;
    }

    if(write_log)
    {
        LOG(DEBUG,"clock(%d) gps_data_commit_peek(%p,%p)(r:%d,w:%d,size:%d, len:%d, from_head:%d) his_num(%d).", util_clock(),
            pdata,phis,pdata->read_idx,pdata->write_idx,pdata->size,len,from_head,(phis?phis->record_num:0));
    }
    return GM_SUCCESS;
}




static void gps_data_release(LocationSaveData *one)
{
    if(one && one->buf)
    {
        GM_MemoryFree(one->buf);
        one->buf = NULL;
        one->len = 0;
    }
}

void gps_his_file_delete(HisDatFileType *phis)
{
    int ret = -1;

    CLOSE_FILE_HANDLE(phis->handle)

    ret = GM_FS_CheckFile(GOOME_HIS_FILE);
    if (ret >= 0)
    {
        GM_FS_Delete(GOOME_HIS_FILE);
        LOG(INFO,"clock(%d) gps_his_file_delete", util_clock());
    }
}

/*
文件没打开, 则打开文件, 并写入数据, 文件不会被关闭. 
出错的情况下, 文件会被关闭
*/
static u32 gps_his_file_write(HisDatFileType *phis, void * DataPtr, UINT Length, int Offset)
{
    u32 fs_len = 0;
    int ret = -1;

    if(phis->handle < 0)
    {
        phis->handle = GM_FS_Open(GOOME_HIS_FILE, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
        if (phis->handle < 0)
        {
            LOG(INFO,"goome_his_file_write-open fail return[%d]", phis->handle);
            phis->handle = -1;
            return 0;
        }
        LOG(DEBUG,"clock(%d) gps_his_file_write open or create file", util_clock());
    }

    GM_FS_Seek(phis->handle, Offset, 0);
    ret = GM_FS_Write(phis->handle, DataPtr, Length, &fs_len);
    if (ret < 0)
    {
        LOG(INFO,"goome_his_file_write-write fail return[%d]", ret);
        CLOSE_FILE_HANDLE(phis->handle)
        return 0;
    }

    GM_FS_Commit(phis->handle);

    return fs_len;
}


/*文件一定会被删除, handle一定会被关掉, 再重新打开*/
static GM_ERRCODE gps_his_file_create(HisDatFileType *phis)
{
    u8 file_head[20];

    gps_his_file_delete(phis);

    GM_memset(file_head, 0x00, sizeof(file_head));
    file_head[0] = (HIS_FILE_HEAD_FLAG >> 24) & 0xFF;
    file_head[1] = (HIS_FILE_HEAD_FLAG >> 16) & 0xFF;
    file_head[2] = (HIS_FILE_HEAD_FLAG >> 8) & 0xFF;
    file_head[3] = (HIS_FILE_HEAD_FLAG ) & 0xFF;

    if (HIS_FILE_OFFSET_CNT == gps_his_file_write(phis, (void *)&file_head, HIS_FILE_OFFSET_CNT, 0))
    {
        LOG(INFO,"gps_his_file_create ok");
        phis->record_num = 0;
        return GM_SUCCESS;
    }
    else
    {
        LOG(INFO,"gps_his_file_create failed");
        CLOSE_FILE_HANDLE(phis->handle)
        return GM_SYSTEM_ERROR;
    }
}


static u32 gps_his_file_read(HisDatFileType *phis, void * DataPtr, UINT Length, int Offset)
{
    u32 fs_len = 0;
    int ret = -1;
    
    if(phis->handle < 0)
    {
        phis->handle = GM_FS_Open(GOOME_HIS_FILE, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE);
        if (phis->handle < 0)
        {
            LOG(DEBUG,"gps_his_file_read open failed, return [%d]",phis->handle);
            phis->handle = -1;
            return 0;
        }
        LOG(DEBUG,"clock(%d) gps_his_file_read open file", util_clock());
    }

    GM_FS_Seek(phis->handle, Offset, 0);
    ret = GM_FS_Read(phis->handle, DataPtr, Length, &fs_len);
    if (ret < 0)
    {
        LOG(INFO,"gps_his_file_read failed, ret[%d]",ret);
        CLOSE_FILE_HANDLE(phis->handle)
        return 0;
    }

    GM_FS_Commit(phis->handle);

    return fs_len;
}


/*将pdata缓存中的内容添加到文件后面*/
static GM_ERRCODE gps_data_save_file(LocationCurData *pdata, HisDatFileType *phis)
{
    u8 head_buff[20];
    u8 one_frame[HIS_FILE_FRAME_SIZE];
    LocationSaveData one;
    u16 fs_len = 0;
    u16 write_len = 0;

    GM_memset(head_buff, 0x00, sizeof(head_buff));
    fs_len = gps_his_file_read(phis, (void *)&head_buff, HIS_FILE_OFFSET_CNT, 0);
    if ((HIS_FILE_OFFSET_CNT != fs_len) || (HIS_FILE_HEAD_FLAG != MKDWORD(head_buff[0], head_buff[1], head_buff[2], head_buff[3])))
    {
        if (GM_SUCCESS != gps_his_file_create(phis))
        {
            //创建文件失败
            return GM_SYSTEM_ERROR;
        }
    }
    else
    {
        phis->record_num = MKWORD(head_buff[4], head_buff[5]);
    }

    while(GM_SUCCESS == gps_data_peek_one(pdata, phis, &one, 1))
    {
        if(SAVE_HIS_MAX_NUM <= phis->record_num)
        {
            LOG(INFO,"gps_data_save_file SAVE_HIS_MAX_NUM reached.");
            
            //淘汰最旧的数据
            gps_his_file_delete_record(phis, gps_data_get_len(pdata));
        }
        
        one_frame[0] = one.len;
        if(one.len >= HIS_FILE_FRAME_SIZE)
        {
            LOG(ERROR,"gps_data_save_file assert(len(%d)) failed.", one.len);
            break;
        }
        
        GM_memcpy(&one_frame[1], one.buf,one.len);
        write_len = gps_his_file_write(phis, one_frame, HIS_FILE_FRAME_SIZE, phis->record_num*HIS_FILE_FRAME_SIZE + HIS_FILE_OFFSET_CNT);
        if(write_len == HIS_FILE_FRAME_SIZE)
        {
            phis->record_num ++;
            gps_data_commit_peek(pdata, phis, true, true);
        }
        else
        {
            LOG(WARN,"gps_data_save_file assert(write) failed, ret[%d]",write_len);
            break;
        }
    }

    gps_his_file_write_head(phis, head_buff);

    CLOSE_FILE_HANDLE(phis->handle)

    LOG(DEBUG,"clock(%d) gps_data_save_file his_data(num:%d)", util_clock(), phis->record_num);

    return GM_SUCCESS;
}


/*将文件中的内容读到pdata缓存, 如果缓存满了, 剩余的文件中的记录前移*/
static GM_ERRCODE gps_data_read_file(LocationCurData *pdata, HisDatFileType *phis, bool read_from_file)
{
    u8 head_buff[20];
    u8 one_frame[HIS_FILE_FRAME_SIZE];
    u16 fs_len = 0;
    u16 write_len = 0;
    int idx = 0;
    int new_idx = 0;

    GM_memset(head_buff, 0x00, sizeof(head_buff));
    fs_len = gps_his_file_read(phis, (void *)&head_buff, HIS_FILE_OFFSET_CNT, 0);
    if ((HIS_FILE_OFFSET_CNT != fs_len) || (HIS_FILE_HEAD_FLAG != MKDWORD(head_buff[0], head_buff[1], head_buff[2], head_buff[3])))
    {
        CLOSE_FILE_HANDLE(phis->handle)
        LOG(DEBUG,"clock(%d) gps_data_read_file no history file exist. read_len(%d).", util_clock(),fs_len);

        //record_num means records in history file.
        phis->record_num = 0;
        return GM_SYSTEM_ERROR;
    }
    else
    {
        phis->record_num = MKWORD(head_buff[4], head_buff[5]);
    }

    if(!read_from_file)
    {
        // only read head. to get record_num
        LOG(INFO,"clock(%d) gps_data_read_file only head(num:%d)", util_clock(), phis->record_num);
        return GM_SUCCESS;
    }

    for(idx = 0; idx < phis->record_num; ++idx)
    {
        fs_len = gps_his_file_read(phis, (void *)&one_frame[0], HIS_FILE_FRAME_SIZE, HIS_FILE_OFFSET_CNT + idx * HIS_FILE_FRAME_SIZE);
        if(fs_len != HIS_FILE_FRAME_SIZE)
        {
            LOG(WARN,"gps_data_read_file assert(read) failed, ret[%d]",fs_len);
            break;
        }
        if(gps_data_get_free_size(pdata) > 0)
        {
            gps_data_insert_one(pdata, phis, &one_frame[1], one_frame[0], false);
        }
        else
        {
            write_len = gps_his_file_write(phis, one_frame, HIS_FILE_FRAME_SIZE, new_idx*HIS_FILE_FRAME_SIZE + HIS_FILE_OFFSET_CNT);
            if(write_len == HIS_FILE_FRAME_SIZE)
            {
                new_idx ++;
            }
            else
            {
                LOG(WARN,"gps_data_read_file assert(write) failed, ret[%d]",write_len);
                break;
            }
        }
    }

    phis->record_num = new_idx;
    gps_his_file_write_head(phis, head_buff);

    CLOSE_FILE_HANDLE(phis->handle)

    LOG(INFO,"clock(%d) gps_data_read_file his_data(num:%d)", util_clock(), phis->record_num);

    return GM_SUCCESS;
}


static u32 gps_his_file_write_head(HisDatFileType *phis, u8 *head_buff)
{
    head_buff[0] = (HIS_FILE_HEAD_FLAG >> 24) & 0xFF;
    head_buff[1] = (HIS_FILE_HEAD_FLAG >> 16) & 0xFF;
    head_buff[2] = (HIS_FILE_HEAD_FLAG >> 8) & 0xFF;
    head_buff[3] = (HIS_FILE_HEAD_FLAG) & 0xFF;

    head_buff[4] = (phis->record_num >> 8) & 0xFF;
    head_buff[5] = (phis->record_num) & 0xFF;
    return gps_his_file_write(phis, head_buff, HIS_FILE_OFFSET_CNT, 0);
}


static void gps_his_file_delete_record(HisDatFileType *phis, u16 del_num)
{
    u8 head_buff[20];
    u8 one_frame[HIS_FILE_FRAME_SIZE];
    u16 fs_len = 0;
    u16 write_len = 0;
    int idx = 0;
    int new_idx = 0;

    if(phis->record_num <= 0)
    {
        LOG(ERROR,"clock(%d) gps_his_file_delete_record. assert(record_num(%d)) failed.", util_clock(),phis->record_num);
        return;
    }
    if(phis->handle < 0)
    {
        LOG(ERROR,"clock(%d) gps_his_file_delete_record. assert(handle(%d)) failed.", util_clock(),phis->handle);
        return;
    }

    for(idx = 0; idx < phis->record_num; ++idx)
    {
        fs_len = gps_his_file_read(phis, (void *)&one_frame[0], HIS_FILE_FRAME_SIZE, HIS_FILE_OFFSET_CNT + idx * HIS_FILE_FRAME_SIZE);
        if(fs_len != HIS_FILE_FRAME_SIZE)
        {
            LOG(WARN,"gps_data_read_file assert(read) failed, ret[%d]",fs_len);
            break;
        }
        if(idx >= del_num)
        {
            write_len = gps_his_file_write(phis, one_frame, HIS_FILE_FRAME_SIZE, new_idx*HIS_FILE_FRAME_SIZE + HIS_FILE_OFFSET_CNT);
            if(write_len == HIS_FILE_FRAME_SIZE)
            {
                new_idx ++;
            }
            else
            {
                LOG(WARN,"gps_his_file_delete_record assert(write) failed, ret[%d]",write_len);
                break;
            }
        }
    }

    phis->record_num = new_idx;
    gps_his_file_write_head(phis, head_buff);

    LOG(INFO,"clock(%d) gps_his_file_delete_record(%d) his_data(num:%d)", util_clock(),del_num, phis->record_num);

    return;
}


GM_ERRCODE gps_service_save_to_cache(u8 *data, u8 len)
{
    if(s_gps_sending.his == NULL)
    {
        gps_data_init(&s_gps_sending, NULL, SAVE_CATCH_MAX_NUM, false);  
    }
    if(gps_data_get_free_size(&s_gps_sending) == 0)
    {
        return GM_MEM_NOT_ENOUGH;
    }

    return gps_data_insert_one(&s_gps_sending, NULL, data, len, true);
}


void gps_service_confirm_cache(u32 *from, u32 ack_place)
{
    u32 confirm_len = (ack_place - (*from));
    u32 confirm_total = 0;
    LocationSaveData one;

    if(s_gps_sending.his == NULL)
    {
        gps_data_init(&s_gps_sending, NULL, SAVE_CATCH_MAX_NUM, false);  
    }
    
    if(0 == gps_data_get_len(&s_gps_sending))
    {
        (*from) = ack_place;
        return;
    }

    if((*from == 0) && (ack_place == 0))
    {
        ack_place = 1000000;   //sdk还没支持GM_GetTcpStatus ， 直接confirm 所有数据, 最后, (*from) = ack_place = 0;
        LOG(DEBUG,"clock(%d) gps_service_confirm_cache(%d,%d) cache_data(num:%d)", util_clock(),*from, ack_place, gps_data_get_len(&s_gps_sending));
    }
    
    while(GM_SUCCESS == gps_data_peek_one(&s_gps_sending, NULL, &one, true))
    {
        confirm_total += one.len;
        if(confirm_total <= confirm_len)
        {
            LOG(DEBUG,"clock(%d) gps_service_confirm_cache(%d,%d) (%d < %d)", util_clock(),*from, ack_place, confirm_total, confirm_len);
            gps_data_commit_peek(&s_gps_sending, NULL, true, true);
            (*from) += one.len;
        }
        else
        {
            break;
        }
    }

    if(0 == gps_data_get_len(&s_gps_sending))
    {
        (*from) = ack_place;
    }
}



GM_ERRCODE gps_service_push_to_stack(u8 *data, u8 len)
{
    if(s_gps_data.his == NULL)
    {
        gps_data_init(&s_gps_data, &s_his_dat, SAVE_HIS_MAX_NUM/3, true);  
    }

    if(gps_data_get_free_size(&s_gps_data) == 0)
    {
        gps_data_save_file(&s_gps_data, &s_his_dat);
    }

    return gps_data_insert_one(&s_gps_data, &s_his_dat, data, len, true);
}

GM_ERRCODE gps_service_peek_one(LocationSaveData *one, bool from_head)
{
    if(s_gps_data.his == NULL)
    {
        gps_data_init(&s_gps_data, &s_his_dat, SAVE_HIS_MAX_NUM/3, true);  
    }

    return gps_data_peek_one(&s_gps_data, &s_his_dat, one, from_head);
}

GM_ERRCODE gps_service_commit_peek(bool from_head)
{
    return gps_data_commit_peek(&s_gps_data, &s_his_dat, from_head, true);
}

void gps_service_his_file_delete(void)
{
    gps_his_file_delete(&s_his_dat);
}

void gps_service_save_to_history_file(void)
{
    LocationSaveData one;
    while(gps_data_get_len(&s_gps_sending) > 0)
    {
        one.len = 0;
        gps_data_peek_one(&s_gps_sending, NULL, &one, true);
        if(one.len > 0)
        {
            gps_service_push_to_stack(one.buf, one.len);
        }
        gps_data_commit_peek(&s_gps_sending, NULL, true, true);
    }
    
    gps_data_save_to_history_file(&s_gps_data, &s_his_dat);
}


