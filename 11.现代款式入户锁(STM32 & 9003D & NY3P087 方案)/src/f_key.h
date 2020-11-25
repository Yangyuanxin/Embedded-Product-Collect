#ifndef __F_KEY_H__
#define __F_KEY_H__

int f_key(unsigned *pMsg);
int f_keyFuncInvalid(unsigned *pMsg);
//int f_keyFuncSwitch(unsigned *pMsg);
int f_keyInput(unsigned *pMsg);
int f_keyInputDone(unsigned *pMsg);
int f_KEYToOut(unsigned *pMsg);
int f_KEYSET(unsigned *pMsg);
int f_KEYSETINVALID(unsigned *pMsg);
int f_keyLowPower(unsigned *pMsg);
int f_keyRestorFactorySetting(unsigned *pMsg);
int f_keyRestorFactorySettingDone(unsigned *pMsg);
//int f_KEYAdminId(unsigned *pMsg);
//int f_KEYAdminIdInit(unsigned *pMsg);
//int f_keyAdminSelect(unsigned *pMsg);
int f_keyAdminRgstPSWord(unsigned *pMsg);
int f_keyAdminDergstPSWord(unsigned *pMsg);	
int f_keyAdminAuthen(unsigned *pMsg);
int f_keyAdminRgstPSWordDone(unsigned *pMsg);
//int f_keyFuncSelect(unsigned *pMsg);
int f_keyUnlockDoing(unsigned *pMsg);
int f_keyUnlockFail(unsigned *pMsg);


//int f_testKeyCookedMode(unsigned *pMsg);
//int f_testMode(unsigned *pMsg);

#endif
