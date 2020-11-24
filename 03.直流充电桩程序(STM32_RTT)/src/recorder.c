#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "sflash.h"
#include "recorder.h"
#include "string.h"


#if 1
#define ETH_RECDER DBG_MSG
#else
#define ETH_RECDER (void)
#endif


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐

typedef struct
{
	u16 valid;  //标志是否有效  == 0xAAAA 有效 用于第一次上电
	u16 cnt;
	u16 newest_addr;  //最新ID  0~16383   0xffff:无效
}record_header_def;

#pragma pack(pop) //恢复对齐状态


record_header_def  recd_h;


///////充电历史记录信息占用FLASH 第2047页开始共1MB + 512byte空间
#define FLASH_PAGE_SIZE  512
#define RECD_HEADER_PAGEADDR 2047  //历史记录头信息地址
#define RECD_DATA_PAGEADDR 2048  //历史记录信息开始地址
#define RECD_DATA_SIZE 64  //一条记录数据大小
#define RECD_MAX_CNT     16384  //历史记录总数  16384 * 64 = 1048576 (1MB)


/**
  * @brief  读取记录头信息
  * @retval 0 成功   <0 失败
  */
int recd_header_read()
{
	sflash_read(RECD_HEADER_PAGEADDR, 0, (u8*)&recd_h, sizeof(recd_h));

	if(recd_h.valid != 0xAAAA)  //无效
	{
		recd_h.cnt = 0;
		recd_h.newest_addr = 0xffff;
	}
	return 0;
}


/*
 * @brief  初始化  读出头信息
 */
void recd_init()
{
	recd_header_read();
	//current_id = recd_h.newest_addr;
}


/**
  * @brief  写头信息
  * @retval 0 成功   <0 失败
  */
int recd_header_write()
{
	recd_h.valid = 0xAAAA;
	return sflash_write(RECD_HEADER_PAGEADDR, 0, (u8*)&recd_h, sizeof(recd_h));
}



/*
 * @brief 清除记录
 */
void recd_clr()
{
	recd_h.cnt = 0;
	recd_h.newest_addr = 0xffff;
	recd_header_write();
}

/*
 * @brief  读取记录
 * @param  addr    0~16383
 * @param  recd
 * @retval 0 成功   <0 失败
 */
int recd_read(u16 addr, record_def* recd)
{
	u16 page_addr;
	u16 offset;
	if((addr == 0xffff) || (recd_h.cnt == 0) || (addr > recd_h.cnt))  //无记录   大于实际条数
	{
		return -1;
	}

	page_addr = 2048 + (addr / (FLASH_PAGE_SIZE / RECD_DATA_SIZE));  //页地址
	offset = (addr % (FLASH_PAGE_SIZE / RECD_DATA_SIZE)) * RECD_DATA_SIZE;  //页内偏移地址
	if(sflash_read(page_addr, offset, (u8*)recd, sizeof(record_def)) == 0)  //读FLASH
	{
		return 0;
	}
	return -2;
}


/*
 * @brief  写记录
 * @param  addr  0~16383
 * @param  recd
 * @retval 0 成功   <0 失败
 */
int recd_write(u16 addr, record_def* recd)
{
	u16 page_addr;
	u16 offset;

	if(addr >= RECD_MAX_CNT)  //超出最大记录条数
	{
		return -1;
	}

	page_addr = 2048 + (addr / (FLASH_PAGE_SIZE / RECD_DATA_SIZE));  //页地址
	offset = (addr % (FLASH_PAGE_SIZE / RECD_DATA_SIZE)) * RECD_DATA_SIZE;  //页内偏移地址
	if(sflash_write(page_addr, offset, (u8*)recd, sizeof(record_def)) == 0)  //写FLASH
	{

	}
	return 0;
}

/*
 * @brief  获取下一条记录存储的ID
 * @retval addr  0~16383
 */
u16 recd_get_next_id()
{
	recd_h.newest_addr++;
	recd_h.newest_addr %= RECD_MAX_CNT;  //当ID == RECD_MAX_CNT 时返回到0
	if(recd_h.cnt != RECD_MAX_CNT)
	{
		recd_h.cnt++;
	}
	recd_header_write();  //保存头信息
	return recd_h.newest_addr;  //返回ID
}

