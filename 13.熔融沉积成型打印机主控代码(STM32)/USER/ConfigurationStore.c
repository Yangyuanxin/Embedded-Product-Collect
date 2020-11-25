#include "marlin_main.h"
#include "planner.h"
#include "temperature.h"
#include "ConfigurationStore.h"
#include "usart.h"



//#ifdef EEPROM_SETTINGS
void Config_StoreSettings()  //存储设置
{

  char ver[4]= "V10";
  char ver2[4]=FLASH_SET_EEPROM_VERSION;
//  u32 i=EEPROM_OFFSET; 
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET,ver); // invalidate data first           //100-103    char  						4个字节     版本号              
  FLASH_WRITE_VAR(AXIS_STEPS_PER_UNIT_X_ADDRESS,axis_steps_per_unit);                                //104-119    float 						4*4个字节   4轴电机每mm的步数           M92    
  FLASH_WRITE_VAR(MAX_FEEDRATE_X_ADDRESS,max_feedrate);                                       //120-135   float 						4*4个字节   4轴最大速度                 M202     
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+36,max_acceleration_units_per_sq_second);               //136-151   unsigned long     4*4个字节   4轴最大加速度               M201  
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+52,acceleration);                                       //152-155   float             4个字节     4轴统一默认打印加速度       M204  'S'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+56,retract_acceleration);                               //156-159   float 					  4个字节     4轴统一默认回抽加速度       M204  'T'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+60,minimumfeedrate);                                    //160-163   float 					  4个字节     4轴统一最小速度             M205  'S'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+64,mintravelfeedrate);                                  //164-167   float 					  4个字节     4轴统一最小行程速度         M205  'T'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+68,minsegmenttime);                                     //168-171   unsigned long 		4个字节     4轴统一最小时间us           M205  'B'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+72,max_xy_jerk);                                        //172-175   float 					  4个字节     XY轴不需加速的速度          M205  'X'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+76,max_z_jerk);                                         //176-179   float 					  4个字节     Z轴不需加速的速度           M205  'Z'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+80,max_e_jerk);                                         //180-183   float 					  4个字节     E轴不需加速的速度           M205  'E'
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,add_homeing);                                        //184-187   float 					  4个字节     归位偏差                    M206

    #ifndef ULTIPANEL
//  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,plaPreheatHotendTemp);
//  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,plaPreheatHPBTemp);
//  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,plaPreheatFanSpeed);
//  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,absPreheatHotendTemp);
//  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,absPreheatHPBTemp);
//  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+84,absPreheatFanSpeed);
  #endif
  #ifdef PIDTEMP
    FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+96,Kp);                                              //188-191   float 					    4个字节     归位偏差                    M301   'P'
    FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+100,Ki);                                              //192-195   float 					    4个字节     归位偏差                    M301   'I'
    FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+104,Kd);                                              //196-199   float 					    4个字节     归位偏差                    M301   'D'
  #else
		float dummy = 3000.0f;
    FLASH_WRITE_VAR(i,dummy);
		dummy = 0.0f;
    FLASH_WRITE_VAR(i,dummy);
    FLASH_WRITE_VAR(i,dummy);
  #endif
	 

//  i=EEPROM_OFFSET;
  FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET,ver2); // validate data                             //100-103    char  						4个字节     新版本号      
  SERIAL_ECHO_START;
  printf("Settings Stored\r\n");
}
//#endif //EEPROM_SETTINGS


