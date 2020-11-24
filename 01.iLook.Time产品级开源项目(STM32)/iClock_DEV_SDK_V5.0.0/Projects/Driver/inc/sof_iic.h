#ifndef __SOF_IIC_H
#define __SOF_IIC_H

typedef enum _I2C_STATE{
  I2C_IDLE =  0,
  I2C_BUSY =  1,
  I2C_OK   =  2,
  I2C_NOACK = 3,
  I2C_ACK   = 4,
  I2C_ERROR =  -1
}I2C_STATE;

//
void I2C_Init(void);
I2C_STATE I2C_Start(void);
I2C_STATE I2C_CheckBusy(void); 
I2C_STATE I2C_WaitAck(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
//
uint8_t I2C_ReceiveByte(void);
void I2C_SendByte(uint8_t SendByte);


#endif