/*
 * @brief  新增一条记录  当要写入一条新记录时调用
 * @retval addr  0~16383
 */
u16 recd_add()
{
	record_def temp;
	u16 id = recd_get_next_id();
	memset(&temp, 0, sizeof(temp));  //所有数据清零
	recd_write(id, &temp);
	return id;
}

/*
 * @brief  返回最新记录ID
 */
u16 recd_get_newest_id()
{
	return recd_h.newest_addr;
}


/*
 * @brief  读取记录
 * @param  id  0~16383   0代表最新的记录   16383代表最旧的记录
 * @param  recd
 * @retval 0 成功   <0 失败
 */
int recd_read_new(u16 id, record_def* recd)
{
	u16 addr;
	if(id >= recd_h.cnt)
	{
		return -1;
	}

	//根据id计算addr
	if(recd_h.newest_addr >= id)
	{
		addr = recd_h.newest_addr - id;
	}
	else
	{
		addr = (RECD_MAX_CNT - 1) - (id - (recd_h.newest_addr + 1));
	}

	return recd_read(addr, recd);  //读记录
}


/*
 * @brief  返回总记录数
 */
u16 recd_get_cnt()
{
	return recd_h.cnt;
}



///*
// * @brief  写
// * @param  recd
// * @retval 0 成功   <0 失败
// */
//int recd_write0(record_def* recd)
//{
//	u16 page_addr;
//	u16 offset;
//	u16 next_offset;
//
//	//next_offset 0 ~ RECD_MAX_CNT-1
//	next_offset = (recd_h.newest_id) % RECD_MAX_CNT;   //若大于RECD_MAX_CNT返回起始位置
//
//	page_addr = 2048 + (next_offset / (FLASH_PAGE_SIZE / RECD_DATA_SIZE));
//	offset = (next_offset % (FLASH_PAGE_SIZE / RECD_DATA_SIZE)) * RECD_DATA_SIZE;
//	if(sflash_write(page_addr, offset, (u8*)recd, sizeof(record_def)) == 0)
//	{
//		if(recd_h.cnt != RECD_MAX_CNT)
//		{
//			recd_h.cnt++;  //计数条数加1
//		}
//
//		recd_h.newest_id = next_offset + 1;
//		current_id = next_offset + 1;
//
//		if(recd_h.cnt == RECD_MAX_CNT)  //空间写满
//		{
//			if(recd_h.newest_id == RECD_MAX_CNT)
//			{
//				recd_h.oldest_id = 1;
//			}
//			else
//			{
//				recd_h.oldest_id = (recd_h.oldest_id + 1) % (RECD_MAX_CNT + 1);  //若大于RECD_MAX_CNT返回起始位置
//			}
//		}
//		else
//		{
//			recd_h.oldest_id = 1;
//		}
//		recd_header_write();  //保存记录头信息
//		return 0;
//	}
//	return -1;
//}


/*
 * @brief  读取最新记录
 * @param  recd
 * @retval 0 成功   <0 失败
 */
//int recd_read_newest(record_def* recd)
//{
//	if((recd_h.cnt == 0))  //记录为空
//	{
//		return -1;
//	}
//
//	if((recd_h.cnt != RECD_MAX_CNT) && (current_id == 0))  //已经读完全部
//	{
//		return -1;
//	}
//	else if((recd_h.cnt == RECD_MAX_CNT) && (current_id == recd_h.oldest_id))  //bug:当读写满RECD_MAX_CNT条数据 后读不到最旧的一条数据
//	{
//		return -1;
//	}
//
//	if(recd_read(current_id, recd) == 0)
//	{
//		current_id--;
//
//		if((current_id == 0) && (recd_h.cnt == RECD_MAX_CNT))
//		{
//			current_id = RECD_MAX_CNT;
//		}
//		return 0;
//	}
//	return -1;
//}

//
//
//
//
//
///*
// * @brief  移动读指针到指定记录ID号
// */
//void recd_lseek(u16 id)
//{
//	if(id != 0)
//	{
//		current_id = id;
//	}
//}
//
//
///*
// * @brief  移动读指针  向更新的方向移动
// */
//void recd_read_forward(u16 offset)
//{
//	current_id += offset + 1;
//	if(current_id > recd_h.newest_addr)
//	{
//		current_id = recd_h.newest_addr;
//	}
//}









