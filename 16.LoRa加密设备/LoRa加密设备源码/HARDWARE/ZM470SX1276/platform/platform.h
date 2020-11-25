#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifndef __GNUC__
	#define inline
#endif
	
#define 		bool 					_Bool
#define 		true 					1
#define 		false 					0
	
//Platform definition
#define 		SX12xxEiger				1

/*!
 * Platform choice. Please uncoment the PLATFORM define and choose your platform
 * or add/change the PLATFORM definition on the compiler Defines option
 */
#define 		PLATFORM				SX12xxEiger

#if( PLATFORM == SX12xxEiger )

	#define USE_SX1276_RADIO
	/*!
	 * Module choice. There are three existing module with the SX1276.
	 * Please set the connected module to the value 1 and set the others to 0
	 */
	#ifdef USE_SX1276_RADIO
		#define MODULE_SX1276RF1IAS					0
		#define MODULE_SX1276RF1JAS					0
		#define MODULE_SX1276RF1KAS					1
	#endif
#endif

#endif










