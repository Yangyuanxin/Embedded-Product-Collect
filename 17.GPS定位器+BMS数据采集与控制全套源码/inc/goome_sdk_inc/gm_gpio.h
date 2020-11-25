/*****************************************************************************
*  Copyright Statement:
*  --------------------
* www.goome.net
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_gpio.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   GPIO API defines.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/


#ifndef __GM_GPIO_H__
#define __GM_GPIO_H__

#include "gm_type.h"
/****************************************************************************
 * Enumeration for GPIO Pins available.
 ***************************************************************************/
typedef enum{
    GM_GPIO0 = 0,
    GM_GPIO1,
    GM_GPIO2,
    GM_GPIO3,
    GM_GPIO4,
    GM_GPIO5,
    GM_GPIO6,
    GM_GPIO7,
    GM_GPIO8,
    GM_GPIO9,
    GM_GPIO10,
    GM_GPIO11,
    GM_GPIO12,
    GM_GPIO13,
    GM_GPIO14,
    GM_GPIO15,
    GM_GPIO16,
    GM_GPIO17,
    GM_GPIO18,
    GM_GPIO19,
    GM_GPIO20,
    GM_GPIO21,
    GM_GPIO22,
    GM_GPIO23,
    GM_GPIO24,
    GM_GPIO25,
    GM_GPIO26,
    GM_GPIO27,
    GM_GPIO28,
    GM_GPIO29,
    GM_GPIO30,
    GM_GPIO31,
    GM_GPIO32,
    GM_GPIO33,
    GM_GPIO34,
    GM_GPIO35,
    GM_GPIO36,
    GM_GPIO37,
    GM_GPIO38,
    GM_GPIO39,
    GM_GPIO40,
    GM_GPIO41,
    GM_GPIO42,
    GM_GPIO43,
    GM_GPIO44,
    GM_GPIO45,
    GM_GPIO46,
    GM_GPIO47,
    GM_GPIO48,
    GM_GPIO49,
    GM_GPIO50,
    GM_GPIO51,
    GM_GPIO52,
    GM_GPIO53,
    GM_GPIO54,
    GM_GPIO55,
    GM_GPIO56,
    GM_GPIO57,

    GM_GPIO_MAX
    
}Enum_PinName;


#if 1 // Dec-08-2016
#define UART1_RXD_PIN  10
#define UART1_TXD_PIN  11

#define UART2_RXD_PIN  12
#define UART2_TXD_PIN  17

#define UART3_RXD_PIN  0
#define UART3_TXD_PIN  1
#endif



typedef enum{
    PINDIRECTION_IN  = 0,
    PINDIRECTION_OUT = 1
}Enum_PinDirection;

typedef enum{
    PINLEVEL_LOW  = 0,
    PINLEVEL_HIGH = 1
}Enum_PinLevel;

typedef enum{
    PINPULLSEL_DISABLE  = 0,    // Disable pull selection
    PINPULLSEL_PULLDOWN = 1,    // Pull-down 
    PINPULLSEL_PULLUP   = 2     // Pull-up 
}Enum_PinPullSel;




/*****************************************************************
* Function:     GM_GpioInit 
* 
* Description:
*               This function enables the GPIO function of the specified pin,
*               and initialize the configurations, including direction,
*               level and pull selection.
*
* Parameters:
*               pinName:
*                   Pin name, one value of Enum_PinName.
*               dir:
*                   The initial direction of GPIO, one value of Enum_PinDirection.
*               level:
*                   The initial level of GPIO, one value of Enum_PinLevel. 
*               pullSel:
*                   Pull selection, one value of Enum_PinPullSel.
* Return:        
*               GM_RET_OK, this function succeeds.
*               GM_RET_ERR_NOSUPPORTPIN, the input GPIO is invalid. 
*               GM_RET_ERR_PINALREADYSUBCRIBE, the GPIO is in use in
*               other place. For example this GPIO has been using as EINT.
*****************************************************************/
extern s32 GM_GpioInit(Enum_PinName       pinName, 
                 Enum_PinDirection  dir, 
                 Enum_PinLevel      level, 
                 Enum_PinPullSel    pullSel
                 );