//#ifdef EEPROM_CHITCHAT
void Config_PrintSettings()//获取设置
{  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
    SERIAL_ECHO_START;
    printf("Steps per unit:\n");
    SERIAL_ECHO_START;
    printf("  M92 X%f",axis_steps_per_unit[0]);
    printf(" Y%f",axis_steps_per_unit[1]);
    printf(" Z%f",axis_steps_per_unit[2]);
    printf(" E%f\r\n",axis_steps_per_unit[3]);

      
    SERIAL_ECHO_START;
    printf("Maximum feedrates (mm/s):\r\n");
    SERIAL_ECHO_START;
    printf("  M203 X%f",max_feedrate[0]);
    printf(" Y%f",max_feedrate[1] ); 
    printf(" Z%f", max_feedrate[2] ); 
    printf(" E%f\r\n", max_feedrate[3]);


    SERIAL_ECHO_START;
    printf("Maximum Acceleration (mm/s2):\r\n");
    SERIAL_ECHO_START;
    printf("  M201 X%ld" ,max_acceleration_units_per_sq_second[0] ); 
    printf(" Y%ld" , max_acceleration_units_per_sq_second[1] ); 
    printf(" Z%ld" ,max_acceleration_units_per_sq_second[2] );
    printf(" E%ld\r\n" ,max_acceleration_units_per_sq_second[3]);
  //  SERIAL_ECHOLN("");
    SERIAL_ECHO_START;
    printf("Acceleration: S=acceleration, T=retract acceleration\r\n");
    SERIAL_ECHO_START;
    printf("  M204 S%f",acceleration ); 
    printf(" T%f\r\n" ,retract_acceleration);
 ///   SERIAL_ECHOLN("");

    SERIAL_ECHO_START;
    printf("Advanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\r\n");
    SERIAL_ECHO_START;
    printf("  M205 S%f",minimumfeedrate ); 
    printf(" T%f" ,mintravelfeedrate ); 
    printf(" B%ld" ,minsegmenttime ); 
    printf(" X%f" ,max_xy_jerk ); 
    printf(" Z%f" ,max_z_jerk);
    printf(" E%f\r\n" ,max_e_jerk);
   // SERIAL_ECHOLN(""); 

    SERIAL_ECHO_START;
    printf("Home offset (mm):\r\n");
    SERIAL_ECHO_START;
    printf("  M206 X%f",add_homeing[0] );
    printf(" Y%f" ,add_homeing[1] );
    printf(" Z%f\n" ,add_homeing[2] );
  //  SERIAL_ECHOLN("");
#ifdef PIDTEMP
    SERIAL_ECHO_START;
    printf("PID settings:\r\n");
    SERIAL_ECHO_START;
    printf("   M301 P%f",Kp); 
    printf(" I%f" ,unscalePID_i(Ki)); 
    printf(" D%f\r\n" ,unscalePID_d(Kd));
  //  SERIAL_ECHOLN(""); 
#endif
} 
//#endif


//#ifdef EEPROM_SETTINGS
void Config_RetrieveSettings()//配置检索设置，从EEPROM读取数据
{
    #ifndef ULTIPANEL
    #endif
//    u32 i=EEPROM_OFFSET;
    char stored_ver[4];
    char ver[4]=FLASH_SET_EEPROM_VERSION;
    FLASH_READ_VAR(FLASH_SET_STORE_OFFSET,stored_ver); //read stored version //读取存储的版本号                         //100-103    char  						4个字节     版本号                      
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << stored_ver << "]");
	  printf("Stored Version: %s , version: %s\n",stored_ver,ver);
    if (strncmp(ver,stored_ver,3) == 0)         //如果ver和stored_ver的前3个字符相同，则返回0
    {       // version number match  //版本号匹配
			
        FLASH_READ_VAR(AXIS_STEPS_PER_UNIT_X_ADDRESS,axis_steps_per_unit);                                //104-119    float 						4*4个字节   4轴电机每mm的步数           M92    
				FLASH_READ_VAR(MAX_FEEDRATE_X_ADDRESS,max_feedrate);                                       //120-135   float 						4*4个字节   4轴最大速度                 M202     
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+36,max_acceleration_units_per_sq_second);               //136-151   unsigned long     4*4个字节   4轴最大加速度               M201  
        
        // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
		reset_acceleration_rates();
        
        FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+52,acceleration);                                       //152-155   float             4个字节     4轴统一默认打印加速度       M204  'S'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+56,retract_acceleration);                               //156-159   float 					  4个字节     4轴统一默认回抽加速度       M204  'T'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+60,minimumfeedrate);                                    //160-163   float 					  4个字节     4轴统一最小速度             M205  'S'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+64,mintravelfeedrate);                                  //164-167   float 					  4个字节     4轴统一最小行程速度         M205  'T'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+68,minsegmenttime);                                     //168-171   unsigned long 		4个字节     4轴统一最小时间us           M205  'B'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+72,max_xy_jerk);                                        //172-175   float 					  4个字节     XY轴不需加速的速度          M205  'X'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+76,max_z_jerk);                                         //176-179   float 					  4个字节     Z轴不需加速的速度           M205  'Z'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+80,max_e_jerk);                                         //180-183   float 					  4个字节     E轴不需加速的速度           M205  'E'
				FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,add_homeing);                                        //184-187   float 					  4个字节     归位偏差                    M206

		#ifndef ULTIPANEL
        //  FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,plaPreheatHotendTemp);
				//  FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,plaPreheatHPBTemp);
				//  FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,plaPreheatFanSpeed);
				//  FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,absPreheatHotendTemp);
				//  FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,absPreheatHPBTemp);
				//  FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+84,absPreheatFanSpeed);
		#endif
        #ifndef PIDTEMP
        float Kp,Ki,Kd;
        #endif
        // do not need to scale PID values as the values in EEPROM are already scaled		
				 FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+96,Kp);                                              //188-191   float 					    4个字节     归位偏差                    M301   'P'
				 FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+100,Ki);                                              //192-195   float 					    4个字节     归位偏差                    M301   'I'
				 FLASH_READ_VAR(FLASH_SET_STORE_OFFSET+104,Kd);                                              //196-199   float 					    4个字节     归位偏差                    M301   'D'
  // Call updatePID (similar to when we have processed M301)
		updatePID();

        SERIAL_ECHO_START;
        printf("Stored settings retrieved\r\n");
    }
    else
    {
        Config_ResetDefault();
    }
    Config_PrintSettings();
}
//#endif

