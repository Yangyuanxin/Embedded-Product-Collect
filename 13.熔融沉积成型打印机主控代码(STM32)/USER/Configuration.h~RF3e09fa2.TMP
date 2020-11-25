#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "sys.h"
//#include "setup.h"

// This configurstion file contains the basic settings.
// Advanced settings can be found in Configuration_adv.h
// BASIC SETTINGS: select your board type, temperature sensor type, axis scaling, and endstop configuration
//基本设置包括：主板类型，温度传感器类型，轴设置，限位开关设置

//User specified version info of this build to display in [Pronterface, etc] terminal window during startup.
//Implementation of an idea by Prof Braino to inform user that any changes made
//to this build by the user have been successfully uploaded into firmware.
#define STRING_VERSION_CONFIG_H __DATE__ " " __TIME__ // build date and time
#define STRING_CONFIG_H_AUTHOR "(none, default config)" //Who made the changes.

// SERIAL_PORT selects which serial port should be used for communication with the host.
// This allows the connection of wireless adapters (for instance) to non-default port pins.
// Serial port 0 is still used by the Arduino bootloader regardless of this setting.
//#define SERIAL_PORT 0

// This determines the communication speed of the printer//波特率设置
//#define BAUDRATE 250000
//#define BAUDRATE 115200

//// The following define selects which electronics board you have. Please choose the one that matches your setup//Dlion固件支持的主控板清单
// 10 = Gen7 custom (Alfons3 Version) "https://github.com/Alfons3/Generation_7_Electronics"
// 11 = Gen7 v1.1, v1.2 = 11
// 12 = Gen7 v1.3
// 13 = Gen7 v1.4
// 3  = MEGA/RAMPS up to 1.2 = 3
// 33 = RAMPS 1.3 / 1.4 (Power outputs: Extruder, Bed, Fan)
// 34 = RAMPS 1.3 / 1.4 (Power outputs: Extruder0, Extruder1, Bed)
// 4  = Duemilanove w/ ATMega328P pin assignment
// 5  = Gen6
// 51 = Gen6 deluxe
// 6  = Sanguinololu < 1.2
// 62 = Sanguinololu 1.2 and above
// 63 = Melzi
// 64 = STB V1.1
// 7  = Ultimaker
// 71 = Ultimaker (Older electronics. Pre 1.5.4. This is rare)
// 8  = Teensylu
// 80 = Rumba
// 81 = Printrboard (AT90USB1286)
// 82 = Brainwave (AT90USB646)
// 9  = Gen3+
// 70 = Megatronics
// 701= Megatronics v2.0
// 702= Minitronics v1.0
// 90 = Alpha OMCA board
// 91 = Final OMCA board
// 301 = Rambo

#ifndef MOTHERBOARD
//#define MOTHERBOARD 7
#endif

// This defines the number of extruders
#define EXTRUDERS 1

//// The following define selects which power supply you have. Please choose the one that matches your setup
// 1 = ATX
// 2 = X-Box 360 203Watts (the blue wire connected to PS_ON and the red wire to VCC)

#define POWER_SUPPLY 1

//===========================================================================
//=============================Thermal Settings  ============================
//===========================================================================
//
//--NORMAL IS 4.7kohm PULLUP!-- 1kohm pullup can be used on hotend sensor, using correct resistor and table
//
//// Temperature sensor settings:
// -2 is thermocouple with MAX6675 (only for sensor 0)
// -1 is thermocouple with AD595
// 0 is not used
// 1 is 100k thermistor - best choice for EPCOS 100k (4.7k pullup)//
// 2 is 200k thermistor - ATC Semitec 204GT-2 (4.7k pullup)
// 3 is mendel-parts thermistor (4.7k pullup)
// 4 is 10k thermistor !! do not use it for a hotend. It gives bad resolution at high temp. !!
// 5 is 100K thermistor - ATC Semitec 104GT-2 (Used in ParCan) (4.7k pullup)
// 6 is 100k EPCOS - Not as accurate as table 1 (created using a fluke thermocouple) (4.7k pullup)
// 7 is 100k Honeywell thermistor 135-104LAG-J01 (4.7k pullup)
// 8 is 100k 0603 SMD Vishay NTCS0603E3104FXT (4.7k pullup)
// 9 is 100k GE Sensing AL03006-58.2K-97-G1 (4.7k pullup)
// 10 is 100k RS thermistor 198-961 (4.7k pullup)
//
//    1k ohm pullup tables - This is not normal, you would have to have changed out your 4.7k for 1k
//                          (but gives greater accuracy and more stable PID)
// 51 is 100k thermistor - EPCOS (1k pullup)
// 52 is 200k thermistor - ATC Semitec 204GT-2 (1k pullup)
// 55 is 100k thermistor - ATC Semitec 104GT-2 (Used in ParCan) (1k pullup)


