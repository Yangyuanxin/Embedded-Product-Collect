
#ifndef __GM_RECORD_H_
#define __GM_RECORD_H_


typedef enum
{
    MDI_RECORD_FAIL = 1,
    MDI_RECORD_SUCCESS,
    MDI_RECORD_TERMINATED,
    MDI_RECORD_DISC_FULL,
    MDI_RECORD_END_OF_FILE,
    MDI_RECORD_NO_SPACE
}record_result_enum;


extern s32 GM_StopRecord(void);
extern s32 GM_StartRecord(void *fileName, kal_uint8 quality, kal_uint32 size_limit, kal_uint32 time_limit);
extern s32 GM_RecordRegister(PsFuncPtr record_cb_func);
extern s32 GM_GetRecordIntensity(u32 channel);

#endif /*__GM_RECORD_H_*/

