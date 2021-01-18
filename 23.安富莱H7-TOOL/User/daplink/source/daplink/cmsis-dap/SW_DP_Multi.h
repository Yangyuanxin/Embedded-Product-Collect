/*
    SW_DP_Multi.h
*/

#ifndef __SW_DAP_MULTI_H__
#define __SW_DAP_MULTI_H__

//#ifdef __cplusplus
//extern "C" {
//#endif

//#include "flash_blob.h"
//#include "target_reset.h"
//#ifdef TARGET_MCU_CORTEX_A
//#include "debug_ca.h"
//#else
//#include "debug_cm.h"
//#endif

typedef struct
{
    uint8_t MultiMode;          /* 0表示单机模式, 1表示1拖2, 2表示1拖3, 3表示1拖4 */
    uint8_t Active[4];
    uint8_t Error[4];
    uint8_t AckOk[4];
    
    uint8_t Ignore[4];
    uint8_t TempIgnore[4];      /* 临时忽略 */
    
    uint32_t DIR_Pins;
    uint32_t SWDIO_Pins;
    uint32_t SWCLK_Pins;
    uint32_t MODER_Mask;
    uint32_t MODER_Out;
    uint32_t CLK_0_DIO_0;
    uint32_t CLK_0_DIO_1;
    
    uint32_t CoreID[4];
    
}MUL_SWD_T;

extern MUL_SWD_T g_gMulSwd;

uint8_t *MUL_SWD_Transfer(uint32_t request, uint32_t *data);
void MUL_SWJ_Sequence (uint32_t count, const uint8_t *data);

void MUL_SWD_GPIOConfig(void);

void MUL_RefreshGpioParam(void);
void MUL_SEND_32BIT(uint32_t val);

void MUL_PORT_SWD_SETUP(void);

//#ifdef __cplusplus
//}
//#endif

#endif  /* __DAP_H__ */
