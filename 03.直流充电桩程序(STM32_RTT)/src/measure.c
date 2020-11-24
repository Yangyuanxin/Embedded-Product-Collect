#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "measure.h"
#include "sys_cfg.h"
#include "string.h"
#include "delay.h"
#include "io.h"

#if 1
#define MEAS_MSG dbg_msg
#else
#define MEAS_MSG (void)
#endif

#define ADC_CH_CNT 2  //ADC通道数
#define ADC_VALUE_CNT 30  //每通道保存值个数

#define ADC_BUF_LEN (ADC_CH_CNT * ADC_VALUE_CNT)  //DMA缓冲区数据长度
static u16 adc_buf[ADC_BUF_LEN];  //DMA缓冲区


#define ADC_FILTER_CNT 20  //滑动平均滤波数据长度
static u16 filter_v[ADC_FILTER_CNT];
static u16 filter_i[ADC_FILTER_CNT];


meas_def meas_info;


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
 * @param  offset: 数据地址  0:电压   1:电流
 */
static u16 ad_filter(u8 offset)
{
	u8 count;
	u16 value_buf[ADC_VALUE_CNT];
	u32  sum = 0;
	for(count = 0; count < ADC_VALUE_CNT; count++)  //读取一通道数据
	{
		value_buf[count] = adc_buf[offset + count * ADC_CH_CNT];
	}

	sort(value_buf, ADC_VALUE_CNT);  //排序  小到大

#define N 10

	for (count = N; count < ADC_VALUE_CNT - N; count++)
	{
		sum += value_buf[count];
	}

	return (u16)(sum / (ADC_VALUE_CNT - N * 2));
}



/*
 * @brief  滑动平均滤波
 * @param  new: 新插入的值
 * @param  offset:   0:电压   1:电流
 * @retval 0:成功   <0 失败
 */
static u16 filter_glide_ave(u8 offset, u16 new)
{
	u8 i;
	u32 sum = 0;
	u16 temp_buf[ADC_FILTER_CNT];
	if(offset == 0)  //电压
	{
		for(i = 0; i < ADC_FILTER_CNT - 1; i++)  //去掉最旧
		{
			filter_v[i] = filter_v[i + 1];
		}
		filter_v[ADC_FILTER_CNT - 1] = new;  //最新值

		memcpy(temp_buf, filter_v, ADC_FILTER_CNT * 2);
	}
	else  //电流
	{
		for(i = 0; i < ADC_FILTER_CNT - 1; i++)  //去掉最旧
		{
			filter_i[i] = filter_i[i + 1];
		}
		filter_i[ADC_FILTER_CNT - 1] = new;  //最新值

		memcpy(temp_buf, filter_i, ADC_FILTER_CNT * 2);
	}

	sort(temp_buf, ADC_FILTER_CNT);  //排序  小到大

#define M 5

	for(i = M; i < ADC_FILTER_CNT - M; i++)
	{
		sum += temp_buf[i];
	}

	return (u16)(sum / (ADC_FILTER_CNT - M * 2));
}



/*
 *
 */
static void adc_conv_init()
{
	//ADC1 DMA 接收数据地址设置
	DMA1_Channel1->CNDTR = ADC_BUF_LEN;
	DMA1_Channel1->CMAR = (u32)adc_buf;
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //
}

//void dis()
//{
//	DMA_Cmd(DMA1_Channel1, DISABLE);
//	ADC_SoftwareStartConvCmd(ADC1, DISABLE);  //
//}

u32 delay_fun_off;
/*
 * ADC转换测量输出电压 电流
 */
void task_meas()
{
	u16 adc;
	int temp;
	int temp_v;
	u8 fun_on = 0;
	u32 delay_off = 0, delay_on = 0;
	adc_conv_init();
	os_dly_wait(100);

	while(1)
	{
		adc = ad_filter(0);
		adc = filter_glide_ave(0, adc);
		meas_info.ad_v = adc;  //AD值 12bit

		temp = meas_adj.v_offset - adc;
		meas_info.v = ((temp * meas_adj.v_gain) * 1000);

		temp_v = meas_info.v;
		if(temp_v < 0)
		{
			temp_v = -temp_v;
		}
 		if(temp_v < 50000)  //小于50V不显示
 		{
 			meas_info.v = 0;
 		}

		adc = ad_filter(1);
		adc = filter_glide_ave(1, adc);
		meas_info.ad_i = adc;
		meas_info.i = ((adc * meas_adj.i_gain) * 1000);

		if(meas_info.v == 0)
		{
			meas_info.i = 0;
		}

		//MEAS_MSG("adc %d  %d\n", meas_info.ad_v, meas_info.ad_i);
		os_dly_wait(200);
	}
}











