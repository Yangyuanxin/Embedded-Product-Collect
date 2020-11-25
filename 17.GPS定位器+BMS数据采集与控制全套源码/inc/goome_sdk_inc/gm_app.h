
#ifndef  _MAIN_H_
#define  _MAIN_H_
#include "gm_type.h"
#include "string.h"
/**----------------------------------------------------------------------------*
 **                         Dependencies                                       *
 **---------------------------------------------------------------------------*/
typedef struct _dll_func_entry
{
	U32 Func_Entry_Code;
	union
	{
		U32 Func_Index;
		U32 Func_RunTime_Addr;
	} Func_Info;
}dll_func_entry;


typedef struct _dll_header_struct
{
	U32 version;
	U32 feature;
	U32 App_Main_Entry_Offset;
	U32 APP_RO_Size;
	U32 APP_RW_Size;
	U32 APP_ZI_Size;
	dll_func_entry* APP_func_entry;
} dll_header_struct;


typedef struct _dll_struct
{
	dll_header_struct header;
	void* mem_block;
	S32 app_code_base_addr;
	S32 app_data_base_addr;
	S32 app_zi_base_addr;
	S32 app_main_entry_addr;
	S32 export_func_count;
	S32* export_funcs;
	
}dll_struct;

typedef void (*DLL_main_func)(dll_struct* dll);

#endif //_MAIN_H_

