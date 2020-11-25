#ifndef __GM_EINT_H__
#define __GM_EINT_H__

#include "gm_gpio.h"

#define EDGE_SENSITIVE           	KAL_TRUE
#define LEVEL_SENSITIVE          	KAL_FALSE


typedef enum{
    GM_EINT0 = 0,
    GM_EINT1,
    GM_EINT2,
    GM_EINT3,
    GM_EINT4,
    GM_EINT5,
    GM_EINT6,
    GM_EINT7,
    GM_EINT8,
    GM_EINT9,
    GM_EINT10,
    GM_EINT11,
    GM_EINT12,
    GM_EINT13,
    GM_EINT14,
    GM_EINT15,
    GM_EINT16,
    GM_EINT17,
    GM_EINT18,
    GM_EINT19,
    GM_EINT20,
    GM_EINT21,
    GM_EINT22,
    GM_EINT23,
    GM_EINT24,

    GM_EINT_MAX
}Enum_EintName;


extern s32 GM_EintPinInit(Enum_PinName pinName);
extern s32 GM_EintSetPolarity(Enum_EintName eintPinName,BOOL eint_polarity);
extern s32 GM_EintRegister(Enum_EintName eintPinName, PsFuncPtr callback_eint, BOOL eint_polarity, BOOL sens);



#endif

