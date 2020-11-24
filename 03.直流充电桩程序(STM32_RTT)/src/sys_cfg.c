#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "sys_cfg.h"
#include "sflash.h"
#include "string.h"

//校表参数
meas_adj_def meas_adj;  //保存到AT45DB 第0页
//系统配置参数
sys_cfg_def sys_cfg;  //保存到AT45DB 第1页


const meas_adj_def meas_adj_default =
{
		0xAAAA,
		0,
		100000,
		0,
		100000
};

const sys_cfg_def sys_cfg_default =
{
		0xAAAA,
		123456,  //系统密码
		1, //ID
		100,  //e_price

		192,  //本机IP
		168,
		1,
		10,

		192,  //网关IP
		168,
		1,
		1,

		255,    //子网掩码
		255,
		255,
		0,

		192,  //远程IP
		168,
		1,
		1,

		8080,  //远程端口
};


/**
  * @brief  读取系统设置
  * @retval 0 成功   <0 失败
  */
int sys_cfg_read()
{
	u16 len = sizeof(sys_cfg_def);
	u8* addr = (u8*)&sys_cfg;
	sflash_read(1, 0, addr, len);
	if(sys_cfg.valid != 0xAAAA)
	{
		memcpy(addr, &sys_cfg_default, len);  //设置为默认参数
	}
	return 0;
}

/**
  * @brief  保存系统设置
  * @retval 0 成功   <0 失败
  */
int sys_cfg_save()
{
	u16 len = sizeof(sys_cfg_def);
	u8* addr = (u8*)&sys_cfg;
	sys_cfg.valid = 0xAAAA;
	sflash_write(1, 0, addr, len);
	return 0;
}



/**
  * @brief  读取AD校正参数
  * @retval 0 成功   <0 失败
  */
int meas_adj_read()
{
	u16 len = sizeof(meas_adj_def);
	u8* addr = (u8*)&meas_adj;
	sflash_read(0, 0, addr, len);
	if(meas_adj.valid != 0xAAAA)
	{
		memcpy(addr, &meas_adj_default, len);  //设置为默认参数
	}
	return 0;
}

/**
  * @brief  保存AD校正参数
  * @retval 0 成功   <0 失败
  */
int meas_adj_save()
{
	u16 len = sizeof(meas_adj_def);
	u8* addr = (u8*)&meas_adj;
	meas_adj.valid = 0xAAAA;
	sflash_write(0, 0, addr, len);
	return 0;
}




#if 0
//配置保存地址
//107系列2K一页,共128页   配置保存在最后一页
#define CONFIG_ADDR (0x08000000 + 2048 * 127)

void sys_cfg_read()
{
	u16 len = sizeof(sys_cfg_def) / 2;
	u16* addr = (u16*)&sys_cfg;
	volatile u16 *cfg_addr = (volatile u16*)CONFIG_ADDR;

	while(len--)
	{
		*addr++ = *cfg_addr++;
	}
}


void sys_cfg_save()
{
	u16 len = sizeof(sys_cfg_def) / 2;
	u16* addr = (u16*)&sys_cfg;
	volatile u16 *cfg_addr = (volatile u16*)CONFIG_ADDR;

	FLASH_Unlock();

	FLASH_ErasePage(CONFIG_ADDR);  //先擦除
	while(len--)
	{
		FLASH_ProgramHalfWord((u32)cfg_addr, *addr);  //写
		cfg_addr++;
		addr++;
	}

	FLASH_Lock();
}



//电压电流测量 校正参数
#define MEAS_ADJ_ADDR (0x08000000 + 2048 * 127)

void meas_adj_read()
{
	u16 len = sizeof(meas_adj_def) / 2;
	u16* addr = (u16*)&meas_adj;
	volatile u16 *adj_addr = (volatile u16*)MEAS_ADJ_ADDR;

	while(len--)
	{
		*addr++ = *adj_addr++;
	}
}


void meas_adj_save()
{
	u16 len = sizeof(meas_adj_def) / 2;
	u16* addr = (u16*)&meas_adj;
	volatile u16 *adj_addr = (volatile u16*)MEAS_ADJ_ADDR;

	FLASH_Unlock();

	FLASH_ErasePage(MEAS_ADJ_ADDR);  //先擦除
	while(len--)
	{
		FLASH_ProgramHalfWord((u32)adj_addr, *addr);  //写
		adj_addr++;
		addr++;
	}

	FLASH_Lock();
}

#endif
