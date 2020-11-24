#include "block_fifo.h"
#include <string.h>


/*
 * block fifo说明，存取是以一块数据为单位(长度不定)而不是字节，一块数据的地址是连续的，可以用于MDA。每一个数据块的前两个字节记录此数据的长度后面紧跟数据
 */


#define FIFO_FULL ((p->read == p->write) && (p->read_cnt != p->write_cnt))
#define FIFO_EMPT ((p->read == p->write) && (p->read_cnt == p->write_cnt))


static void write_to_fifo(b_fifo_typ *p, unsigned char *data, unsigned short len)
{
	memcpy(p->write->data, data, len);
	p->write->len = len;
	p->write_cnt++;
	p->write = (b_fifo_data_typ*)((unsigned int)p->write + len + sizeof(p->write->len));
}


/*
 * @brief   写一个数据块到b_fifo
 * @param   b_fifo_buf 内存地址
 * @param   data 要写的数据
 * @param   len  数据长度
 * @retval  1 成功  0 失败
 */
int b_fifo_write(void *b_fifo_buf, unsigned char *data, unsigned short len)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;
	unsigned int temp;


	if(FIFO_FULL)  //fifo已满
	{
		return 0;
	}


	if(p->write >= p->read)  //写指针在读指针后
	{
		temp = (unsigned int)p->buf_end - (unsigned int)p->write;
		if(temp >= len + sizeof(p->write->len))  //后面有足够空间
		{
			write_to_fifo(p, data, len);  //写入FIFO

			if((unsigned int)p->write >= (unsigned int)p->buf_end)
			{
				p->write = (b_fifo_data_typ*)p->buf;  //指针返回到开始
			}
			return 1;
		}
		else  //后面没有足够空间  到前面找
		{
			temp = (unsigned int)p->read - (unsigned int)p->buf;
			if(temp >= len + sizeof(p->write->len))  //有足够空间
			{
				p->write_end = p->write;  //标记靠近p->buf的最后一个block  当p->write_end == p->read 时p->read返回到p->buf
				p->write = (b_fifo_data_typ*)p->buf;
				write_to_fifo(p, data, len);
				return 1;
			}
		}
	}
	else //写指针在读指针前
	{
		temp = (unsigned int)p->read - (unsigned int)p->write;

		if(temp >= len + sizeof(p->write->len))  //有足够空间
		{
			write_to_fifo(p, data, len);
			return 1;
		}
	}

	return 0;
}



/*
 * @brief   从b_fifo读一个数据块
 * @param   b_fifo_buf 内存地址
 * @param   data 读取数据的存储地址
 * @param   len  数据长度
 * @retval  0 失败  大于0成功(数据长度)
 */
unsigned short b_fifo_read(void *b_fifo_buf, unsigned char *data)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;
	unsigned short temp;
	if(!FIFO_EMPT)
	{
		memcpy(data, p->read, p->read->len);
		temp = p->read->len;
		p->read_cnt++;
		p->read = (b_fifo_data_typ*)((unsigned int)p->read + p->read->len + sizeof(p->read->len));  //指针后移
		if(((unsigned int)p->read >= (unsigned int)p->buf_end))
		{
			p->read = (b_fifo_data_typ*)p->buf;  //指针返回
		}

		return temp;
	}
	return 0;
}


/*
 * @brief   从b_fifo读一个数据块的地址，数据处理完后应调用b_fifo_del函数删除些数据，以释放空间
 * @param   b_fifo_buf 内存地址
 * @retval  0 失败  大于0成功
 */
b_fifo_data_typ* b_fifo_get(void *b_fifo_buf)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;

	if(!FIFO_EMPT)
	{
		if(p->read == p->write_end)
		{
			p->read = (b_fifo_data_typ*)p->buf;  //指针返回
			p->write_end = 0;
		}

		return p->read;
	}
	return 0;
}

/*
 * @brief    删除第一个有效数据块    与b_fifo_get函数配合使用
 * @param    b_fifo_buf 内存地址
 */
void b_fifo_del(void *b_fifo_buf)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;

	if(!FIFO_EMPT)
	{
		//memset(p->read->data, 0, p->read->len);

		p->read = (b_fifo_data_typ*)((unsigned int)p->read + p->read->len + sizeof(p->read->len));  //指针后移
		if(((unsigned int)p->read >= (unsigned int)p->buf_end))  //指针返回
		{
			p->read = (b_fifo_data_typ*)p->buf;
		}
		p->read_cnt++;
	}
}


/*
 * @brief   初始化block fifo
 * @param   b_fifo_buf 内存地址
 * @param   b_fifo_buf_len 长度
 * @param   block_cnt  block数量
 */
void b_fifo_init(void *b_fifo_buf, unsigned short b_fifo_buf_len)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;

	//memset(b_fifo_buf, 0, b_fifo_buf_len);

	p->read = (b_fifo_data_typ*)p->buf;
	p->write = (b_fifo_data_typ*)p->buf;
	p->read_cnt = p->write_cnt = 0;
	p->write_end = 0;
	p->buf_end = (unsigned char*)b_fifo_buf + b_fifo_buf_len;
}

