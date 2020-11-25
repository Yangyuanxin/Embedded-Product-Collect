

#ifndef __GM_AIDINGEPO_H__
#define __GM_AIDINGEPO_H__


extern s32 GM_EpoInit(PsFuncPtr epocallBack);

extern s32  GM_GpsSendCmd(char* cmd);//给GPS发送指令，不用加校验

extern void GM_GpsUtcTime (int iYr,int iMo,int iDay,int iHr,int iMi,int iSe) ;

//设置 GPS时间 UTC
extern void GM_GpsEpoPos(float lat,float lon,int alt);

// 设置 参考坐标,经度,纬度,范围, 年,月,天,时,分,秒
extern void GM_GpsPwrOn(void);

extern void GM_GpsPwrOff(void);

#endif

