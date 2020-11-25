#ifndef planner_h
#define planner_h

#include "marlin_main.h"


#define FLASH_SET_STORE_OFFSET                5*1024*1024
#define FLASH_SET_EEPROM_VERSION "V01"

#define AXIS_STEPS_PER_UNIT_X_ADDRESS        FLASH_SET_STORE_OFFSET+4		//104-107
#define AXIS_STEPS_PER_UNIT_Y_ADDRESS        FLASH_SET_STORE_OFFSET+8		//108-111
#define AXIS_STEPS_PER_UNIT_Z_ADDRESS        FLASH_SET_STORE_OFFSET+12	//112-115
#define AXIS_STEPS_PER_UNIT_E0_ADDRESS       FLASH_SET_STORE_OFFSET+16	//116-119
#define MAX_FEEDRATE_X_ADDRESS            	 FLASH_SET_STORE_OFFSET+20	//120-123
#define MAX_FEEDRATE_Y_ADDRESS        			 FLASH_SET_STORE_OFFSET+24	//124-127
#define MAX_FEEDRATE_Z_ADDRESS        			 FLASH_SET_STORE_OFFSET+28	//128-131
#define MAX_FEEDRATE_E0_ADDRESS        			 FLASH_SET_STORE_OFFSET+32	//132-135



//#define DEBUG_PLANNER


extern unsigned long minsegmenttime;
extern float max_feedrate[4]; // set the max speeds
extern float axis_steps_per_unit[4];
extern unsigned long max_acceleration_units_per_sq_second[4]; // Use M201 to override by software
extern float minimumfeedrate;
extern float acceleration;         // Normal acceleration mm/s^2  THIS IS THE DEFAULT ACCELERATION for all moves. M204 SXXXX
extern float retract_acceleration; //  mm/s^2   filament pull-pack and push-forward  while standing still in the other axis M204 TXXXX
extern float max_xy_jerk; //speed than can be stopped at once, if i understand correctly.
extern float max_z_jerk;
extern float max_e_jerk;
extern float mintravelfeedrate;
extern unsigned long axis_steps_per_sqr_second[NUM_AXIS];



#ifdef AUTOTEMP
    extern bool autotemp_enabled;
    extern float autotemp_max;
    extern float autotemp_min;
    extern float autotemp_factor;
#endif
// This struct is used when buffering the setup for each linear movement "nominal" values are as specified in 
// the source g-code and may never actually be reached if acceleration management is active.
typedef struct {
  // Fields used by the bresenham algorithm for tracing the line
  long steps_x, steps_y, steps_z, steps_e;  //每个坐标轴所需走的步数 Step count along each axis
  unsigned long step_event_count;           //完成这个block所需走的步数，上面4个的最大值 The number of step events required to complete this block
  long accelerate_until;                    //梯形曲线中的加速距离 The index of the step event on which to stop acceleration
  long decelerate_after;                    //加速和匀速的距离 The index of the step event on which to start decelerating
  long acceleration_rate;                   //加速率，用来计算加速度 The acceleration rate used for acceleration calculation
  unsigned char direction_bits;             //这个block的方向位，1反向，0正向，每一个位代表一个轴的方向 The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)
  unsigned char active_extruder;            //所有用到的有效挤出头 Selects the active extruder
  #ifdef ADVANCE
    long advance_rate;
    volatile long initial_advance;
    volatile long final_advance;
    float advance;
  #endif

  // Fields used by the motion planner to manage acceleration
//  float speed_x, speed_y, speed_z, speed_e;        // Nominal mm/sec for each axis
  float nominal_speed;                               //额定速度，即梯形曲线的匀速阶段速度 The nominal speed for this block in mm/sec 
  float entry_speed;                                 //进入速度，即从上一个block进入到这个block时的速度 Entry speed at previous-current junction in mm/sec
  float max_entry_speed;                             //最大进入速度，进入速度不能超过这个值 Maximum allowable junction entry speed in mm/sec
  float millimeters;                                 //总路程，单位mm The total travel of this block in mm
  float acceleration;                                //加速度，单位mm/sec^2    acceleration mm/sec^2
  unsigned char recalculate_flag;                    //连接处重新计算梯形速度曲线的标志 Planner flag to recalculate trapezoids on entry junction
  unsigned char nominal_length_flag;                 //能达到额定速度的标志 Planner flag for nominal speed always reached

  // Settings for the trapezoid generator     //梯形速度曲线产生器的设置参数
  unsigned long nominal_rate;                        //这个block的额定速度steps/sec         The nominal step rate for this block in step_events/sec 
  unsigned long initial_rate;                        //梯形曲线的初始速度/进入速度 The jerk-adjusted step rate at start of block  
  unsigned long final_rate;                          //梯形曲线的退出速度 The minimal rate at exit
  unsigned long acceleration_st;                     //加速度 acceleration steps/sec^2
  unsigned long fan_speed;                           //风扇速度
  #ifdef BARICUDA
  unsigned long valve_pressure;
  unsigned long e_to_p_pressure;
  #endif
  volatile char busy;//正在处理这个block的标志位，1表示正在执行这个block
} block_t;

void plan_init(void);
// Add a new linear movement to the buffer. x, y and z is the signed, absolute target position in 
// millimaters. Feed rate specifies the speed of the motion.
void plan_buffer_line(const float x, const float y, const float z, const float e, float feed_rate, const uint8_t extruder);

// Set position. Used for G92 instructions. //设定位置，用于G92指令
void plan_set_position(const float x, const float y, const float z, const float e);
void plan_set_e_position(const float e);

void check_axes_activity(void); 
uint8_t movesplanned(void); //return the nr of buffered moves

extern float axis_steps_per_unit[4];
// Called when the current block is no longer needed. Discards the block and makes the memory
// availible for new blocks.    
void plan_discard_current_block(void);
block_t *plan_get_current_block(void);
bool blocks_queued(void);

void allow_cold_extrudes(bool allow);
void reset_acceleration_rates(void);

#endif
