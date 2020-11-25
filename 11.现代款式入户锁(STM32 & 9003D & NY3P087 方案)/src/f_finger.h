#ifndef __F_FINGER_H__
#define __F_FINGER_H__

int f_finger(unsigned *data);
int f_fingerInit(unsigned *pMsg); 
int f_fingerError(unsigned *pMsg);
int f_fingerSearch(unsigned *pMsg);	  
int f_fingerSearchSuccess(unsigned *pMsg);
int f_fingerSearchFail(unsigned *pMsg);

//int f_RgstfingerBefore(unsigned *pMsg);
int f_RgstfingerInit(unsigned *pMsg);
int f_Rgstfinger(unsigned *pMsg);
int f_RgstfingerDoing(unsigned *pMsg);
//int f_RgstfingerDone(unsigned *pMsg);
int f_fingerEnrollSuccess(unsigned *pMsg);
int f_fingerEnrollFail(unsigned *pMsg);

int f_RgstAdminfingerInit(unsigned *pMsg);
int f_RgstAdminfingerDoing(unsigned *pMsg);
int f_RgstAdminfingerDone(unsigned *pMsg);

//int f_DegstfingerInit(unsigned *pMsg);
int f_fingerDeleteInit(unsigned *pMsg);
int f_fingerDelete(unsigned *pMsg);
int f_fingerDeleteDoing(unsigned *pMsg);
int f_fingerDeleteDone(unsigned *pMsg);

//int f_fingerInitAll(unsigned *pMsg);
int f_fingerDelAllFP(unsigned *pMsg);
//int f_fingerInitAllDoing(unsigned *pMsg);
int f_fingerDelAllFPDoing(unsigned *pMsg);
//int f_fingerInitAllDone(unsigned *pMsg);
int f_fingerDelAllFPDone(unsigned *pMsg);

int f_keyFirstTouch(unsigned *pMsg);
//int f_lockActionDone(unsigned *pMsg);
int f_fingerNoFPWarn(unsigned *pMsg);

#endif
