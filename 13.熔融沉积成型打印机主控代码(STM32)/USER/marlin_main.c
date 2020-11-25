#include "marlin_main.h"


//#include "lcd.h" //屏幕目前尚不能使用，因为屏幕的SPI通信和W25Q16的SPI通信的片选信号都接的是PA4

/*  // G/M代码的解释
//Implemented Codes
//-------------------
// G0  -> G1
// G1  - Coordinated Movement X Y Z E
// G2  - CW ARC
// G3  - CCW ARC
// G4  - Dwell S<seconds> or P<milliseconds>
// G10 - retract filament according to settings of M207
// G11 - retract recover filament according to settings of M208
// G28 - Home all Axis
// G90 - Use Absolute Coordinates
// G91 - Use Relative Coordinates
// G92 - Set current position to cordinates given

//RepRap M Codes
// M0   - Unconditional stop - Wait for user to press a button on the LCD (Only if ULTRA_LCD is enabled)
// M1   - Same as M0
// M104 - Set extruder target temp
// M105 - Read current temp
// M106 - Fan on
// M107 - Fan off
// M109 - Wait for extruder current temp to reach target temp.
// M114 - Display current position

//Custom M Codes
// M15  - update flash data (font data ; icon data and so on)
// M16  - screen_adjust
// M17  - Enable/Power all stepper motors 
// M18  - Disable all stepper motors; same as M84
// M20  - List SD card
// M21  - Init SD card
// M22  - Release SD card
// M23  - Select SD file (M23 filename.g)
// M24  - Start/resume SD print
// M25  - Pause SD print
// M26  - Set SD position in bytes (M26 S12345)
// M27  - Report SD print status
// M28  - Start SD write (M28 filename.g)
// M29  - Stop SD write
// M30  - Delete file from SD (M30 filename.g)
// M31  - Output time since last M109 or SD card start to serial
// M42  - Change pin status via gcode Use M42 Px Sy to set pin x to value y, when omitting Px the onboard led will be used.
// M80  - Turn on Power Supply
// M81  - Turn off Power Supply
// M82  - Set E codes absolute (default)
// M83  - Set E codes relative while in Absolute Coordinates (G90) mode
// M84  - Disable steppers until next move,
//        or use S<seconds> to specify an inactivity timeout, after which the steppers will be disabled.  S0 to disable the timeout.
// M85  - Set inactivity shutdown timer with parameter S<seconds>. To disable set zero (default)
// M92  - Set axis_steps_per_unit - same syntax as G92
// M114 - Output current position to serial port
// M115 - Capabilities string
// M117 - display message
// M119 - Output Endstop status to serial port
// M126 - Solenoid Air Valve Open (BariCUDA support by jmil)
// M127 - Solenoid Air Valve Closed (BariCUDA vent to atmospheric pressure by jmil)
// M128 - EtoP Open (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M129 - EtoP Closed (BariCUDA EtoP = electricity to air pressure transducer by jmil)
// M140 - Set bed target temp
// M190 - Wait for bed current temp to reach target temp.
// M200 - Set filament diameter
// M201 - Set max acceleration in units/s^2 for print moves (M201 X1000 Y1000)
// M202 - Set max acceleration in units/s^2 for travel moves (M202 X1000 Y1000) Unused in marlin_main!!
// M203 - Set maximum feedrate that your machine can sustain (M203 X200 Y200 Z300 E10000) in mm/sec
// M204 - Set default acceleration: S normal moves T filament only moves (M204 S3000 T7000) im mm/sec^2  also sets minimum segment time in ms (B20000) to prevent buffer underruns and M20 minimum feedrate
// M205 -  advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk, E=maximum E jerk
// M206 - set additional homeing offset
// M207 - set retract length S[positive mm] F[feedrate mm/sec] Z[additional zlift/hop]
// M208 - set recover=unretract length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
// M209 - S<1=true/0=false> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
// M218 - set hotend offset (in mm): T<extruder_number> X<offset_on_X> Y<offset_on_Y>
// M220 S<factor in percent>- set speed factor override percentage
// M221 S<factor in percent>- set extrude factor override percentage
// M240 - Trigger a camera to take a photograph
// M280 - set servo position absolute. P: servo index, S: angle or microseconds
// M300 - Play beepsound S<frequency Hz> P<duration ms>
// M301 - Set PID parameters P I and D
// M302 - Allow cold extrudes
// M303 - PID relay autotune S<temperature> sets the target temperature. (default target temperature = 150C)
// M304 - Set bed PID parameters P I and D
// M400 - Finish all moves
// M500 - stores paramters in EEPROM
// M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
// M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
// M503 - print the current settings (from memory not from eeprom)
// M540 - Use S[0|1] to enable or disable the stop SD card print on endstop hit (requires ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
// M600 - Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
// M907 - Set digital trimpot motor current using axis codes.
// M908 - Control digital trimpot directly.
// M350 - Set microstepping mode.
// M351 - Toggle MS1 MS2 pins directly.
// M928 - Start SD logging (M928 filename.g) - ended by M29
// M999 - Restart after being stopped by error
*/

//Stepper Movement Variables

//===========================================================================
//=============================public variables=============================
//===========================================================================
float homing_feedrate[] = HOMING_FEEDRATE;
bool axis_relative_modes[] = AXIS_RELATIVE_MODES;
int feedmultiply=100; //100->1 200->2
int saved_feedmultiply;
int extrudemultiply=100; //100->1 200->2
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };

float stop_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };

float add_homeing[3]={0,0,0};
float min_pos[3] = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };  //定义打印机大小
float max_pos[3] = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };

static const float  base_min_pos[3]   = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS };  
static float  base_max_pos[3]   = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
static const float  base_home_pos[3]  = { X_HOME_POS, Y_HOME_POS, Z_HOME_POS };
static float  max_length[3]	    = { X_MAX_LENGTH, Y_MAX_LENGTH, Z_MAX_LENGTH };
static const float  home_retract_mm[3]= { X_HOME_RETRACT_MM, Y_HOME_RETRACT_MM, Z_HOME_RETRACT_MM };
static const signed char home_dir[3]  = { X_HOME_DIR, Y_HOME_DIR, Z_HOME_DIR }; //定义方向

// Extruder offset, only in XY plane
#if EXTRUDERS > 1
float extruder_offset[2][EXTRUDERS] = {
#if defined(EXTRUDER_OFFSET_X) && defined(EXTRUDER_OFFSET_Y)
  EXTRUDER_OFFSET_X, EXTRUDER_OFFSET_Y
#endif
};
#endif
uint8_t active_extruder = 0;
int fanSpeed=0;
#ifdef BARICUDA
int ValvePressure=0;
int EtoPPressure=0;
#endif

#ifdef FWRETRACT
  bool autoretract_enabled=true;
  bool retracted=false;
  float retract_length=3, retract_feedrate=17*60, retract_zlift=0.8;
  float retract_recover_length=0, retract_recover_feedrate=8*60;
#endif
//===========================================================================
//=============================private variables=============================
//===========================================================================
const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};  //轴编码
 float destination[NUM_AXIS] = {  0.0, 0.0, 0.0, 0.0};
float offset[3] = {0.0, 0.0, 0.0};
static bool home_all_axis = true;
static float feedrate = 1500.0, next_feedrate, saved_feedrate;
static long gcode_N, gcode_LastN, Stopped_gcode_LastN = 0;

volatile static bool relative_mode = false;  //Determines Absolute or Relative Coordinates

static char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];
volatile static bool fromsd[BUFSIZE];
static int bufindr = 0;
static int bufindw = 0;
static int buflen = 0;
//static int i = 0;
static char serial_char;
static int serial_count = 0;
static bool comment_mode = false;
static char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z, E, etc

//Inactivity shutdown variables
static unsigned long previous_millis_cmd = 0;
static unsigned long max_inactive_time = 0;
static unsigned long stepper_inactive_time = DEFAULT_STEPPER_DEACTIVE_TIME*1000L;
//static unsigned long hotend_inactive_time = 120*1000L;//120*1000ms

unsigned long starttime=0;
unsigned long stoptime=0;
static u8 tmp_extruder;


bool Stopped=false;

bool poweroff_flag = false;
static uint32_t nextRecordMillis = 0; //用来记录下次向flash写入参数的时间
bool lcd_contiune_print_after_poweroff; //断电后继续打印的标志位
unsigned long poweroff_sdpos;
float poweroff_position_z;
char poweroff_printing_filename[30];
char cmd_position_z[20];


//uint8_t duandianxuda = 1; //前期测试用的

//#if NUM_SERVOS > 0
//  Servo servos[NUM_SERVOS];
//#endif
//===========================================================================
//=============================ROUTINES=============================
//===========================================================================

void get_arc_coordinates(void);
bool setTargetedHotend(int code);


void enquecommand(const char *cmd)
{
  if(buflen < BUFSIZE)
  {
    //this is dangerous if a mixing of serial and this happsens
    strcpy(&(cmdbuffer[bufindw][0]),cmd);
    SERIAL_ECHO_START;
    printf("enqueing \"%s\"",cmdbuffer[bufindw]);
    bufindw= (bufindw + 1)%BUFSIZE;
    buflen += 1;
  }
}


