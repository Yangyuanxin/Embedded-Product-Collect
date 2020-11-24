#ifndef BLOCK_FIFO_H_
#define BLOCK_FIFO_H_




typedef struct
{
	unsigned short len;      //数据长度
	unsigned char  data[1];  //数据
}b_fifo_data_typ;


typedef struct
{
	b_fifo_data_typ *read;     //当前读指针
	b_fifo_data_typ *write;    //当前写指针
	b_fifo_data_typ *write_end;  //当写指针移动到接近buf_end时由于没有空间要重新回到buf时赋值，用于判断read指针何时返回
	unsigned char read_cnt;    //已经读取的数据块总数
	unsigned char write_cnt;   //已经写入的数据块总数
	unsigned char *buf_end;    //缓冲区结束地址
    unsigned char buf[1];      //缓冲区开始地址
}b_fifo_typ;

int b_fifo_write(void *b_fifo_buf, unsigned char *data, unsigned short len);
unsigned short b_fifo_read(void *b_fifo_buf, unsigned char *data);
void b_fifo_init(void *b_fifo_buf, unsigned short b_fifo_buf_len);
b_fifo_data_typ* b_fifo_get(void *b_fifo_buf);
void b_fifo_del(void *b_fifo_buf);


#endif
