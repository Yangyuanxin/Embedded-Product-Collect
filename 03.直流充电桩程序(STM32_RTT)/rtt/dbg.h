#ifndef DBG_H_
#define DBG_H_

#include"SEGGER_RTT.h"
int RTT_dbg_msg(const char * sFormat, ...);
int RTT_dbg_error(const char * sFormat, ...);


#if 1
		#define dbg_msg   RTT_dbg_msg
		#define dbg_error RTT_dbg_error
#else
	#define dbg_msg   (void)
	#define dbg_error (void)
#endif

#endif /* DBG_H_ */