/*****************************************************************
* Function:     GM_GpioSetLevel 
* 
* Description:
*               This function sets the level of the specified GPIO.
*
* Parameters:
*               pinName:
*                   Pin name, one value of Enum_PinName.
*               level:
*                   The initial level of GPIO, one value of Enum_PinLevel. 
* Return:        
*               GM_RET_OK, this function succeeds.
*               GM_RET_ERR_NOSUPPORTPIN, the input GPIO is invalid. 
*               GM_RET_ERR_NORIGHTOPERATE, can't operate,Maybe the GPIO not Init
*               GM_RET_ERR_NOGPIOMODE, the input GPIO is not GPIO mode
*****************************************************************/
extern s32 GM_GpioSetLevel(Enum_PinName pinName, Enum_PinLevel level);

/*****************************************************************
* Function:     GM_GpioGetLevel 
* 
* Description:
*               This function gets the level of the specified GPIO.
*
* Parameters:
*               pinName:
*                   Pin name, one value of Enum_PinName.
* Return:        
*               The level value of the specified GPIO, which is 
*               nonnegative integer.
*               GM_RET_ERR_NOSUPPORTPIN, the input GPIO is invalid. 
*****************************************************************/
extern s32 GM_GpioGetLevel(Enum_PinName pinName);





extern char GM_GetGpioMode(kal_uint16 port);

/*****************************************************************
* Function:     GM_GpioSetDirection 
* 
* Description:
*               This function sets the direction of the specified GPIO.
*
* Parameters:
*               pinName:
*                   Pin name, one value of Enum_PinName.
*               dir:
*                   The initial direction of GPIO, one value of Enum_PinDirection.
* Return:        
*               GM_RET_OK indicates this function successes.
*               GM_RET_ERR_NOSUPPORTPIN, the input GPIO is invalid. 
*               GM_RET_ERR_NORIGHTOPERATE, can't operate,Maybe the GPIO not Init
*               GM_RET_ERR_NOGPIOMODE, the input GPIO is not GPIO mode
*****************************************************************/
extern s32 GM_GpioSetDirection(Enum_PinName pinName, Enum_PinDirection dir);

/*****************************************************************
* Function:     GM_GpioGetDirection 
* 
* Description:
*               This function gets the direction of the specified GPIO.
*
* Parameters:
*               pinName:
*                   Pin name, one value of Enum_PinName.
* Return:        
*               The direction of the specified GPIO, which is 
*               nonnegative integer..
*               GM_RET_ERR_NOSUPPORTPIN, the input GPIO is invalid. 
*               GM_RET_ERR_NORIGHTOPERATE, can't operate,Maybe the GPIO not Init
*               GM_RET_ERR_NOGPIOMODE, the input GPIO is not GPIO mode
*****************************************************************/
extern s32 GM_GpioGetDirection(Enum_PinName pinName);

/*****************************************************************
* Function:     GM_GpioSetPullSelection 
* 
* Description:
*               This function sets the pull selection of the specified GPIO.
*
* Parameters:
*               pinName:
*                   Pin name, one value of Enum_PinName.
*               Enum_PinPullSel:
*                   Pull selection, one value of Enum_PinPullSel.
* Return:        
*               GM_RET_OK indicates this function successes.
*               GM_RET_ERR_NOSUPPORTPIN, the input GPIO is invalid. 
*               GM_RET_ERR_NORIGHTOPERATE, can't operate,Maybe the GPIO not Init
*               GM_RET_ERR_NOGPIOMODE, the input GPIO is not GPIO mode
*****************************************************************/
extern s32 GM_GpioSetPullSelection(Enum_PinName pinName, Enum_PinPullSel pullSel);



#endif  // __GM_GPIO_H__