void Config_ResetDefault()//将设置恢复出厂设置
{	u8 i;
    float tmp1[]=DEFAULT_AXIS_STEPS_PER_UNIT;
    float tmp2[]=DEFAULT_MAX_FEEDRATE;
    long tmp3[]=DEFAULT_MAX_ACCELERATION;
    for (i=0;i<4;i++) 
    {
        axis_steps_per_unit[i]=tmp1[i];  
        max_feedrate[i]=tmp2[i];  
        max_acceleration_units_per_sq_second[i]=tmp3[i];
    }
    
    // steps per sq second need to be updated to agree with the units per sq second
    reset_acceleration_rates();
    
    acceleration=DEFAULT_ACCELERATION;
    retract_acceleration=DEFAULT_RETRACT_ACCELERATION;
    minimumfeedrate=DEFAULT_MINIMUMFEEDRATE;
    minsegmenttime=DEFAULT_MINSEGMENTTIME;       
    mintravelfeedrate=DEFAULT_MINTRAVELFEEDRATE;
    max_xy_jerk=DEFAULT_XYJERK;
    max_z_jerk=DEFAULT_ZJERK;
    max_e_jerk=DEFAULT_EJERK;
    add_homeing[0] = add_homeing[1] = add_homeing[2] = 0;
#ifdef ULTIPANEL
    plaPreheatHotendTemp = PLA_PREHEAT_HOTEND_TEMP;
    plaPreheatHPBTemp = PLA_PREHEAT_HPB_TEMP;
    plaPreheatFanSpeed = PLA_PREHEAT_FAN_SPEED;
    absPreheatHotendTemp = ABS_PREHEAT_HOTEND_TEMP;
    absPreheatHPBTemp = ABS_PREHEAT_HPB_TEMP;
    absPreheatFanSpeed = ABS_PREHEAT_FAN_SPEED;
#endif
#ifdef PIDTEMP
    Kp = DEFAULT_Kp;
    Ki = scalePID_i(DEFAULT_Ki);
    Kd = scalePID_d(DEFAULT_Kd);
    
    // call updatePID (similar to when we have processed M301)
    updatePID();
    
#ifdef PID_ADD_EXTRUSION_RATE
    Kc = DEFAULT_Kc;
#endif//PID_ADD_EXTRUSION_RATE
#endif//PIDTEMP

		
		Config_StoreSettings();
				

SERIAL_ECHO_START;
printf("Hardcoded Default Settings Loaded\r\n");

}
