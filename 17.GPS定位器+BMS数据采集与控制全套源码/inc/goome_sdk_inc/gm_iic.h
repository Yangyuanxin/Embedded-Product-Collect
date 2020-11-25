
#ifndef __GM_IIC_H__
#define __GM_IIC_H__

extern s32 GM_I2cSetMode(kal_uint8 i2c_mode);
extern s32 GM_I2cGetMode(void);
extern BOOL GM_I2cReadBytes(u8 RegAddr, u8 *pBuffer, u16 len);
extern BOOL GM_I2cWriteBytes(u8 RegAddr, u8 *pData, u16 len);
extern BOOL GM_I2cWriteByte(kal_uint8 RegAddr, kal_uint8 Data);
extern s32  GM_I2cConfig(u8 slaveAddr, u32 speed);
extern s32 GM_I2cInit(kal_uint8 is_hw, kal_uint8 slaveAddr_R, kal_uint8 slaveAddr_W, kal_uint8 pin_scl, kal_uint8 pin_sda);

#endif  /*__GM_IIC_H__*/