#define TEMP_SENSOR_0 1//   1代表选择    100k的热敏电阻作为传感器                                                                 //1.传感器类别选择
#define TEMP_SENSOR_1 0
#define TEMP_SENSOR_2 0
#define TEMP_SENSOR_BED 1

// Actual temperature must be close to target for this long before M109 returns success
#define TEMP_RESIDENCY_TIME 10  // (seconds)
#define TEMP_HYSTERESIS 3       // (degC) range of +/- temperatures considered "close" to the target one
//这个值决定当实际温度在正负3度是被认为接近设定的目标温度值。适当加大该值可以减少等待升温的时间，过大则对出丝不利，建议该值保持默认
#define TEMP_WINDOW     1       // (degC) Window around target to start the recidency timer x degC early.

// The minimal temperature defines the temperature below which the heater will not be enabled It is used
// to check that the wiring to the thermistor is not broken.
// Otherwise this would lead to the heater being powered on all the time.
#define HEATER_0_MINTEMP 1                                                                                                        //2.设置喷嘴和热床最低和最高温度
#define HEATER_1_MINTEMP 1
#define HEATER_2_MINTEMP 1
#define BED_MINTEMP 1
//设置喷嘴和热床的最低温度，低于该温度时，打印机将不能启动，表现为报错，并且加热头和热床的加热无法打开

// When temperature exceeds max temp, your heater will be switched off.
// This feature exists to protect your hotend from overheating accidentally, but *NOT* from thermistor short/failure!
// You should use MINTEMP for thermistor short/failure protection.
#define HEATER_0_MAXTEMP 275                                                                                      //修改1
#define HEATER_1_MAXTEMP 275
#define HEATER_2_MAXTEMP 275
#define BED_MAXTEMP 150                                                                                                 //修改2
//上面是喷嘴和热床的最高温度设置，防止烧坏主板

// If your bed has low resistance e.g. .6 ohm and throws the fuse you can duty cycle it to reduce the
// average current. The value should be an integer and the heat bed will be turned on for 1 interval of
// HEATER_BED_DUTY_CYCLE_DIVIDER intervals.
//#define HEATER_BED_DUTY_CYCLE_DIVIDER 4
//上面这个是配置为了防止热床电阻太小，长时间加热容易烧坏mos管，增加这个数字，可以防止mos管过热，但加热时间会增大

//下面是关于PID调节的参数                                                                                                          //3.PID调节配置     
/*PID温控配置，PID参数设置，需要根据系统情况设置，可以通过M303代码调用PID_autotune函数获得基本PID参数，
然后参照修改下面的DEFAULT_Kp、DEFAULT_Ki、DEFAULT_Kd，例如：M303 E0 C8 S190，表示获取打印头0、目标温度
设置为190度、循环调用PID_autotune  8伺候的相应的PID参数。系统自动生成的PID参数相当不错，可保持默认。
当然机器不同，也不一定都有效
*/
// PID settings:    
// Comment the following line to disable PID and enable bang-bang.
#define PIDTEMP
#define BANG_MAX 256 // limits current to nozzle while in bang-bang mode; 256=full current
#define PID_MAX 256 // limits current to nozzle while PID is active (see PID_FUNCTIONAL_RANGE below); 256=full current
#ifdef PIDTEMP
//#define PID_DEBUG // Sends debug data to the serial port.
  //#define PID_OPENLOOP 1 // Puts PID in open loop. M104/M140 sets the output power from 0 to PID_MAX
  #define PID_FUNCTIONAL_RANGE 10 // If the temperature difference between the target temperature and the actual temperature
                                  // is more then PID_FUNCTIONAL_RANGE then the PID will be shut off and the heater will be set to min/max.
  #define PID_INTEGRAL_DRIVE_MAX 255  //limit for the integral term
  #define K1 0.95 //smoothing factor withing the PID
  #define PID_dT ((58.0)/1000) //sampling period of the temperature routine


