#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "uIPMain.h"

#if 1
#define ETH_MSG DBG_MSG
#else
#define ETH_MSG (void)
#endif


__task void task_eth(void)
{
	uIPMain();
}



//#include <Net_Config.h>
//
//
///*
// * 以太网时钟
// */
//__task void timer_task(void)
//{
//	os_itv_set(10);
//	while (1)
//	{
//		timer_tick();
//		//tick = __TRUE;
//		os_itv_wait ();
//	}
//}
//
//
//
//#define Device_ID1  (*(u32*)(0x1FFFF7E8))
//#define Device_ID2  (*(u32*)(0x1FFFF7EC))
//#define Device_ID3  (*(u32*)(0x1FFFF7F0))
//extern u8 own_hw_adr[6];  //Net_lib.c
//
///*
// * 以太网 优先级应设最低 因为会一直RADY
// */
//__task void Task_ETH (void)
//{
//	own_hw_adr[0] = 0x88;  //必须为偶数
//	own_hw_adr[1] = (u8)(Device_ID1);
//	own_hw_adr[2] = (u8)(Device_ID1 >> 16);
//	own_hw_adr[3] = (u8)(Device_ID2);
//	own_hw_adr[4] = (u8)(Device_ID2 >> 16);
//	own_hw_adr[5] = (u8)(Device_ID3);
//
//	init_TcpNet();
//
//	os_tsk_create (timer_task, 30);
//
//	while (1)
//	{
//		main_TcpNet();
//		//dhcp_check ();
//		os_tsk_pass();
//	}
//}

