void setup(void)
{
//	delay_init();	    	//延时函数初始化
//	LED_Init();
//	BEEP_Init();
	
	//NVIC_Configuration(); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	TIM2_Int_Init(99,7199);
//	TIM3_Int_Init(9,7199);//用于记录系统时间 //1s的时钟频率72M /（7199+1）=10Khz  //定时周期为1ms
//	uart1_init(BAUDRATE);	 	//串口波特率初始化为115200
//  LED_Blink(10); //闪灯20次，指示开机，后面可改为蜂鸣器
	//BEEP_Ring();	
	
	// loads data from EEPROM if available else uses defaults (and resets step acceleration rate)
  //Config_RetrieveSettings();
  Config_ResetDefault();
  Config_PrintSettings();
  st_init();    // Initialize stepper, this enables interrupts
  tp_init();    // Initialize 
  plan_init();  // Initialize planner;
	
	lcd_update();
}

 void loop(void)
{
	if(buflen < (BUFSIZE-1))
		 { 
			 get_command();
		 }
     #ifdef SDSUPPORT
		 if(SD_CD)
		 {
		  card.cardOK = false;
		 }
		card_checkautostart(false); 
		#endif
		 if(buflen) 
		 {
				#ifdef SDSUPPORT						
			 
					if(card.saving)
					{	 
						if(strstr(cmdbuffer[bufindr], PSTR("M29")) == NULL)
						{
							card_write_command(cmdbuffer[bufindr]);
							printf(MSG_OK);
						}
						else                                         
						{
							card_closefile();
							printf(MSG_FILE_SAVED);
						}
						 printf("\n");
					}
					else
					{	
						process_commands();
					}
				#else
					process_commands();  
				#endif //SDSUPPORT	
					if(buflen > 0)
					//{
						buflen = (buflen-1);
						bufindr = (bufindr + 1)%BUFSIZE;
					//}
		 }
		 /*利用硬件实现断电检测，来实现断电续打。*/ //遇到了瓶颈，断电后短暂的持续供电尚未解决。
//	if(poweroff_flag == true) //断电信号触发
//	{
//		poweroff_flag = false; //清除断电标志位
//		FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+114,card.sdpos);
//		FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+118,current_position[Z_AXIS]);
//		FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+122,printingFilename);
//		card.sdprinting = false;	
//	}
		 /*利用纯软件方法实现断电续打*/  //频繁写入可能会影响flash寿命
//	if(nextRecordMillis < millis()) //屏幕超时更新
//	{
//        //if(card.sdprinting == true)
//		//{			
//			FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+114,card.sdpos);
//			FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+118,current_position[Z_AXIS]);
//			FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+122,printingFilename);
//		//}
//	  nextRecordMillis = millis() + 5000; //每10s写入一次flash
//	}	 
  //check heater every n milliseconds
  manage_heater();
  manage_inactivity();
  checkHitEndstops();
  lcd_update(); //屏幕界面更新
// }

}

void get_command(void)  //读取串口信息
{ 
	char i;
	int16_t n;
  char time[30];
  unsigned long t;
  int hours, minutes;
  while( MYSERIAL_available() > 0  && buflen < BUFSIZE)
  {	
    serial_char = MYSERIAL_read();
    if(serial_char == '\n' ||serial_char == '\r' ||(serial_char == ':' && comment_mode == false) ||serial_count >= (MAX_CMD_SIZE - 1) )
    {
      if(!serial_count) 
	    {
        comment_mode = false; //for new command
        return;
      }
      cmdbuffer[bufindw][serial_count] = 0; //terminate string
      if(!comment_mode)  //comment_mode 注释，如果不是注释
	    {
        comment_mode = false; //for new command
        fromsd[bufindw] = false;
        if(strchr(cmdbuffer[bufindw], 'N') != NULL) //如果该字符串中有字符‘N’N为行码 //extern char *strchr(const char *s,char c);功能：查找字符串s中首次出现字符c的位置 说明：返回首次出现c的位置的指针，如果s中不存在c则返回NULL                                                     
        {
          strchr_pointer = strchr(cmdbuffer[bufindw], 'N'); //返回字符‘N’在字符串处的指针
          gcode_N = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10)); //读取N后面的数字 //long int strtol (const char* str, char** endptr, int base);将字符串转换为长整型数(long) 说明：str 为要转换的字符串，endstr 为第一个不能转换的字符的指针，base 为字符串 str 所采用的进制。
          if(gcode_N != gcode_LastN+1 && (strstr(cmdbuffer[bufindw], PSTR("M110")) == NULL) ) //行数字不等于上一行数字加一，并且字符串中没有“M110”则返回空指针。 //extern char *strstr(char *str1, char *str2); 找出str2字符串在str1字符串中第一次出现的位置（不包括str2的串结束符）说明：返回该位置的指针，如找不到，返回空指针。
		      {                                                                             
            SERIAL_ERROR_START;   //串口打印“ERROR”
            printf(MSG_ERR_LINE_NO); //打印行号错误信息
            printf("%ld",gcode_LastN); //打印上一行的行号
            //Serial.println(gcode_N);
            FlushSerialRequestResend(); //清除串口信息重新发送
            serial_count = 0;  //串口行字符计数
            return;
          }

          if(strchr(cmdbuffer[bufindw], '*') != NULL) //当前字符串中有“*”// *号为标记码
          {
            u8 checksum = 0;
            u8 count = 0;
            while(cmdbuffer[bufindw][count] != '*') checksum = checksum^cmdbuffer[bufindw][count++];  //软件计算标记码
            strchr_pointer = strchr(cmdbuffer[bufindw], '*');  //返回“*”在字符串中的位置
            if( (u8)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum) //*号后面的数值不等于标记码的数值 //函数 strtod() 用来将字符串转换成双精度浮点数(double)
							{                                                                
									SERIAL_ERROR_START;  //串口打印“ERROR”
									printf(MSG_ERR_CHECKSUM_MISMATCH); //打印标记码不匹配信息
									printf(" checksum: %d\n\r",checksum);//打印出标记码的数值
									count = 0;
								  printf(" '");
									while(cmdbuffer[bufindw][count] != '*')
									{
										printf("%c",cmdbuffer[bufindw][count++]);
									}
									printf(" '\n\r ");
									checksum = 0;
									count = 0;
									while(cmdbuffer[bufindw][count] != '*')
									{ 
										printf("cmdbuffer:%d;",cmdbuffer[bufindw][count]);
										checksum = checksum^cmdbuffer[bufindw][count++];
									  printf(" checksum:%d \n\r",checksum);
									}
									///	printf("\n\r ");
						
									printf("%ld",gcode_LastN);  //打印上一行的行号
									FlushSerialRequestResend(); //清除串口信息重新发送
									serial_count = 0;
									return;
            }
            //if no errors, continue parsing //如果没有错，继续语法分析
          }
          else
          {
            SERIAL_ERROR_START;  //串口打印“ERROR”
            printf(MSG_ERR_NO_CHECKSUM);  //打印 有行码没有标记码的错误信息
            printf("%ld",gcode_LastN);  //打印上一行的行码
            FlushSerialRequestResend();  //清除串口信息重新发送
            serial_count = 0;
            return;
          }

          gcode_LastN = gcode_N;  //赋值，计录行码历史值
          //if no errors, continue parsing
        }
        else  // if we don't receive 'N' but still see '*'//如果没有收到行码‘N’但是依然收到了标记码‘*’
        {
          if((strchr(cmdbuffer[bufindw], '*') != NULL))  //当前字符串中有“*”
          {
            SERIAL_ERROR_START;  //串口打印“ERROR”
            printf(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);  //打印没有行码有标记码的错误信息
            printf("%ld",gcode_LastN);
            serial_count = 0;
            return;
          }
        }
        if((strchr(cmdbuffer[bufindw], 'G') != NULL)) //当前字符串中有字符“G”
		    {
          strchr_pointer = strchr(cmdbuffer[bufindw], 'G');  //返回字符G所在位置的指针
          switch((int)((strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL))))  //读取字符G后面的数字
						{
							case 0:
							case 1:
							case 2:
							case 3:
								if(Stopped == false) { // If printer is stopped by an error the G[0-3] codes are ignored. //如果打印机由于G[0-3]代码的错误停止，忽略掉。
									#ifdef SDSUPPORT
									if(card.saving)
										break;
							    #endif //SDSUPPORT
									printf(MSG_OK);  //打印“OK”
									printf("\n");
								}
								else {
									printf(MSG_ERR_STOPPED);  //打印 由于错误停止的信息 "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)"
								}
								break;
							default:
								break;
             }
        }
        bufindw = (bufindw + 1)%BUFSIZE;
        buflen += 1;
      }
      serial_count = 0; //clear buffer  //清除缓冲区
    }
    else
    {
      if(serial_char == ';') comment_mode = true; //如果串口读取到;号,说明后面为注释
      if(!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char; //如果不是注释内容，将串口信息存储到缓冲区
    }
  }
	#ifdef SDSUPPORT   
  if(!card.sdprinting || serial_count!=0)
		{ 
    return;
   }
  while( !card_eof()  && buflen < BUFSIZE) 
  {	
		//利用读取到文件的位置进行断电续打测试（断电时记录card.sdpos）
		//用于测试指定到SD卡文件中的位置，开始读取并打印
//		if(duandianxuda == 1)
//		{
//		  duandianxuda =0;
//			enquecommand("G28");
//			enquecommand("G1 Z10");
//			enquecommand("M109 S190");
//			card_setIndex(830000);
//			card.sdpos = f_tell(&card.fgcode);
//		}
		if(lcd_contiune_print_after_poweroff == 1) //如果断电续打标志位置1
		{
			lcd_contiune_print_after_poweroff = 0;
			//开启打印前的预处理指令，如归位，z轴到指定位置，喷头加热到预设温度等。
			enquecommand("G28");
			sprintf(cmd_position_z,PSTR("G1 Z%f"),poweroff_position_z);
			enquecommand(cmd_position_z);
			enquecommand("M109 S190");
			enquecommand("M106 S255");
			card_setIndex(poweroff_sdpos); //设定文件开始的读取位置
		}
    n=card_get();
    serial_char = (BYTE)n;
    if(serial_char == '\n' ||
       serial_char == '\r' ||
       (serial_char == ':' && comment_mode == false) ||
       serial_count >= (MAX_CMD_SIZE - 1)||
			 n==-1)
    { 
      if(card_eof()) //读取到了文件的末尾
			{
        printf(MSG_FILE_PRINTED);
	    	printf("\n");
        stoptime=millis();
        t=(stoptime-starttime)/1000;
        minutes=(t/60)%60;
        hours=t/60/60;
        sprintf(time, PSTR("%d hours %d minutes"),hours, minutes);
        SERIAL_ECHO_START;
        printf("%s",time);
				//将打印完成的时间存储到consumingTime字符串数组中，便于显示到屏幕上
        sprintf(consumingTime, PSTR("Cost Time:%dh%dm"),hours, minutes);
				PrintingFinishedMenu.menuItems[0] = (uint8_t *)printingFilename;
				PrintingFinishedMenu.menuItems[1] = "*Printing**Done*";
				PrintingFinishedMenu.menuItems[2] = (uint8_t *)consumingTime;
				PrintingFinishedMenu.menuItems[3] = "any key back...";
				CurrentMenu = &PrintingFinishedMenu;
				lcdDisplayUpdate = 1; //切换菜单后要将显示更新标志位置位，用于使当前菜单更新到切换的菜单
				lcd_update();
        card_printingHasFinished();
       // card_checkautostart(true);
				
				//将存储的断电续打参数清零（暂且将变量清零在写入，若找到更好的方法再替换）
				poweroff_sdpos = 0;
				poweroff_position_z = 0;
				for(i=0;i<30;i++)
				{
					poweroff_printing_filename[i] = 0; 
				}
		    FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+114,poweroff_sdpos); 
		    FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+118,poweroff_position_z);
		    FLASH_WRITE_VAR(FLASH_SET_STORE_OFFSET+122,poweroff_printing_filename);
				
//				PrintingFinished(); //打印完成后LCD屏幕的变化
      }
      if(!serial_count)
      {
        comment_mode = false; //for new command
        return; //if empty line
      }
      cmdbuffer[bufindw][serial_count] = 0; //terminate string