// If you are using a preconfigured hotend then you can use one of the value sets by uncommenting it
// Ultimaker
    #define  DEFAULT_Kp 22.2
    #define  DEFAULT_Ki 1.08
    #define  DEFAULT_Kd 144

// Makergear
//    #define  DEFAULT_Kp 7.0
//    #define  DEFAULT_Ki 0.1
//    #define  DEFAULT_Kd 12

// Mendel Parts V9 on 12V
//    #define  DEFAULT_Kp 63.0
//    #define  DEFAULT_Ki 2.25
//    #define  DEFAULT_Kd 440
#endif // PIDTEMP

// Bed Temperature Control
// Select PID or bang-bang with PIDTEMPBED.  If bang-bang, BED_LIMIT_SWITCHING will enable hysteresis
//
// uncomment this to enable PID on the bed.   It uses the same frequency PWM as the extruder.
// If your PID_dT above is the default, and correct for your hardware/configuration, that means 7.689Hz,
// which is fine for driving a square wave into a resistive load and does not significantly impact you FET heating.
// This also works fine on a Fotek SSR-10DA Solid State Relay into a 250W heater.
// If your configuration is significantly different than this and you don't understand the issues involved, you proabaly
// shouldn't use bed PID until someone else verifies your hardware works.
// If this is enabled, find your own PID constants below.
//#define PIDTEMPBED
//
//#define BED_LIMIT_SWITCHING

// This sets the max power delived to the bed, and replaces the HEATER_BED_DUTY_CYCLE_DIVIDER option.
// all forms of bed control obey this (PID, bang-bang, bang-bang with hysteresis)
// setting this to anything other than 256 enables a form of PWM to the bed just like HEATER_BED_DUTY_CYCLE_DIVIDER did,
// so you shouldn't use it unless you are OK with PWM on your bed.  (see the comment on enabling PIDTEMPBED)
#define MAX_BED_POWER 256 // limits duty cycle to bed; 256=full current

#ifdef PIDTEMPBED
//120v 250W silicone heater into 4mm borosilicate (MendelMax 1.5+)
//from FOPDT model - kp=.39 Tp=405 Tdead=66, Tc set to 79.2, argressive factor of .15 (vs .1, 1, 10)
    #define  DEFAULT_bedKp 10.00
    #define  DEFAULT_bedKi .023
    #define  DEFAULT_bedKd 305.4

//120v 250W silicone heater into 4mm borosilicate (MendelMax 1.5+)
//from pidautotune
//    #define  DEFAULT_bedKp 97.1
//    #define  DEFAULT_bedKi 1.41
//    #define  DEFAULT_bedKd 1675.16

// FIND YOUR OWN: "M303 E-1 C8 S90" to run autotune on the bed at 90 degreesC for 8 cycles.
#endif // PIDTEMPBED



//this prevents dangerous Extruder moves, i.e. if the temperature is under the limit
//can be software-disabled for whatever purposes by
#define PREVENT_DANGEROUS_EXTRUDE
//if PREVENT_DANGEROUS_EXTRUDE is on, you can still disable (uncomment) very long bits of extrusion separately.
#define PREVENT_LENGTHY_EXTRUDE

#define EXTRUDE_MINTEMP 170//该值防止挤出头温度未达到设定目标温度而进行挤出操作时的潜在风险
#define EXTRUDE_MAXLENGTH (X_MAX_LENGTH+Y_MAX_LENGTH) //prevent extrusion of very large distances.
//该值限制挤出的最大长度，超过该长度，挤出机不动作

//===========================================================================
//=============================Mechanical Settings===========================
//===========================================================================
//机械设置开始

// Uncomment the following line to enable CoreXY kinematics
// #define COREXY

// corse Endstop Settings
#define ENDSTOPPULLUPS // Comment this out (using // at the start of the line) to disable the endstop pullup resistors
//限位开关上拉电阻配置，如果你使用的是机械式的限位开关，请保留此部分

