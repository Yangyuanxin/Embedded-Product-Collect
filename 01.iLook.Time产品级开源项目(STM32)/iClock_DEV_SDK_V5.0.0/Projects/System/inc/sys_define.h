#ifndef __SYS_DEFINE_H
#define __SYS_DEFINE_H

#define LCM_WIDTH		128
#define LCM_HIGH		32



typedef enum _GPIO_STATE
{
    LOW,
    HIGH,
}GPIO_STATE;


typedef enum _ONOFF_STATE
{
    ON,
    OFF,
}ONOFF_STATE;

typedef enum _IO_STATE
{
    IN,
    OUT,
}IO_STATE;

typedef enum _BOOL
{
  false=0,
  true=!false,
}BOOL;

#endif





