#include "call.h"


static GM_ERRCODE gm_call_status_call_back(void* call_status);


static GM_ERRCODE gm_call_status_call_back(void* call_status)
{
    int* status =(int*) call_status;
        
    switch(*status)
    {
        case 0:
            //phone calling
            break;
        case 1:
            //call OK
            break;
        case 2:
            //call hang 
            break;
    }
}



static GM_ERRCODE gm_call_receive_call_back(void* evt)
{
    u8* phone_number =(u8*) evt;

    printf("RING:%.20s", phone_number);

    GM_CallAnswer(gm_call_status_call_back);

    return GM_SUCCESS;
}



GM_ERRCODE gm_make_phone_call(const u8 *dist_number)
{
    if (NULL == dist_number)
    {
        return GM_PARAM_ERROR;
    }

    if ((GSM_CREG_REGISTER_LOCAL != gm_gsm_get_creg_status()) && (GSM_CREG_REGISTER_ROAM != gm_gsm_get_creg_status()))
    {
        return GM_ERROR_STATUS;
    }
    
    GM_MakeCall(dist_number, gm_call_status_call_back);

    return GM_SUCCESS;
}



GM_ERRCODE call_create(void)
{
    GM_RegisterCallBack(GM_CB_CALL_RECEIVE, gm_call_receive_call_back);
    
    return GM_SUCCESS;
}


GM_ERRCODE call_destroy(void)
{
    GM_RegisterCallBack(GM_CB_CALL_RECEIVE, 0);
    
    return GM_SUCCESS;
}