//机械式限位开关请保持该处不动
#ifndef ENDSTOPPULLUPS//没有配置限位开关上拉电阻时，限位开关上拉电阻细分控制
  // fine Enstop settings: Individual Pullups. will be ignord if ENDSTOPPULLUPS is defined
  #define ENDSTOPPULLUP_XMAX
  #define ENDSTOPPULLUP_YMAX
  #define ENDSTOPPULLUP_ZMAX
  #define ENDSTOPPULLUP_XMIN
  #define ENDSTOPPULLUP_YMIN
  //#define ENDSTOPPULLUP_ZMIN
#endif

#ifdef ENDSTOPPULLUPS//配置限位开关上拉电阻时，限位开关上拉电阻细分控制
  #define ENDSTOPPULLUP_XMAX
  #define ENDSTOPPULLUP_YMAX
  #define ENDSTOPPULLUP_ZMAX
  #define ENDSTOPPULLUP_XMIN
  #define ENDSTOPPULLUP_YMIN
  #define ENDSTOPPULLUP_ZMIN
#endif

//配置限位开关打开，把距离信号转为数字信号到单片机                                                                            //4.限位开关上拉开关设置，如果直接将机械限位开关接地，需要开启上拉
// The pullups are needed if you directly connect a mechanical endswitch between the signal and ground pins.
#define X_ENDSTOPS_INVERTING   1 // set to true to invert the logic of the endstops.                    //修改3
#define Y_ENDSTOPS_INVERTING   1 // set to true to invert the logic of the endstops.
#define Z_ENDSTOPS_INVERTING   1 // set to true to invert the logic of the endstops.
//#define DISABLE_MAX_ENDSTOPS

// For Inverting Stepper Enable Pins (Active Low) use 0, Non Inverting (Active High) use 1                                     //5.电机反向使能？
#define X_ENABLE_ON 0
#define Y_ENABLE_ON 0
#define Z_ENABLE_ON 0
#define E_ENABLE_ON 0 // For all extruders

//如果机器z轴有手动调整的部件，可以将DISABLE_Z设置为1，这样打印机打印时，可以手动调整z轴了
// Disables axis when it's not being used. 当哪个轴不运动时是否关闭电机。
#define DISABLE_X 0
#define DISABLE_Y 0
#define DISABLE_Z 0
#define DISABLE_E 0 // For all extruders

//原则是要保证原点在打印平台的坐下角([0,0]),或右上角([max,max])。                                                              //6.电机方向控制
//电机运动方向控制。由于电机连线不同，电机的运动方向也不同，但打印机的0点位置在左下角，如果电机的运动方向
 //与控制方向不同，则可以将下面参数值true和false对调，也可以将步进电机的4根线反过来插。
#define INVERT_X_DIR false    // for Mendel set to false, for Orca set to true                                      //修改4
#define INVERT_Y_DIR false  // for Mendel set to true, for Orca set to false
#define INVERT_Z_DIR true    // for Mendel set to false, for O      rca set to true
#define INVERT_E0_DIR false  // for direct drive extruder v9 set to true, for geared extruder set to false
#define INVERT_E1_DIR false    // for direct drive extruder v9 set to true, for geared extruder set to false
//#define INVERT_E2_DIR 0   // for direct drive extruder v9 set to true, for geared extruder set to false

// ENDSTOP SETTINGS://回原点方向配置                                                                                            //7.回原点方向,限位开关
// Sets direction of endstops when homing; 1=MAX, -1=MIN
#define X_HOME_DIR -1
#define Y_HOME_DIR -1
#define Z_HOME_DIR -1

#define min_software_endstops true //If true, axis won't move to coordinates less than HOME_POS.
#define max_software_endstops true  //If true, axis won't move to coordinates greater than the defined lengths below.

//这几个参数是配置打印尺寸的重要参数。这里需要说明的是坐标原点并不是打印中心，真正的打印中心一般在                             //8.设置各轴的打印尺寸
//[[(x.max-x.min)/2,(y.max-y.min)/2]]的位置。中心位置坐标需要在后面的切片工具中使用到，打印中心
//坐标应该与这里的参数配置匹配，否则很可能会打印到平台以外
// Travel limits after homing
#define X_MAX_POS 130                                                                                               //修改5
#define X_MIN_POS 0
#define Y_MAX_POS 130
#define Y_MIN_POS 0
#define Z_MAX_POS 120
#define Z_MIN_POS 0

