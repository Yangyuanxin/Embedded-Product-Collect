#include "stm32f10x.h"
#include "filter.h"



/*
 * @brief  排序  小到大
 * @param  buf: 数据
 * @param  len: 数据长度
 */
static void sort(u16 *buf, u8 len)
{
	u8 i,j;
	u16 temp;

	for (j = 0; j < len - 1; j++) //排序  小到大
	{
		for (i = j + 1; i < len; i++)
		{
			if (buf[j] > buf[i])
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}
}


/*
 * @brief  中位值平均滤波
 * @param  buf: 数据
 * @param  len: 数据长度
 * @param  offset: 排序后数组前后要去掉的元素个数  offset < len / 2
 */
static u16 filter_median_value(u16 *buf, u8 len, u8 offset)
{
	u8 i;
	u32 temp = 0;
	sort(buf, len);  //先排序

	for(i = offset; i < len - offset; i++)  //把中间值累加
	{
		temp += buf[i];
	}
	return (u16)(temp / (len - offset * 2));  //平均
}


/*
 * @brief  滑动滤波
 * @param  buf: 数据
 * @param  len: 数据长度
 * @param  new: 新插入的值
 * @param  offset: 排序后数组前后要去掉的元素个数  offset < len / 2
 * @retval 0:成功   <0 失败
 */
static u16 filter_glide(u16 *buf, u8 len, u16 new)
{
	u8 i;
	u32 temp = 0;
	for(i = 0; i < len - 1; i++)  //去掉最旧
	{
		buf[i] = buf[i + 1];
	}
	buf[len - 1] = new;  //最新值

	for(i = 0; i < len; i++)  //把中间值累加
	{
		temp += buf[i];
	}
	return (u16)(temp / len);  //平均
}


/*
 * @brief  滑动平均滤波
 * @param  buf: 数据
 * @param  len: 数据长度
 * @param  new: 新插入的值
 * @param  offset: 排序后数组前后要去掉的元素个数  offset < len / 2
 * @retval 0:成功   <0 失败
 */
static u16 filter_glide_ave(u16 *buf, u8 len, u16 new, u8 offset)
{

}

void filter_calc(filter_def *f)
{
//	u8 count,i,j;
//	u32 temp;
//
//	for(j = 0; j < f->flt1_buf_len - 1; j++) //排序  小到大
//	{
//		for (i = j + 1; i < f->flt1_buf_len; i++)
//		{
//			if (f->flt1_buf[j] > f->flt1_buf[i])
//			{
//				temp = f->flt1_buf[i];
//				f->flt1_buf[i] = f->flt1_buf[j];
//				f->flt1_buf[j] = temp;
//			}
//		}
//	}
//
//	temp = 0;
//
//	for(count = f->flt1_cnt; count < f->flt1_buf_len - f->flt1_cnt; count++)  //中值滤波  去掉数组前后各f->flt1_cnt个数据  中间值相加示平均
//	{
//		temp += f->flt1_buf[count];
//	}
//
//	temp = (u16)(temp / (f->flt1_buf_len - f->flt1_cnt * 2));
//
//	f->value1 = (u16)temp;  //保存第一次结果
//
//	if(f->flt2_buf != ((void*)0))  //不需要滑动滤波
//	{
//		return;
//	}
//
//	for(j = 0; j < f->flt2_buf_len - 1; j++) //去掉最旧的数据
//	{
//		f->flt2_buf[j] = f->flt2_buf[j + 1];
//	}
//
//	f->flt2_buf[f->flt2_buf_len - 1] = f->value1;  //在最后插入最新的值
//
//	temp = 0;
//
//	for(j = 0; j < f->flt2_buf_len; j++) //示平均
//	{
//		temp += f->flt2_buf[j];
//	}
//
//	temp /= f->flt2_buf_len;
//
//	f->value2 = (u16)temp;
}