//      if(!comment_mode){
        fromsd[bufindw] = true;
        buflen += 1;
        bufindw = (bufindw + 1)%BUFSIZE;
//      }
      comment_mode = false; //for new command
      serial_count = 0; //clear buffer
    }
    else
    {
      if(serial_char == ';') comment_mode = true;
      if(!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char;
    }
  }
  #endif //SDSUPPORT
}


 float code_value(void)  //将strchr_pointer指针后的字符串中的数字按十进制数转化为数值
 {

  return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));
}

long code_value_long(void)  //将strchr_pointer指针后的字符串中的数字按十进制数转化为双精度数值
{
  return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}

bool code_seen(char code)  //如果字符“code”被找到，返回值为true
{
  strchr_pointer = strchr(cmdbuffer[bufindr], code);
  return (strchr_pointer != NULL);  //Return True if a character was found
}


void axis_is_at_home(int axis) 
{
  current_position[axis] = base_home_pos[axis] + add_homeing[axis];
  min_pos[axis] =          base_min_pos[axis] + add_homeing[axis];
  max_pos[axis] =          base_max_pos[axis] + add_homeing[axis];
}
#define HOMEAXIS_DO(LETTER) (( LETTER##_HOME_DIR==-1) || (LETTER##_HOME_DIR==1))
static void homeaxis(int axis) 
{
  if (axis==X_AXIS ? HOMEAXIS_DO(X) :
      axis==Y_AXIS ? HOMEAXIS_DO(Y) :
      axis==Z_AXIS ? HOMEAXIS_DO(Z) :
      0) 	//
   {
    current_position[axis] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    destination[axis] = 1.5 * max_length[axis] * home_dir[axis];
    feedrate = homing_feedrate[axis];
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    current_position[axis] = 0;
    plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
    destination[axis] = -home_retract_mm[axis] * home_dir[axis];
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    destination[axis] = 2*home_retract_mm[axis] * home_dir[axis];
    feedrate = homing_feedrate[axis]/2 ;
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
    st_synchronize();

    axis_is_at_home(axis);
    destination[axis] = current_position[axis];
    feedrate = 0.0;
    endstops_hit_on_purpose();
  }
}
#define HOMEAXIS(LETTER) homeaxis(LETTER##_AXIS)
void process_commands(void)  //解析G代码和M代码
{ unsigned long codenum; //throw away variable
  char *starpos = NULL;
  int8_t i;
  if(code_seen('G'))  //读取到了字符G
  {
    switch((int)code_value())  //G后面的数字
    {
	
    case 0: // G0 -> G1
    case 1: // G1
      if(Stopped == false) 
			{
        get_coordinates(); // For X Y Z E F
				//利用Z轴停止的高度进行断电续打测试（断电时记录current_position[Z_AXIS]）
//				if((current_position[Z_AXIS] >10&&current_position[Z_AXIS] <20) && (destination[Z_AXIS]>10 && destination[Z_AXIS]<20))
				//if((current_position[Z_AXIS] >10) && (destination[Z_AXIS]>10)) //打印高度超过10mm停止打印
				//if((current_position[Z_AXIS] <=10) && (destination[Z_AXIS]<=10)) //从10mm的位置开始打印
				//{
					//ClearToSend();
					//break;

				//}
        prepare_move();
//		  printf(" test SD \n"); //用来测试是否执行了SD卡中的命令
        //ClearToSend();
        return;
      }
      //break;
    case 2: // G2  - CW ARC
      if(Stopped == false) {
        get_arc_coordinates();
        prepare_arc_move(true);
        return;
      }
    case 3: // G3  - CCW ARC
      if(Stopped == false) {
        get_arc_coordinates();
        prepare_arc_move(false);
        return;
      }
    case 4: // G4 dwell
      codenum = 0;
      if(code_seen('P')) codenum = code_value(); // milliseconds to wait  //毫秒等待
      if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait  //秒等待

      st_synchronize();  //synchronize使……合拍，同步
      codenum += millis();  // keep track of when we started waiting  //当开始等待时保持轨迹
      previous_millis_cmd = millis(); //记录历史值
      while(millis()  < codenum ){
        manage_heater();
        manage_inactivity();
//				interface_update(); //屏幕界面更新
      }
      break;
      #ifdef FWRETRACT	   //ONLY PARTIALLY TESTED //只有部分经过测试
      case 10: // G10 retract  //回抽
      if(!retracted)
      {
        destination[X_AXIS]=current_position[X_AXIS];
        destination[Y_AXIS]=current_position[Y_AXIS];
        destination[Z_AXIS]=current_position[Z_AXIS];
        current_position[Z_AXIS]+=-retract_zlift;
        destination[E_AXIS]=current_position[E_AXIS]-retract_length;
        feedrate=retract_feedrate;
        retracted=true;
        prepare_move();
      }

      break;
      case 11: // G10 retract_recover
      if(!retracted)
      {
        destination[X_AXIS]=current_position[X_AXIS];
        destination[Y_AXIS]=current_position[Y_AXIS];
        destination[Z_AXIS]=current_position[Z_AXIS];

        current_position[Z_AXIS]+=retract_zlift;
        current_position[E_AXIS]+=-retract_recover_length;
        feedrate=retract_recover_feedrate;
        retracted=false;
        prepare_move();
      }
      break;
      #endif //FWRETRACT   //ONLY PARTIALLY TESTED
    case 28: //G28 Home all Axis one at a time  //所有轴立刻归位
      saved_feedrate = feedrate;
      saved_feedmultiply = feedmultiply;
      feedmultiply = 100;
      previous_millis_cmd = millis();

      enable_endstops(true);

      for(i=0; i < NUM_AXIS; i++) {
        destination[i] = current_position[i];
      }
      feedrate = 0.0;
      home_all_axis = !((code_seen(axis_codes[0])) || (code_seen(axis_codes[1])) || (code_seen(axis_codes[2])));

      #if Z_HOME_DIR > 0                      // If homing away from BED do Z first
      if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
        HOMEAXIS(Z);
      }
      #endif

      #ifdef QUICK_HOME
      if((home_all_axis)||( code_seen(axis_codes[X_AXIS]) && code_seen(axis_codes[Y_AXIS])) )  //first diagonal move
      {
        current_position[X_AXIS] = 0;current_position[Y_AXIS] = 0;

        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[X_AXIS] = 1.5 * X_MAX_LENGTH * X_HOME_DIR;destination[Y_AXIS] = 1.5 * Y_MAX_LENGTH * Y_HOME_DIR;
        feedrate = homing_feedrate[X_AXIS];
        if(homing_feedrate[Y_AXIS]<feedrate)
          feedrate =homing_feedrate[Y_AXIS];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        st_synchronize();

        axis_is_at_home(X_AXIS);
        axis_is_at_home(Y_AXIS);
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        destination[X_AXIS] = current_position[X_AXIS];
        destination[Y_AXIS] = current_position[Y_AXIS];
        plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
        feedrate = 0.0;
        st_synchronize();
        endstops_hit_on_purpose();
      }
      #endif

      if((home_all_axis) || (code_seen(axis_codes[X_AXIS])))
      {
        HOMEAXIS(X);
      }

      if((home_all_axis) || (code_seen(axis_codes[Y_AXIS]))) {
        HOMEAXIS(Y);
      }

      #if Z_HOME_DIR < 0                      // If homing towards BED do Z last
      if((home_all_axis) || (code_seen(axis_codes[Z_AXIS]))) {
        HOMEAXIS(Z);
      }
      #endif

      if(code_seen(axis_codes[X_AXIS]))
      {
        if(code_value_long() != 0) {
          current_position[X_AXIS]=code_value()+add_homeing[0];
        }
      }

      if(code_seen(axis_codes[Y_AXIS])) {
        if(code_value_long() != 0) {
          current_position[Y_AXIS]=code_value()+add_homeing[1];
        }
      }

      if(code_seen(axis_codes[Z_AXIS])) {
        if(code_value_long() != 0) {
          current_position[Z_AXIS]=code_value()+add_homeing[2];
        }
      }
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);

      #ifdef ENDSTOPS_ONLY_FOR_HOMING
        enable_endstops(false);
      #endif

      feedrate = saved_feedrate;
      feedmultiply = saved_feedmultiply;
      previous_millis_cmd = millis();
      endstops_hit_on_purpose();
      break;
    case 90: // G90
      relative_mode = false;
      break;
    case 91: // G91
      relative_mode = true;
	  #ifdef DEBUG_PRINTF
	  printf("relative_mode = true;");
	  #endif
      break;
    case 92: // G92
      if(!code_seen(axis_codes[E_AXIS]))
        st_synchronize();
      for(i=0; i < NUM_AXIS; i++) {
        if(code_seen(axis_codes[i])) {
           if(i == E_AXIS) {
             current_position[i] = code_value();
             plan_set_e_position(current_position[E_AXIS]);
           }
           else {
             current_position[i] = code_value()+add_homeing[i];
             plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
           }
        }
      }
      break;
    }
  }
  else if(code_seen('M'))
  {	switch( (int)code_value() )
    { 
  	 
	 
	    case 0: // M0 - Unconditional stop - Wait for user button press on LCD
	    case 1: // M1 - Conditional stop - Wait for user button press on LCD
			{
		     #ifdef ULTIPANEL
		      codenum = 0;
		      if(code_seen('P')) codenum = code_value(); // milliseconds to wait
		      if(code_seen('S')) codenum = code_value() * 1000; // seconds to wait
		
		      st_synchronize();
		      previous_millis_cmd = millis();
		      if (codenum > 0){
		        codenum += millis();  // keep track of when we started waiting
		        while(millis()  < codenum && !LCD_CLICKED){
		          manage_heater();
		          manage_inactivity();
//							interface_update(); //屏幕界面更新
		        }
		      }else{
		        while(!LCD_CLICKED){
		          manage_heater();
		          manage_inactivity();
//							interface_update(); //屏幕界面更新
		        }
		      }
		   	  #endif
		    } break;
			case 15:
						break;
			case 16:
						break;
	    case 17:
	        enable_x();
	        enable_y();
	        enable_z();
	        enable_e0();
	        enable_e1();
            break;
		#ifdef SDSUPPORT
			case 19:

						break;
	    case 20:
		      printf(MSG_BEGIN_FILE_LIST);
			  printf("\n");
		      card_ls();
		      printf(MSG_END_FILE_LIST);
			  printf("\n");
		      break;
	    case 21: 	
		      card_initsd();		
		      break;
	    case 22: 
		      card_release();
		      break;
	    case 23:
		 	//	printf("--%s\n",strchr_pointer);
		      starpos = (strchr(strchr_pointer + 4,'*'));
		      if(starpos!=NULL)
		        *(starpos-1)='\0';
		//	  printf("--%s\n",strchr_pointer + 4);
		      card_openFile(strchr_pointer + 4,true);
		      break;
	    case 24:
		      card_startFileprint();
		      starttime=millis();
		      break;
	    case 25: 
		      card_pauseSDPrint();
		      break;
	    case 26: 
		      if(card.cardOK && code_seen('S')) {
		        card_setIndex(code_value_long());
		      }
		      break;
	    case 27: 
		      card_getStatus();
		      break;
	    case 28:
		      starpos = (strchr(strchr_pointer + 4,'*'));
		     if(starpos != NULL){
		        char* npos = strchr(cmdbuffer[bufindr], 'N');
		        strchr_pointer = strchr(npos,' ') + 1;
		       *(starpos-1) = '\0';
		      }
		      card_openFile(strchr_pointer+4,false);
		      break;
	    case 29:
		      //processed in write to file routine above
		      //card,saving = false;
		      break;
	    case 30: 
		      if (card.cardOK){
		        card_closefile();
		        starpos = (strchr(strchr_pointer + 4,'*'));
		        if(starpos != NULL){
		          char* npos = strchr(cmdbuffer[bufindr], 'N');
		          strchr_pointer = strchr(npos,' ') + 1;
		          *(starpos-1) = '\0';
		        }
		        card_removeFile(strchr_pointer + 4);
		      }
		      break;
	#endif //SDSUPPORT
	    case 31: //M31 take time since the start of the SD print or an M109 command
		      {
		     // char time[30];
		      unsigned long t=(stoptime-starttime)/1000;
		      int sec,min;
					stoptime=millis();
		      min=t/60;
		      sec=t%60;
		      printf("%d min, %d sec", min, sec);
		      SERIAL_ECHO_START;
		      autotempShutdown();
		      }
		      break;
		case 42: //M42 -Change pin status via gcode
		    /*  if (code_seen('S'))
		      {
		        int pin_status = code_value();
		        int pin_number = LED_PIN;
		        if (code_seen('P') && pin_status >= 0 && pin_status <= 255)
		          pin_number = code_value();
		        for(int8_t i = 0; i < (int8_t)sizeof(sensitive_pins); i++)
		        {
		          if (sensitive_pins[i] == pin_number)
		          {
		            pin_number = -1;
		            break;
		          }
		        }
		      #if defined(FAN_PIN)
		        if (pin_number == FAN_PIN)
		          fanSpeed = pin_status;
		      #endif
		        if (pin_number > -1)
		        {
		          pinMode(pin_number, OUTPUT);
		          digitalWrite(pin_number, pin_status);
		          analogWrite(pin_number, pin_status);
		        }
		      }
			  */
		     break;
		case 104: // M104 只负责设置挤出头的目标温度
		      if(setTargetedHotend(104))
					{
		        break;
		      }
		      if (code_seen('S')) 
					{	
						setTargetHotend(code_value(), tmp_extruder);
						
					}
		      setWatch();
		      break;
	    case 140: // M140 set bed temp
		      if (code_seen('S')) setTargetBed(code_value());
		      break;
	    case 105 : // M105
		      if(setTargetedHotend(105)){
		        break;
		      }
		      #if defined(TEMP_0_PIN)
			  		printf("ok T:%.1f /%.1f",degHotend(tmp_extruder),degTargetHotend(tmp_extruder));
		      		#if defined(TEMP_BED_PIN)
			  			printf(" B:%.1f /%.1f",degBed(),degTargetBed());
		       		#endif //TEMP_BED_PIN
		      #else
		        SERIAL_ERROR_START;
		        printf(MSG_ERR_NO_THERMISTORS);
		      #endif //TEMP_0_PIN
		
		        printf(" @:%d",getHeaterPower(tmp_extruder));
				    printf(" B@:%d\n",getHeaterPower(-1));
			  return;
		    //  break;
		case 109:
			{// M109 - Wait for extruder heater to reach target.等待挤出头加热到目标温度
				bool target_direction; //用于记录加热还是冷却
				long residencyStart;
				if(setTargetedHotend(109))break;
		        #ifdef AUTOTEMP
		        	 autotemp_enabled=false;
		        #endif
			    if (code_seen('S')) setTargetHotend(code_value(), tmp_extruder);
		        #ifdef AUTOTEMP
			        if (code_seen('S')) autotemp_min=code_value();
			        if (code_seen('B')) autotemp_max=code_value();	
			        if (code_seen('F'))
			        {
			          autotemp_factor=code_value();
			          autotemp_enabled=true;
			        }
			    #endif
				setWatch();
      			codenum = millis();
		      	/* See if we are heating up or cooling down */ //看是在加热还是在冷却 1为加热 0为冷却
      			target_direction = isHeatingHotend(tmp_extruder); // true if heating, false if cooling
				#ifdef TEMP_RESIDENCY_TIME
		        	residencyStart = -1;
		        /* continue to loop until we have reached the target temp
		          _and_ until TEMP_RESIDENCY_TIME hasn't passed since we reached it */
			        while((residencyStart == -1) || (residencyStart >= 0 && (((unsigned int) (millis() - residencyStart)) < (TEMP_RESIDENCY_TIME * 1000UL))) ) 
						  { if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    printf("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					          //  SERIAL_PROTOCOLPGM("T:");
					          //  SERIAL_PROTOCOL_F(degHotend(tmp_extruder),1);
					          //  SERIAL_PROTOCOLPGM(" E:");
					           // SERIAL_PROTOCOL((int)tmp_extruder);
					            #ifdef TEMP_RESIDENCY_TIME
					              printf(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 printf("%ld\n", codenum );
					              }
					              else
					              {
					                 printf("?\n");
					              }
					            #else
					              printf("\n");
					            #endif
					            codenum = millis();
					          }
							      manage_heater();
          					  manage_inactivity();
										lcd_update();
//										interface_update(); //屏幕界面更新
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		     	#else
		        	while ( target_direction ? (isHeatingHotend(tmp_extruder)) : (isCoolingHotend(tmp_extruder)&&(CooldownNoWait==false)) ) 
						  { if( (millis() - codenum) > 1000UL )
							  { //Print Temp Reading and remaining time every 1 second while heating up/cooling down
							    printf("T:%.1f E:%d",degHotend(tmp_extruder),tmp_extruder);
					          //  SERIAL_PROTOCOLPGM("T:");
					          //  SERIAL_PROTOCOL_F(degHotend(tmp_extruder),1);
					          //  SERIAL_PROTOCOLPGM(" E:");
					           // SERIAL_PROTOCOL((int)tmp_extruder);
					            #ifdef TEMP_RESIDENCY_TIME
					              printf(" W:");
					              if(residencyStart > -1)
					              {
					                 codenum = ((TEMP_RESIDENCY_TIME * 1000UL) - (millis() - residencyStart)) / 1000UL;
					                 printf("%ld\n", codenum );
					              }
					              else
					              {
					                 printf("?\n");
					              }
					            #else
					              printf("\n");
					            #endif
					            codenum = millis();
					          }
							  manage_heater();
          					  manage_inactivity();
										lcd_update();
//										interface_update(); //屏幕界面更新
					         #ifdef TEMP_RESIDENCY_TIME
					            /* start/restart the TEMP_RESIDENCY_TIME timer whenever we reach target temp for the first time
					              or when current temp falls outside the hysteresis after target temp was reached */
						         if ((residencyStart == -1 &&  target_direction && (degHotend(tmp_extruder) >= (degTargetHotend(tmp_extruder)-TEMP_WINDOW))) ||
						              (residencyStart == -1 && !target_direction && (degHotend(tmp_extruder) <= (degTargetHotend(tmp_extruder)+TEMP_WINDOW))) ||
						              (residencyStart > -1 && labs(degHotend(tmp_extruder) - degTargetHotend(tmp_extruder)) > TEMP_HYSTERESIS) )
						          {
						            residencyStart = millis();
						          }
					        #endif //TEMP_RESIDENCY_TIME
						  }
		      	#endif //TEMP_RESIDENCY_TIME
			    starttime=millis();
                previous_millis_cmd = millis();
			}break;
		case 190: // M190 - Wait for bed heater to reach target.
		    #if defined(TEMP_BED_PIN)
		        if (code_seen('S')) setTargetBed(code_value());
		        codenum = millis();
		        while(isHeatingBed())
		        {
		          if(( millis() - codenum) > 1000 ) //Print Temp Reading every 1 second while heating up.
		          {
		            float tt=degHotend(active_extruder);
								printf("T:%.1f E:%d B:%.1f\n",tt,active_extruder,degBed());
		           // SERIAL_PROTOCOLPGM("T:");
		           // SERIAL_PROTOCOL(tt);
		           // SERIAL_PROTOCOLPGM(" E:");
		          //  SERIAL_PROTOCOL((int)active_extruder);
		          //  SERIAL_PROTOCOLPGM(" B:");
		          //  SERIAL_PROTOCOL_F(degBed(),1);
		           // SERIAL_PROTOCOLLN("");
		            codenum = millis();
		          }
		          manage_heater();
		          manage_inactivity();
							lcd_update();
//							interface_update(); //屏幕界面更新
		        }
		        previous_millis_cmd = millis();
		    #endif
		    break;
          #if defined(FAN_PIN) 
	        case 106: //M106 Fan On //开启模型散热风扇
		        if (code_seen('S'))
				{
				  fanSpeed = constrain(code_value(),0,255);
		        }
		        else 
				{
		          fanSpeed=255;
				}
	            break;
	        case 107: //M107 Fan Off //关闭模型散热风扇
	        	fanSpeed = 0;
	        break;
		  #endif //FAN_PIN
		  case 126: //M126 valve open
			#ifdef BARICUDA
	        // PWM for HEATER_1_PIN
		        #if defined(HEATER_1_PIN) 
				    if (code_seen('S')){
			             ValvePressure=constrain(code_value(),0,255);
			          }
			          else {
			            ValvePressure=255;
			          }
			          
				#endif
			#endif
			break;
		  case 127: //M127 valve closed
			#ifdef BARICUDA
	        // PWM for HEATER_1_PIN
		        #if defined(HEATER_1_PIN) 
			          ValvePressure = 0;
				#endif
			#endif
			break;
		 case 128://M128 valve open
			#ifdef BARICUDA
	        // PWM for HEATER_1_PIN
		        #if defined(HEATER_2_PIN) 
			          if (code_seen('S')){
			             EtoPPressure=constrain(code_value(),0,255);
			          }
			          else {
			            EtoPPressure=255;
			          }
				#endif
			#endif
			break;
		  case 129: //M129 valve closed
			#ifdef BARICUDA
	        // PWM for HEATER_1_PIN
		        #if defined(HEATER_2_PIN) 
			          EtoPPressure = 0;
				#endif
			#endif
			break;
		 case 80: // M80 - ATX Power On
	         // SET_OUTPUT(PS_ON_PIN); //GND	////////////////////////////////////////////////////
	         //  WRITE(PS_ON_PIN, PS_ON_AWAKE);////////////////////////////////////////////////////
	        break;
	     case 81: // M81 - ATX Power Off //可用于打印完自动关机，还需要修改
//			disable_heater();
//			st_synchronize();
//			disable_e0();
//			disable_e1();
//			disable_e2();
//			finishAndDisableSteppers();
//			fanSpeed = 0;
//			delay(1000); // Wait a little before to switch off
//			#if defined(SUICIDE_PIN) && SUICIDE_PIN > -1
//			st_synchronize();
//			suicide();
//			#elif defined(PS_ON_PIN) 
//			PS_ON_PIN = PS_ON_ASLEEP;
//			#endif
//			#ifdef ULTIPANEL
//			powersupply = false;
//			LCD_MESSAGEPGM(MACHINE_NAME" "MSG_OFF".");
//			lcd_update();
//			#endif
		 	break;
	     case 82:
	        axis_relative_modes[3] = false;
	        break;
	     case 83:
	        axis_relative_modes[3] = true;
	        break;
	     case 18: //compatibility
	     case 84: // M84
	        if(code_seen('S'))
			{
	          stepper_inactive_time = code_value() * 1000;
	        }
			else
			{
		        bool all_axis = !((code_seen(axis_codes[0])) || (code_seen(axis_codes[1])) || (code_seen(axis_codes[2]))|| (code_seen(axis_codes[3])));
		        if(all_axis)
		        {
		          st_synchronize();
		          disable_e0();
		          disable_e1();
		        //  disable_e2();
		          finishAndDisableSteppers();
		        }
		        else
		        {
		          st_synchronize();
		          if(code_seen('X')) disable_x();
		          if(code_seen('Y')) disable_y();
		          if(code_seen('Z')) disable_z();
		         // #if ((E0_ENABLE_PIN != X_ENABLE_PIN) && (E1_ENABLE_PIN != Y_ENABLE_PIN)) // Only enable on boards that have seperate ENABLE_PINS
		            if(code_seen('E')) {
		              disable_e0();
		              disable_e1();
		            //  disable_e2();
		            }
		        //  #endif
		        }
		     }
			 break;
		  case 85: // M85
		     code_seen('S');
		     max_inactive_time = code_value() * 1000;
		     break;
		  case 92: // M92
		      for(i=0; i < NUM_AXIS; i++)
		      {
		        if(code_seen(axis_codes[i]))
		        {
		          if(i == 3) { // E
		            float value = code_value();
		            if(value < 20.0) {
		              float factor = axis_steps_per_unit[i] / value; // increase e constants if M92 E14 is given for netfab.
		              max_e_jerk *= factor;
		              max_feedrate[i] *= factor;
		              axis_steps_per_sqr_second[i] *= factor;
		            }
		            axis_steps_per_unit[i] = value;
		          }
		          else {
		            axis_steps_per_unit[i] = code_value();
		          }
		        }
		      }
			 break;
	      case 115: // M115
		      printf(MSG_M115_REPORT);
		      break;
	      case 117: // M117 display message		/////////////////////////////////////////////////////
		      starpos = (strchr(strchr_pointer + 5,'*'));
		      if(starpos!=NULL)
		        *(starpos-1)='\0';
		      break;
		  case 114: // M114	 
				  printf("X:%f Y:%f Z:%f E:%f",current_position[X_AXIS],current_position[Y_AXIS],current_position[Z_AXIS],current_position[E_AXIS]);			
			    printf(MSG_COUNT_X);
				  printf("%f Y:%f Z:%f\n",((float)st_get_position(X_AXIS))/axis_steps_per_unit[X_AXIS],((float)st_get_position(Y_AXIS))/axis_steps_per_unit[Y_AXIS],((float)st_get_position(Z_AXIS))/axis_steps_per_unit[Z_AXIS]);
		      break; 
		   case 120: // M120
		      enable_endstops(false) ;
		      break;
		   case 121: // M121
		      enable_endstops(true) ;
		      break;
		   case 119: // M119
		      printf(MSG_M119_REPORT);
			  printf("\n");
		      #if defined(X_MIN_PIN) 
		        printf(MSG_X_MIN);
				printf(((X_MIN_PIN==X_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
				printf("\n");
		       // (X_MIN_PIN==X_ENDSTOPS_INVERTING) ? (printf(MSG_ENDSTOP_HIT)) : (printf(MSG_ENDSTOP_OPEN));
		      #endif
		      #if defined(X_MAX_PIN)
		        printf(MSG_X_MAX);
		        printf(((X_MAX_PIN==X_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
				printf("\n");
		      #endif
		      #if defined(Y_MIN_PIN)
		        printf(MSG_Y_MIN);
		        printf(((Y_MIN_PIN^Y_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
				printf("\n");
		      #endif
		      #if defined(Y_MAX_PIN) 
		        printf(MSG_Y_MAX);
		        printf(((Y_MAX_PIN^Y_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
				printf("\n");
		      #endif
		      #if defined(Z_MIN_PIN) 
		        printf(MSG_Z_MIN);
		        printf(((Z_MIN_PIN^Z_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
				printf("\n");
		      #endif
		      #if defined(Z_MAX_PIN)
		        printf(MSG_Z_MAX);
		        printf(((Z_MAX_PIN^Z_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
				printf("\n");
		      #endif
		      break;
			  //TODO: update for all axis, use for loop
		  case 201: // M201	 
		      for( i=0; i < NUM_AXIS; i++)
		      {
		        if(code_seen(axis_codes[i]))
		        {
		          max_acceleration_units_per_sq_second[i] = code_value();
		        }
		      }
		      // steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
		      reset_acceleration_rates();
		      break;
	      case 202: // M202
		  	#if 0 // Not used for Sprinter/grbl gen6
		      for(i=0; i < NUM_AXIS; i++) {
		        if(code_seen(axis_codes[i])) axis_travel_steps_per_sqr_second[i] = code_value() * axis_steps_per_unit[i];
		      }
			#endif
		      break;
	      case 203: // M203 max feedrate mm/sec
		      for( i=0; i < NUM_AXIS; i++) 
				  {
		        if(code_seen(axis_codes[i])) max_feedrate[i] = code_value();
		      }
		      break; 
	      case 204: // M204 acclereration S normal moves T filmanent only moves
		      {
		        if(code_seen('S')) acceleration = code_value() ;
		        if(code_seen('T')) retract_acceleration = code_value() ;
		      }
	          break;
	      case 205: //M205 advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk
		    {
		      if(code_seen('S')) minimumfeedrate = code_value();
		      if(code_seen('T')) mintravelfeedrate = code_value();
		      if(code_seen('B')) minsegmenttime = code_value() ;
		      if(code_seen('X')) max_xy_jerk = code_value() ;
		      if(code_seen('Z')) max_z_jerk = code_value() ;
		      if(code_seen('E')) max_e_jerk = code_value() ;
		    }
	    	break;
	      case 206: // M206 additional homeing offset
		      for( i=0; i < 3; i++)
		      {
		        if(code_seen(axis_codes[i])) add_homeing[i] = code_value();
		      }
		      break;
        #ifdef FWRETRACT
		    case 207: //M207 - set retract length S[positive mm] F[feedrate mm/sec] Z[additional zlift/hop]
		    {
		      if(code_seen('S'))
		      {
		        retract_length = code_value() ;
		      }
		      if(code_seen('F'))
		      {
		        retract_feedrate = code_value() ;
		      }
		      if(code_seen('Z'))
		      {
		        retract_zlift = code_value() ;
		      }
		    }break;
		    case 208: // M208 - set retract recover length S[positive mm surplus to the M207 S*] F[feedrate mm/sec]
		    {
		      if(code_seen('S'))
		      {
		        retract_recover_length = code_value() ;
		      }
		      if(code_seen('F'))
		      {
		        retract_recover_feedrate = code_value() ;
		      }
		    }break;
		    case 209: // M209 - S<1=true/0=false> enable automatic retract detect if the slicer did not support G10/11: every normal extrude-only move will be classified as retract depending on the direction.
		    {
		      if(code_seen('S'))
		      {
		        int t= code_value() ;
		        switch(t)
		        {
		          case 0: autoretract_enabled=false;retracted=false;break;
		          case 1: autoretract_enabled=true;retracted=false;break;
		          default:
		            SERIAL_ECHO_START;
		            printf(MSG_UNKNOWN_COMMAND);
		            printf("%d",cmdbuffer[bufindr]);
		            printf("\"");
		        }
		      }
		
		    }break;
   			#endif // FWRETRACT
		    #if EXTRUDERS > 1
			case 218: // M218 - set hotend offset (in mm), T<extruder_number> X<offset_on_X> Y<offset_on_Y>
			    {
			      if(setTargetedHotend(218)){
			        break;
			      }
			      if(code_seen('X'))
			      {
			        extruder_offset[X_AXIS][tmp_extruder] = code_value();
			      }
			      if(code_seen('Y'))
			      {
			        extruder_offset[Y_AXIS][tmp_extruder] = code_value();
			      }
			      SERIAL_ECHO_START;
			      printf(MSG_HOTEND_OFFSET);
			      for(tmp_extruder = 0; tmp_extruder < EXTRUDERS; tmp_extruder++)
			      {
			         //SERIAL_ECHO(" ");
			         printf(" %f,%f",extruder_offset[X_AXIS][tmp_extruder],extruder_offset[Y_AXIS][tmp_extruder]);
			        // SERIAL_ECHO(",");
			       //  SERIAL_ECHO(extruder_offset[Y_AXIS][tmp_extruder]);
			      }
			      printf("\n");
			    }break;
		   #endif
		   case 220: // M220 S<factor in percent>- set speed factor override percentage
			    {
			      if(code_seen('S'))
			      {
			        feedmultiply = code_value() ;
			      }
			    }
		      break;
		   case 221: // M221 S<factor in percent>- set extrude factor override percentage
			    {
			      if(code_seen('S'))
			      {
			        extrudemultiply = code_value() ;
			      }
			    }
		      break;
		   #if NUM_SERVOS > 0
		   case 280: // M280 - set servo position absolute. P: servo index, S: angle or microseconds
		      {
		        int servo_index = -1;
		        int servo_position = 0;
		        if (code_seen('P'))
		          servo_index = code_value();
		        if (code_seen('S')) {
		          servo_position = code_value();
		          if ((servo_index >= 0) && (servo_index < NUM_SERVOS)) {
		            servos[servo_index].write(servo_position);
		          }
		          else {
		            SERIAL_ECHO_START;
		            SERIAL_ECHO("Servo ");
		            SERIAL_ECHO(servo_index);
		            SERIAL_ECHOLN(" out of range");
		          }
		        }
		        else if (servo_index >= 0) {
		          SERIAL_PROTOCOL(MSG_OK);
		          SERIAL_PROTOCOL(" Servo ");
		          SERIAL_PROTOCOL(servo_index);
		          SERIAL_PROTOCOL(": ");
		          SERIAL_PROTOCOL(servos[servo_index].read());
		          SERIAL_PROTOCOLLN("");
		        }
		      }
		      break;
		    #endif // NUM_SERVOS > 0
		   // #if LARGE_FLASH == true && ( BEEPER > 0 || defined(ULTRALCD) )
		    case 300: // M300
		    {
		      int beepP = 1000;
		      if(code_seen('P')) beepP = code_value();
						BEEP=1;  
						delay_ms(beepP);
						BEEP=0;
		    }
		    break;
		 //   #endif // M300
       #ifdef PIDTEMP
	    case 301: // M301
	      {
	        if(code_seen('P')) Kp = code_value();
	        if(code_seen('I')) Ki = scalePID_i(code_value());
	        if(code_seen('D')) Kd = scalePID_d(code_value());
	
	        #ifdef PID_ADD_EXTRUSION_RATE
	        if(code_seen('C')) Kc = code_value();
	        #endif
	
	        updatePID();
	        printf(MSG_OK);
					printf(" p:%f i:%f d:%f",Kp,unscalePID_i(Ki),unscalePID_d(Kd));
	       // SERIAL_PROTOCOL(" p:");
	       // SERIAL_PROTOCOL(Kp);
	       // SERIAL_PROTOCOL(" i:");
	       // SERIAL_PROTOCOL(unscalePID_i(Ki));
	       // SERIAL_PROTOCOL(" d:");
	       // SERIAL_PROTOCOL(unscalePID_d(Kd));
	        #ifdef PID_ADD_EXTRUSION_RATE
	       
	        //Kc does not have scaling applied above, or in resetting defaults
			printf(" c:%f",Kc);
	      //  SERIAL_PROTOCOL(Kc);
	        #endif
	        printf("\n");
	      }
	      break;
	    #endif //PIDTEMP
	    #ifdef PIDTEMPBED
	    case 304: // M304
	      {
	        if(code_seen('P')) bedKp = code_value();
	        if(code_seen('I')) bedKi = scalePID_i(code_value());
	        if(code_seen('D')) bedKd = scalePID_d(code_value());
	
	        updatePID();
					printf(MSG_OK);
					printf(" p:%f i:%f d:%f",Kp,unscalePID_i(bedKi,unscalePID_d(bedKd)));
					printf("\n");
	//         SERIAL_PROTOCOL(MSG_OK);
	//         SERIAL_PROTOCOL(" p:");
	//         SERIAL_PROTOCOL(bedKp);
	//         SERIAL_PROTOCOL(" i:");
	//         SERIAL_PROTOCOL(unscalePID_i(bedKi));
	//         SERIAL_PROTOCOL(" d:");
	//         SERIAL_PROTOCOL(unscalePID_d(bedKd));
	//         SERIAL_PROTOCOLLN("");
	      }
	      break;
	    #endif //PIDTEMP
	    case 240: // M240  Triggers a camera by emulating a Canon RC-1 : http://www.doc-diy.net/photo/rc-1_hacked/
		     {
		      #if defined(PHOTOGRAPH_PIN)
		//         const uint8_t NUM_PULSES=16;
		//         const float PULSE_LENGTH=0.01524;
		//         for(int i=0; i < NUM_PULSES; i++) {
		//           WRITE(PHOTOGRAPH_PIN, HIGH);
		//           _delay_ms(PULSE_LENGTH);
		//           WRITE(PHOTOGRAPH_PIN, LOW);
		//           _delay_ms(PULSE_LENGTH);
		//         }
		//         delay(7.33);
		//         for(int i=0; i < NUM_PULSES; i++) {
		//           WRITE(PHOTOGRAPH_PIN, HIGH);
		//           _delay_ms(PULSE_LENGTH);
		//           WRITE(PHOTOGRAPH_PIN, LOW);
		//           _delay_ms(PULSE_LENGTH);
		//         }
		      #endif
		     }
		    break;
		    case 302: // allow cold extrudes
		    {
		      allow_cold_extrudes(true);
		    }
		    break;
		    case 303: // M303 PID autotune
		    {
		      float temp = 150.0;
		      int e=0;
		      int c=5;
		      if (code_seen('E')) e=code_value();
		        if (e<0)
		          temp=70;
		      if (code_seen('S')) temp=code_value();
		      if (code_seen('C')) c=code_value();
		      PID_autotune(temp, e, c);
		    }
		    break;
		    case 400: // M400 finish all moves
		    {
		      st_synchronize();
		    }
		    break;
		    case 500: // M500 Store settings in EEPROM
		    {
		        Config_StoreSettings();////////////////////////////////////////////////////////////////////////////////////////
		    }
		    break;
		    case 501: // M501 Read settings from EEPROM
		    {
		        Config_RetrieveSettings();////////////////////////////////////////////////////////////////////////////////////
		    }
		    break;
		    case 502: // M502 Revert to default settings
		    {
		        Config_ResetDefault();//////////////////////////////////////////////////////////////////////////////////////////
		    }
		    break;
		    case 503: // M503 print settings currently in memory
		    {
		        Config_PrintSettings();///////////////////////////////////////////////////////////////////////////////////////////
		    }
		    break;
	    #ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
		 case 540:
		    {
		        if(code_seen('S')) abort_on_endstop_hit = code_value() > 0;
		    }
		    break;
		#endif

	    #ifdef FILAMENTCHANGEENABLE
	    case 600: //Pause for filament change X[pos] Y[pos] Z[relative lift] E[initial retract] L[later retract distance for removal]
	    {
	        uint8_t cnt;
			float target[4];
	        float lastpos[4];
			
			LCD12864_Clear();
	        LCD12864_ShowString(0,0," Change Filament");
	        LCD12864_HightlightShow(1,0,16,1);
	        LCD12864_ShowString(2,0,"If change done ,   any back...");
			
	        target[X_AXIS]=current_position[X_AXIS];
	        target[Y_AXIS]=current_position[Y_AXIS];
	        target[Z_AXIS]=current_position[Z_AXIS];
	        target[E_AXIS]=current_position[E_AXIS];
	        lastpos[X_AXIS]=current_position[X_AXIS];
	        lastpos[Y_AXIS]=current_position[Y_AXIS];
	        lastpos[Z_AXIS]=current_position[Z_AXIS];
	        lastpos[E_AXIS]=current_position[E_AXIS];
	        //retract by E
	        if(code_seen('E'))
	        {
	          target[E_AXIS]+= code_value();
	        }
	        else
	        {
	          #ifdef FILAMENTCHANGE_FIRSTRETRACT
	            target[E_AXIS]+= FILAMENTCHANGE_FIRSTRETRACT ;
	          #endif
	        }
	        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);
	
	        //lift Z
	        if(code_seen('Z'))
	        {
	          target[Z_AXIS]+= code_value();
	        }
	        else
	        {
	          #ifdef FILAMENTCHANGE_ZADD
	            target[Z_AXIS]+= FILAMENTCHANGE_ZADD ;
	          #endif
	        }
	        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);
	
	        //move xy
	        if(code_seen('X'))
	        {
	          target[X_AXIS]+= code_value();
	        }
	        else
	        {
	          #ifdef FILAMENTCHANGE_XPOS
	            target[X_AXIS]= FILAMENTCHANGE_XPOS ;
	          #endif
	        }
	        if(code_seen('Y'))
	        {
	          target[Y_AXIS]= code_value();
	        }
	        else
	        {
	          #ifdef FILAMENTCHANGE_YPOS
	            target[Y_AXIS]= FILAMENTCHANGE_YPOS ;
	          #endif
	        }
	
	        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);
	
	        if(code_seen('L'))
	        {
	          target[E_AXIS]+= code_value();
	        }
	        else
	        {
	          #ifdef FILAMENTCHANGE_FINALRETRACT
	            target[E_AXIS]+= FILAMENTCHANGE_FINALRETRACT ;
	          #endif
	        }
	
	        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder);
	
	        //finish moves
	        st_synchronize();
	        //disable extruder steppers so filament can be removed
	        disable_e0();
	        disable_e1();
//	        disable_e2();
	        delay_ms(100);
	        cnt=0;
	         //while(!lcd_clicked)
            while(!keyPressed) //等待按键按下  				
			{
	          keyPressed = 0; //键值清零，必须有否则按下中键退出时会再次进入更换耗材菜单
			   cnt++;
	           manage_heater();
	           manage_inactivity();
//						 interface_update(); //屏幕界面更新
	           if(cnt==0) //cnt溢出时又会从0计数
	           {
				  BEEP_Ring(3);//beep();
	           }
	         }
	
	        //return to normal
	        if(code_seen('L'))
	        {
	          target[E_AXIS]+= -code_value();
	        }
	        else
	        {
	          #ifdef FILAMENTCHANGE_FINALRETRACT
	            target[E_AXIS]+=(-1)*FILAMENTCHANGE_FINALRETRACT ;
	          #endif
	        }
	        current_position[E_AXIS]=target[E_AXIS]; //the long retract of L is compensated by manual filament feeding
	        plan_set_e_position(current_position[E_AXIS]);
	        plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder); //should do nothing
	        plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], target[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder); //move xy back
	        plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], target[E_AXIS], feedrate/60, active_extruder); //move z back
	        plan_buffer_line(lastpos[X_AXIS], lastpos[Y_AXIS], lastpos[Z_AXIS], lastpos[E_AXIS], feedrate/60, active_extruder); //final untretract
	    
		   lcdDisplayUpdate = 1;
	       CurrentMenu->displayUpdate_f(); //恢复当前菜单的显示
		}
	    break;
	    #endif //FILAMENTCHANGEENABLE
	    case 907: // M907 Set digital trimpot motor current using axis codes.
	    {
	        for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) digipot_current(i,code_value());
	        if(code_seen('B')) digipot_current(4,code_value());
	        if(code_seen('S')) for(i=0;i<=4;i++) digipot_current(i,code_value());
	    }
	    break;
	    case 908: // M908 Control digital trimpot directly.
	    {
	        uint8_t channel,current;
	        if(code_seen('P')) channel=code_value();
	        if(code_seen('S')) current=code_value();
	        digipot_current(channel, current);
	    }
	    break;
	    case 350: // M350 Set microstepping mode. Warning: Steps per unit remains unchanged. S code sets stepping mode for all drivers.
	    { 
	        if(code_seen('S')) for( i=0;i<=4;i++) microstep_mode(i,code_value());
	        for( i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_mode(i,(uint8_t)code_value());
	        if(code_seen('B')) microstep_mode(4,code_value());
	        microstep_readings();
	    }
	    break;
	    case 351: // M351 Toggle MS1 MS2 pins directly, S# determines MS1 or MS2, X# sets the pin high/low.
	    {
	      if(code_seen('S')) switch((int)code_value())
	      {
	        case 1:
	          for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,code_value(),-1,-1);
	          if(code_seen('B')) microstep_ms(4,code_value(),-1,-1);
	          break;
	        case 2:
	          for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,-1,code_value(),-1);
	          if(code_seen('B')) microstep_ms(4,-1,code_value(),-1);
	          break;
					case 3:
	          for(i=0;i<NUM_AXIS;i++) if(code_seen(axis_codes[i])) microstep_ms(i,-1,-1,code_value());
	          if(code_seen('B')) microstep_ms(4,-1,-1,code_value());
	          break;
	      }
	      microstep_readings();
	
	    }
	    break;
	case 999: // M999: Restart after being stopped
	      Stopped = false;
	    //  lcd_reset_alert_level();//////////////////////////////////////
	      gcode_LastN = Stopped_gcode_LastN;
	      FlushSerialRequestResend();
	    break;		
	
	}//end switch(int) code_value();

  }
   else if(code_seen('T'))
   { tmp_extruder = code_value();
    if(tmp_extruder >= EXTRUDERS) {
      SERIAL_ECHO_START;
      printf("T%d",tmp_extruder);
     // SERIAL_ECHO(tmp_extruder);
      printf(MSG_INVALID_EXTRUDER);
    }
	else 
	{volatile bool make_move = false;
      if(code_seen('F')) 
	  {
        make_move = true;
        next_feedrate = code_value();
        if(next_feedrate > 0.0) {
          feedrate = next_feedrate;
        }
      }
	  #if EXTRUDERS > 1
	  if(tmp_extruder != active_extruder) 
	  {
        // Save current position to return to after applying extruder offset
        memcpy(destination, current_position, sizeof(destination));
        // Offset extruder (only by XY)
        for(i = 0; i < 2; i++) 
					{
           current_position[i] = current_position[i] -
                                 extruder_offset[i][active_extruder] +
                                 extruder_offset[i][tmp_extruder];
        }
        // Set the new active extruder and position
        active_extruder = tmp_extruder;
        plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        // Move to the old position if 'F' was in the parameters
        if(make_move && Stopped == false) {
           prepare_move();
        }
      }	//end   if(tmp_extruder != active_extruder) 
	   #endif
	  SERIAL_ECHO_START;
      printf(MSG_ACTIVE_EXTRUDER);
      printf("%d",active_extruder);
	  printf("\n");
	}
  }//end else if(code_seen('T'))
  else
  { SERIAL_ECHO_START;
    printf(MSG_UNKNOWN_COMMAND);
    printf("%s",cmdbuffer[bufindr]);
    printf("\"");
  }
   ClearToSend();
}

void FlushSerialRequestResend()  //清除串口请求重新发送
{
  //char cmdbuffer[bufindr][100]="Resend:";
  MYSERIAL_flush();  //清除串口消息
  printf(MSG_RESEND); //打印重新发送
  printf("%d\n",gcode_LastN + 1); //打印当前行号
  ClearToSend();
}

void ClearToSend()  //清除并发送
{
  previous_millis_cmd = millis(); //存储当前机器运行的时间长度
	
	#ifdef SDSUPPORT
  if(fromsd[bufindr])
    return;
  #endif //SDSUPPORT
	
  printf(MSG_OK);  //打印“OK”
  printf("\n");    
}

void get_coordinates()  //获取坐标值
{ int8_t i;
  volatile bool seen[4]={false,false,false,false};
  for( i=0; i < NUM_AXIS; i++) {
    if(code_seen(axis_codes[i]))  //如果读取到字符X
    {
      destination[i] = (float)code_value() + (axis_relative_modes[i] || relative_mode)*current_position[i]; //目标位置 = 读取到的数值 + 当前位置
      seen[i]=true;
    }
    else destination[i] = current_position[i]; //Are these else lines really needed? //当前坐标值赋值个目标坐标值
	#ifdef DEBUG_PRINTF 
	printf("%d：%f\n",i,destination[i]);  //打印轴号，目标值坐标
	#endif

  }
  if(code_seen('F')) {  //如果读取到字符F
    next_feedrate = code_value(); //下次进给速率 为字符F后面的数值
    if(next_feedrate > 0.0) feedrate = next_feedrate;  //将下次进给速率赋值给进给速率
  }
  	#ifdef DEBUG_PRINTF 
	printf("feedrate：%f\n",feedrate);  //打印进给速率
	#endif

  #ifdef FWRETRACT  //材料回抽
  if(autoretract_enabled)
  if( !(seen[X_AXIS] || seen[Y_AXIS] || seen[Z_AXIS]) && seen[E_AXIS])
  {
    float echange=destination[E_AXIS]-current_position[E_AXIS];
    if(echange<-MIN_RETRACT) //retract
    {
      if(!retracted)
      {

      destination[Z_AXIS]+=retract_zlift; //not sure why chaninging current_position negatively does not work.
      //if slicer retracted by echange=-1mm and you want to retract 3mm, corrrectede=-2mm additionally
      float correctede=-echange-retract_length;
      //to generate the additional steps, not the destination is changed, but inversely the current position
      current_position[E_AXIS]+=-correctede;
      feedrate=retract_feedrate;
      retracted=true;
      }

    }
    else
      if(echange>MIN_RETRACT) //retract_recover
    {
      if(retracted)
      {
      //current_position[Z_AXIS]+=-retract_zlift;
      //if slicer retracted_recovered by echange=+1mm and you want to retract_recover 3mm, corrrectede=2mm additionally
      float correctede=-echange+1*retract_length+retract_recover_length; //total unretract=retract_length+retract_recover_length[surplus]
      current_position[E_AXIS]+=correctede; //to generate the additional steps, not the destination is changed, but inversely the current position
      feedrate=retract_recover_feedrate;
      retracted=false;
      }
    }

  }
  #endif //FWRETRACT
}
void get_arc_coordinates(void)  //获取圆弧坐标
{
#ifdef SF_ARC_FIX
   bool relative_mode_backup = relative_mode;
   relative_mode = true;
#endif
   get_coordinates();
#ifdef SF_ARC_FIX
   relative_mode=relative_mode_backup;
#endif

   if(code_seen('I')) {  //如果读取到了I
     offset[0] = code_value();
   }
   else {
     offset[0] = 0.0;
   }
   if(code_seen('J')) {  // 如果读取到了J
     offset[1] = code_value();
   }
   else {
     offset[1] = 0.0;
   }
}
void clamp_to_software_endstops(float target[3])  //软件限位
{
  if (min_software_endstops) {
    if (target[X_AXIS] < min_pos[X_AXIS]) target[X_AXIS] = min_pos[X_AXIS];
    if (target[Y_AXIS] < min_pos[Y_AXIS]) target[Y_AXIS] = min_pos[Y_AXIS];
    if (target[Z_AXIS] < min_pos[Z_AXIS]) target[Z_AXIS] = min_pos[Z_AXIS];
  }

  if (max_software_endstops) {
    if (target[X_AXIS] > max_pos[X_AXIS]) target[X_AXIS] = max_pos[X_AXIS];
    if (target[Y_AXIS] > max_pos[Y_AXIS]) target[Y_AXIS] = max_pos[Y_AXIS];
    if (target[Z_AXIS] > max_pos[Z_AXIS]) target[Z_AXIS] = max_pos[Z_AXIS];
  }
}

void prepare_move(void)  //准备移动
{  int8_t i;
  clamp_to_software_endstops(destination); //软件限位
  previous_millis_cmd = millis();  //记录机器当前运行时间长度的历史值
  // Do not use feedmultiply for E or Z only moves  //只有E或Z轴移动时不要使用feedmultiply
  if( (current_position[X_AXIS] == destination [X_AXIS]) && (current_position[Y_AXIS] == destination [Y_AXIS])) {
      plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate/60, active_extruder);
  }
  else {
    plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate*feedmultiply/60/100.0, active_extruder);
  }
  for(i=0; i < NUM_AXIS; i++) {
    current_position[i] = destination[i]; //记录坐标的历史值
  }
}

void prepare_arc_move(u8 isclockwise) {  //准备圆弧运动  clockwise顺时针方向的
	int8_t i;
  float r = hypot(offset[X_AXIS], offset[Y_AXIS]); // Compute arc radius for mc_arc //估算圆弧半径

  // Trace the arc  //描绘圆弧轨迹
  mc_arc(current_position, destination, offset, X_AXIS, Y_AXIS, Z_AXIS, feedrate*feedmultiply/60/100.0, r, isclockwise, active_extruder);

  // As far as the parser（分析程序） is concerned, the position is now == target. In reality the
  // motion control system might still be processing the action and the real tool position
  // in any intermediate（中间的） location.
  for(i=0; i < NUM_AXIS; i++) {
    current_position[i] = destination[i]; //记录坐标的历史值
  }
  previous_millis_cmd = millis(); //记录机器当前运行时间长度的历史值
}
/*
#if defined(CONTROLLERFAN_PIN) && CONTROLLERFAN_PIN > -1

#if defined(FAN_PIN)
  #if CONTROLLERFAN_PIN == FAN_PIN 
    #error "You cannot set CONTROLLERFAN_PIN equal to FAN_PIN"
  #endif
#endif  
unsigned long lastMotor = 0; //Save the time for when a motor was turned on last
unsigned long lastMotorCheck = 0;

void controllerFan(void)
{
  if ((millis() - lastMotorCheck) >= 2500) //Not a time critical function, so we only check every 2500ms
  {
    lastMotorCheck = millis();

    if(!X_ENABLE_PIN || !Y_ENABLE_PIN || !Z_ENABLE_PIN
    #if EXTRUDERS > 2
       || !E2_ENABLE_PIN
    #endif
    #if EXTRUDER > 1
       || !E1_ENABLE_PIN
    #endif
       || !E0_ENABLE_PIN) //If any of the drivers are enabled...
    {
      lastMotor = millis(); //... set time to NOW so the fan will turn on
    }
    
    if ((millis() - lastMotor) >= (CONTROLLERFAN_SECS*1000UL) || lastMotor == 0) //If the last time any driver was enabled, is longer since than CONTROLLERSEC...   
    {
        digitalWrite(CONTROLLERFAN_PIN, 0); 
        analogWrite(CONTROLLERFAN_PIN, 0); 
    }
    else
    {
        // allows digital or PWM fan output to be used (see M42 handling)
        digitalWrite(CONTROLLERFAN_PIN, CONTROLLERFAN_SPEED);
        analogWrite(CONTROLLERFAN_PIN, CONTROLLERFAN_SPEED); 
    }
  }
}
#endif
*/

//管理不活跃状态
//这是为了在打印完成后关闭各个运行部件（从节能角度考虑），但打印过程中不该出现执行该函数的状况。
//目前在加热时会出现该状况，解决办法：适当加大stepper_inactive_time的值
void manage_inactivity(void) 
{
  //接收指令处于不活跃状态的时间
	if( (millis() - previous_millis_cmd) >  max_inactive_time )
	{
    if(max_inactive_time)
		{
      kill();
		}
	}
//	//打印完成后等待一段时间再关闭加热头散热风扇
//	if(hotend_inactive_time)
//	{
//	  if( (millis() - previous_millis_cmd) >  hotend_inactive_time )
//	  {
//          if(blocks_queued() == false) //block队列为空，
//		  {
//			  #if defined(FAN_0_PIN)		 
//			  FAN_0_PIN  = 0;    
//			  #endif //FAN_0_PIN
//			  #if defined(FAN_1_PIN)		 
//			  FAN_1_PIN  = 0;    
//			  #endif //FAN_1_PIN
//			  #if defined(FAN_BED_PIN)		 
//			  FAN_BED_PIN = 0;     
//			  #endif //FAN_BED_PIN	
//		  }			  
//	  }
//	}
	//步进电机处于不活跃状态的时间
	if(stepper_inactive_time)  
	{
		if( (millis() - previous_millis_cmd) >  stepper_inactive_time )
		{
			if(blocks_queued() == false) 
			{
				disable_x();
				disable_y();
				disable_z();
				disable_e0();
				disable_e1();
		     //   disable_e2();
		     }
	    }
	}
	check_axes_activity(); //检查轴的活跃性
}

//关闭中断，关闭加热，关闭步进电机
void kill(void) 
{
  CRITICAL_SECTION_START; // Stop interrupts
  disable_heater();

  disable_x();
  disable_y();
  disable_z();
  disable_e0();
  disable_e1();
 // disable_e2();

//#if defined(PS_ON_PIN) && PS_ON_PIN > -1
//  pinMode(PS_ON_PIN,INPUT);
//#endif  
  SERIAL_ERROR_START;
  printf(MSG_ERR_KILLED);
//  suicide();
  while(1) { /* Intentionally left empty */ } // Wait for reset
}

void Stop(void)
{
  disable_heater();
  if(Stopped == false) {

    Stopped = true;
    Stopped_gcode_LastN = gcode_LastN; // Save last g_code for restart
    SERIAL_ERROR_START;
    printf(MSG_ERR_STOPPED);
  }
}

bool IsStopped(void) 
{
	return Stopped; 
}
bool setTargetedHotend(int code)
{
  tmp_extruder = active_extruder;
  if(code_seen('T')) 
	{
    tmp_extruder = code_value();
    if(tmp_extruder >= EXTRUDERS) 
		{
      SERIAL_ECHO_START;
      switch(code)
			{
        case 104:
          printf(MSG_M104_INVALID_EXTRUDER);
          break;
        case 105:
          printf(MSG_M105_INVALID_EXTRUDER);
          break;
        case 109:
          printf(MSG_M109_INVALID_EXTRUDER);
          break;
        case 218:
          printf(MSG_M218_INVALID_EXTRUDER);
          break;
      }
      printf("%d",tmp_extruder);
      return true;
    }
  }
  return false ;
}