#define X_MAX_LENGTH (X_MAX_POS - X_MIN_POS)
#define Y_MAX_LENGTH (Y_MAX_POS - Y_MIN_POS)
#define Z_MAX_LENGTH (Z_MAX_POS - Z_MIN_POS)

// The position of the homing switches
//归位开关设置
//#define MANUAL_HOME_POSITIONS  // If defined, MANUAL_*_HOME_POS below will be used
//如果开启该配置，下面 MANUAL_*_HOME_POS配置将生效
//#define BED_CENTER_AT_0_0  // If defined, the center of the bed is at (X=0, Y=0)
//如果开启该配置，热床的中心位置在(X=0, Y=0) 

//手动设置回原点位置。使用该功能后，默认回原点位置将是你所设定的一下三个值的位置
//Manual homing switch locations:
//对于三角洲这意味着笛卡尔打印机的顶部和中心的值。
#define MANUAL_X_HOME_POS 0
#define MANUAL_Y_HOME_POS 0
#define MANUAL_Z_HOME_POS 0

//// MOVEMENT SETTINGS//轴设置                                                                                                  //9.电机轴的数量        
#define NUM_AXIS 4 // The axis order in all axis related arrays is X, Y, Z, E
//轴的数量，各轴的配置是顺序是X, Y, Z, E
#define HOMING_FEEDRATE {40*60, 40*60, 10*60, 0}  // set the homing speeds (mm/min)                                             //10.各轴归位时的速度
//配置归位时的速度，单位是mm/mm
// default settings

#define DEFAULT_AXIS_STEPS_PER_UNIT   {80,80,4000,94.4962144}  // default steps per unit for ultimaker                         //11.各轴每移动1mm时需要的步数(有细分再乘以细分)
//默认各轴移动1mm时需要的步数(脉冲数)，计费方法参照： http://calculator.josefprusa.cz/                                                                        //修改6
//各轴脉冲数的计算方法：
//X、Y轴：步进电机每转步数(1.8度步距角的电机为200，0.9度步距角的电机为400)*步进电机驱动细分配置(一般16细分)/同步带齿间距/同步轮齿数
//Z轴：步进电机每转步数*步进电机驱动细分配置/丝杠导程
//E轴：步进电机每转步数*步进电机驱动细分配置*挤出机齿轮传动比/挤出轮周长


#define DEFAULT_MAX_FEEDRATE          {200, 200, 4, 45}    // (mm/sec)                                                          //12.各轴电机的最大速度
//各轴步进电机的速度设置。当你Z轴往上运动比较困难的时候，修改Z轴速度（调小，我这里把默认的5改成2.5了）                                                   //修改7
//配置各电机的最高速度。过高的值需要更大的电流输出，这将导致电机过热，并且有可能使电机在打印时失步。一般可设置为200-400

                                                                                                                                //13.各轴电机的最大加速度
#define DEFAULT_MAX_ACCELERATION      {3000,3000,100,3000}     // X, Y, Z, E maximum start speed for accelerated moves. E default values are good for skeinforge 40+, for older versions raise them a lot.
//该配置为电机最大加速度。过高的电机加速度将导致电机在打印动作时过冲，从而丢失，建议将X\Y最大加速度修改为1000-3000

//默认打印加速度                                                                                                                //14.各轴默认的加速度
#define DEFAULT_ACCELERATION          1000    // X, Y, Z and E max acceleration in mm/s^2 for printing moves
//默认回轴加速度
#define DEFAULT_RETRACT_ACCELERATION  1000   // X, Y, Z and E max acceleration in mm/s^2 for r retracts                         //15.各轴默认的回轴加速度

// Offset of the extruders (uncomment if using more than one and relying on firmware to position when changing).
// The offset has to be X=0, Y=0 for the extruder 0 hotend (default extruder).
// For the other hotends it is their distance from the extruder 0 hotend.
// #define EXTRUDER_OFFSET_X {0.0, 20.00} // (in mm) for each extruder, offset of the hotend on the X axis
// #define EXTRUDER_OFFSET_Y {0.0, 5.00}  // (in mm) for each extruder, offset of the hotend on the Y axis

//该配置为加速度变化率。该值过大同样会导致电机丢失。当该值处于合理范围内的较小值时，打印动作将更平滑、
//打印机机械应力将更小、材料在换向时将有更好的附着力、打印噪声也将降低；当该值处于合理范围内的较大值，
//打印时间将缩短。建议谨慎修改该值，最好不动
// The speed change that does not require acceleration (i.e. the software might assume it can be done instanteneously)
//各轴不需要加速的距离，即无需加速，立即完成的距离（即软件认为他可以在瞬间完成的）
#define DEFAULT_XYJERK                 20.0       // (mm/sec)
#define DEFAULT_ZJERK                 0.4     // (mm/sec)
#define DEFAULT_EJERK                 5.0    // (mm/sec)

//===========================================================================
//=============================Additional Features===========================
//===========================================================================

// EEPROM
// the microcontroller can store settings in the EEPROM, e.g. max velocity...
// M500 - stores paramters in EEPROM
// M501 - reads parameters from EEPROM (if you need reset them after you changed them temporarily).
// M502 - reverts to the default "factory settings".  You still need to store them in EEPROM afterwards if you want to.
//define this to enable eeprom support
//是否开启EEPROM，开启后，可以通过Gcode或LCD来修改，载入，保存相关参数。
//#define EEPROM_SETTINGS
//to disable EEPROM Serial responses and decrease program space by ~1700 byte: comment this out:
// please keep turned on if you can.
//关闭串口修改EEPROM的功能
//#define EEPROM_CHITCHAT

// Preheat Constants //预热参数配置，无需作修改
#define PLA_PREHEAT_HOTEND_TEMP 180 
#define PLA_PREHEAT_HPB_TEMP 50
#define PLA_PREHEAT_FAN_SPEED 255   // Insert Value between 0 and 255

#define ABS_PREHEAT_HOTEND_TEMP 240
#define ABS_PREHEAT_HPB_TEMP 80
#define ABS_PREHEAT_FAN_SPEED 255   // Insert Value between 0 and 255

//LCD and SD support//LCD和SD卡配置，请根据你的LCD板子做相应配置，再次不作说明
//#define ULTRA_LCD  //general lcd support, also 16x2
//#define DOGLCD  // Support for SPI LCD 128x64 (Controller ST7565R graphic Display Family)
//#define SDSUPPORT // Enable SD Card Support in Hardware Console
//#define SDSLOW // Use slower SD transfer mode (not normally needed - uncomment if you're getting volume init error)

//#define ULTIMAKERCONTROLLER //as available from the ultimaker online store.
//#define ULTIPANEL  //the ultipanel as on thingiverse

// The RepRapDiscount Smart Controller (white PCB)
// http://reprap.org/wiki/RepRapDiscount_Smart_Controller
//#define REPRAP_DISCOUNT_SMART_CONTROLLER

// The GADGETS3D G3D LCD/SD Controller (blue PCB)
// http://reprap.org/wiki/RAMPS_1.3/1.4_GADGETS3D_Shield_with_Panel
//#define G3D_PANEL

// The RepRapDiscount FULL GRAPHIC Smart Controller (quadratic white PCB)
// http://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller
//
// ==> REMEMBER TO INSTALL U8glib to your ARDUINO library folder: http://code.google.com/p/u8glib/wiki/u8glib
//#define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER

// The RepRapWorld REPRAPWORLD_KEYPAD v1.1
// http://reprapworld.com/?products_details&products_id=202&cPath=1591_1626
//#define REPRAPWORLD_KEYPAD
//#define REPRAPWORLD_KEYPAD_MOVE_STEP 10.0 // how much should be moved when a key is pressed, eg 10.0 means 10mm per click
/*
//automatic expansion
#if defined (REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER)
 #define DOGLCD
 #define U8GLIB_ST7920
 #define REPRAP_DISCOUNT_SMART_CONTROLLER
#endif

#if defined(ULTIMAKERCONTROLLER) || defined(REPRAP_DISCOUNT_SMART_CONTROLLER) || defined(G3D_PANEL)
 #define ULTIPANEL
 #define NEWPANEL
#endif

#if defined(REPRAPWORLD_KEYPAD)
  #define NEWPANEL
  #define ULTIPANEL
#endif

//I2C PANELS

//#define LCD_I2C_SAINSMART_YWROBOT
#ifdef LCD_I2C_SAINSMART_YWROBOT
  // This uses the LiquidCrystal_I2C library ( https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home )
  // Make sure it is placed in the Arduino libraries directory.
  #define LCD_I2C_TYPE_PCF8575
  #define LCD_I2C_ADDRESS 0x27   // I2C Address of the port expander
  #define NEWPANEL
  #define ULTIPANEL 
#endif

// PANELOLU2 LCD with status LEDs, separate encoder and click inputs
//#define LCD_I2C_PANELOLU2
#ifdef LCD_I2C_PANELOLU2
  // This uses the LiquidTWI2 library v1.2.3 or later ( https://github.com/lincomatic/LiquidTWI2 )
  // Make sure the LiquidTWI2 directory is placed in the Arduino or Sketchbook libraries subdirectory.
  // (v1.2.3 no longer requires you to define PANELOLU in the LiquidTWI2.h library header file)
  // Note: The PANELOLU2 encoder click input can either be directly connected to a pin 
  //       (if BTN_ENC defined to != -1) or read through I2C (when BTN_ENC == -1). 
  #define LCD_I2C_TYPE_MCP23017
  #define LCD_I2C_ADDRESS 0x20 // I2C Address of the port expander
  #define LCD_USE_I2C_BUZZER //comment out to disable buzzer on LCD
  #define NEWPANEL
  #define ULTIPANEL 
#endif

// Panucatt VIKI LCD with status LEDs, integrated click & L/R/U/P buttons, separate encoder inputs
//#define LCD_I2C_VIKI
#ifdef LCD_I2C_VIKI
  // This uses the LiquidTWI2 library v1.2.3 or later ( https://github.com/lincomatic/LiquidTWI2 )
  // Make sure the LiquidTWI2 directory is placed in the Arduino or Sketchbook libraries subdirectory.
  // Note: The pause/stop/resume LCD button pin should be connected to the Arduino
  //       BTN_ENC pin (or set BTN_ENC to -1 if not used)
  #define LCD_I2C_TYPE_MCP23017 
  #define LCD_I2C_ADDRESS 0x20 // I2C Address of the port expander
  #define LCD_USE_I2C_BUZZER //comment out to disable buzzer on LCD (requires LiquidTWI2 v1.2.3 or later)
  #define NEWPANEL
  #define ULTIPANEL 
#endif
 
#ifdef ULTIPANEL
//  #define NEWPANEL  //enable this if you have a click-encoder panel
  #define SDSUPPORT
  #define ULTRA_LCD
  #ifdef DOGLCD // Change number of lines to match the DOG graphic display
    #define LCD_WIDTH 20
    #define LCD_HEIGHT 5
  #else
    #define LCD_WIDTH 20
    #define LCD_HEIGHT 4
  #endif
 
#else //no panel but just lcd
  #ifdef ULTRA_LCD
  #ifdef DOGLCD // Change number of lines to match the 128x64 graphics display
    #define LCD_WIDTH 20
    #define LCD_HEIGHT 5
  #else
    #define LCD_WIDTH 16
    #define LCD_HEIGHT 2
  #endif
  #endif
#endif
*/
// Increase the FAN pwm frequency. Removes the PWM noise but increases heating in the FET/Arduino
//#define FAST_PWM_FAN

// Use software PWM to drive the fan, as for the heaters. This uses a very low frequency
 // which is not ass annoying as with the hardware PWM. On the other hand, if this frequency
 // is too low, you should also increment SOFT_PWM_SCALE.
 //风扇的软PWM开启
 //#define FAN_SOFT_PWM 

// M240  Triggers a camera by emulating a Canon RC-1 Remote
// Data from: http://www.doc-diy.net/photo/rc-1_hacked/
// #define PHOTOGRAPH_PIN     23

// SF send wrong arc g-codes when using Arc Point as fillet procedure
//#define SF_ARC_FIX

// Support for the BariCUDA Paste Extruder.
//#define BARICUDA
//#define SDSUPPORT
/*********************************************************************\
*
* R/C SERVO support
*
* Sponsored by TrinityLabs, Reworked by codexmas
*
**********************************************************************/

// Number of servos
//
// If you select a configuration below, this will receive a default value and does not need to be set manually
// set it manually if you have more servos than extruders and wish to manually control some
// leaving it undefined or defining as 0 will disable the servo subsystem
// If unsure, leave commented / disabled
//
// #define NUM_SERVOS 3

#include "Configuration_adv.h"

#endif //__CONFIGURATION_H
