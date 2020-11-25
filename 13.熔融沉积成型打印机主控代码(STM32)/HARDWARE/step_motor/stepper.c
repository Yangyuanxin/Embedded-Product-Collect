#include "stepper.h"
#include "marlin_main.h"
#include "language.h"
//#include "Configuration_adv.h"
//#include "Configuration.h"
//#include "speed_lookuptable.h" //实际上没有采用查表的方法
#include "temperature.h"
#include "usart.h"
#include "timer.h"
//#include "spi.h"

#ifdef DEBUG_STEPPER
  unsigned int add_interrupt_time = 0; //为调试时统计梯形曲线匀速段时间
#endif

//下面这个定义宏，定义的是数字电位器的引脚，在此没有用到数字电位器。
//且这种引脚定义方式不适用于STM32，后面如果要用数字电位器，在具体考虑怎么应用。
//#define DIGIPOT_CHANNELS {4,1,0,2,3} // X Y Z E0 E1 digipot channels to stepper driver mapping

//存储读取到的步进电机驱动的细分值，没有用到软件设置细分，直接硬件配置为16细分，故下面参数全初始化为16
static uint8_t subsection_x_value=16;
static uint8_t subsection_y_value=16;
static uint8_t subsection_z_value=16;
static uint8_t subsection_e0_value=16;
static uint8_t subsection_e1_value=16;


//===========================================================================
//=============================public variables  ============================
//===========================================================================
block_t *current_block;  // A pointer to the block currently being traced


//===========================================================================
//=============================private variables ============================
//===========================================================================
//static makes it inpossible to be called from outside of this file by extern.!

// Variables used by The Stepper Driver Interrupt
static unsigned char out_bits;        // The next stepping-bits to be output
static long counter_x,       // Counter variables for the bresenham line tracer
            counter_y, 
            counter_z,       
            counter_e;
volatile static unsigned long step_events_completed; //block的完成度// The number of step events executed in the current block //在当前block中被执行的步事件的数量
#ifdef ADVANCE  //（非活动）
  static long advance_rate, advance, final_advance = 0;
  static long old_advance = 0;
  static long e_steps[3];
#endif
static long acceleration_time, deceleration_time;
//static unsigned long accelerate_until, decelerate_after, acceleration_rate, initial_rate, final_rate, nominal_rate;
static unsigned short acc_step_rate; // needed for deccelaration start point
static char step_loops;
static unsigned short TIMER4_nominal;
static unsigned short step_loops_nominal;

volatile long endstops_trigsteps[3]={0,0,0};
volatile long endstops_stepsTotal,endstops_stepsDone;
static volatile bool endstop_x_hit=false;
static volatile bool endstop_y_hit=false;
static volatile bool endstop_z_hit=false;

#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED  //（非活动）
bool abort_on_endstop_hit = false;
#endif //ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED

static bool old_x_min_endstop=false;
static bool old_x_max_endstop=false;
static bool old_y_min_endstop=false;
static bool old_y_max_endstop=false;
static bool old_z_min_endstop=false;
static bool old_z_max_endstop=false;

static bool check_endstops = true;

volatile long count_position[NUM_AXIS] = { 0, 0, 0, 0}; //用于记录各轴自运动开始相对于起始运动点的位置
volatile signed char count_direction[NUM_AXIS] = { 1, 1, 1, 1}; //记录各轴的方向（脉冲方向值按脉冲步数叠加起来就是各轴的位置）

//===========================================================================
//=============================functions         ============================
//===========================================================================

#define CHECK_ENDSTOPS  if(check_endstops) //开启限位开关检测
//原版arduino程序的下列两个函数的定义是利用汇编代码完成的（汇编可以提高运算的速度），这里的定义采用C语言的方式，运算效率可能略低，但实现的功能一致。
#define MultiU24X24toH16(intRes, longIn1, longIn2) intRes= ((uint64_t)(longIn1) * (longIn2)) >> 24 //intRes = longIn1 * longIn2 >> 24
#define MultiU16X8toH16(intRes, charIn1, intIn2) intRes = ((charIn1) * (intIn2)) >> 16  // intRes = intIn1 * intIn2 >> 16

void checkHitEndstops(void)
{
 if( endstop_x_hit || endstop_y_hit || endstop_z_hit) {
   SERIAL_ECHO_START;
   printf(MSG_ENDSTOPS_HIT);
   if(endstop_x_hit) {
     printf(" X:%f",(float)endstops_trigsteps[X_AXIS]/axis_steps_per_unit[X_AXIS]);  //endstops_trigsteps[X_AXIS]存储的信息是什么？  
   }
   if(endstop_y_hit) {
     printf(" Y:%f",(float)endstops_trigsteps[Y_AXIS]/axis_steps_per_unit[Y_AXIS]);  
   }
   if(endstop_z_hit) {
     printf(" Z:%f",(float)endstops_trigsteps[Z_AXIS]/axis_steps_per_unit[Z_AXIS]); 
   }
   printf("\n");
   endstop_x_hit=false;
   endstop_y_hit=false;
   endstop_z_hit=false;
#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED //（非活动）
   if (abort_on_endstop_hit)
   {
     card.sdprinting = false;
     card.closefile();
     quickStop();
     setTargetHotend0(0);
     setTargetHotend1(0);
     setTargetHotend2(0);
   }
#endif
 }
}

void endstops_hit_on_purpose(void)  //限位开关在目标位置触发后，将其复位
{
  endstop_x_hit=false;
  endstop_y_hit=false;
  endstop_z_hit=false;
}

void enable_endstops(bool check)  //是否使能限位开关检测，并将状态信息储存到check_endstops变量中
{
  check_endstops = check;
}

//         __________________________
//        /|                        |\     _________________         ^
//       / |                        | \   /|               |\        |
//      /  |                        |  \ / |               | \       s
//     /   |                        |   |  |               |  \      p
//    /    |                        |   |  |               |   \     e
//   +-----+------------------------+---+--+---------------+----+    e
//   |               BLOCK 1            |      BLOCK 2          |    d
//
//                           time ----->
// 
//  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates 
//  first block->accelerate_until step_events_completed, then keeps going at constant speed until 
//  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
//  The slope of acceleration is calculated with the leib ramp alghorithm.

void st_wake_up(void)  //唤醒步进电机，开启TIMx中断
{ 
  ENABLE_STEPPER_DRIVER_INTERRUPT();  
}

void step_wait(void)  //步进等待
{
   u8 i;
    for( i=0; i < 6; i++){
    }
}
  

unsigned short calc_timer(unsigned short step_rate)  //计算中断时间
{
  unsigned short timer;
  if(step_rate > MAX_STEP_FREQUENCY) step_rate = MAX_STEP_FREQUENCY;//步进电机频率最大值限幅（40KHZ）
  
  if(step_rate > 20000) { // If steprate > 20kHz >> step 4 times
    step_rate = (step_rate >> 2)&0x3fff;  //该计算公式是什么意思？
    step_loops = 4;  //全局变量，记录循环次数
  }
  else if(step_rate > 10000) { // If steprate > 10kHz >> step 2 times
    step_rate = (step_rate >> 1)&0x7fff; //该计算公式是什么意思？
    step_loops = 2;
  }
  else {
    step_loops = 1;
  } 
  
  if(step_rate < 32) step_rate = 32;
  timer = 2000000/step_rate - 1;
	 
  if(timer < 100) { timer = 100; printf(MSG_STEPPER_TO_HIGH); printf("%d",step_rate); }//(20kHz this should never happen)
  return timer;
}

// Initializes the trapezoid generator from the current block. Called whenever a new 
// block begins. //初始化当前block的梯形发生器，每当一个新的block开始时都要调用
void trapezoid_generator_reset(void)
{
  #ifdef ADVANCE  //（非活动）
    advance = current_block->initial_advance;
    final_advance = current_block->final_advance;
    // Do E steps + advance steps
    e_steps[current_block->active_extruder] += ((advance >>8) - old_advance);
    old_advance = advance >>8;  
  #endif
	
  deceleration_time = 0; //设定减速段的时间
  // step_rate to timer interval
  TIMER4_nominal = calc_timer(current_block->nominal_rate); //计算额定速度段中断的时间
  // make a note of the number of step loops required at nominal speed //记下在额定速度下所需的步骤循环数量
  step_loops_nominal = step_loops;
  acc_step_rate = current_block->initial_rate; //设定加速度段的速度
  acceleration_time = calc_timer(acc_step_rate); //计算加速度段中断的时间
   TIM_SetAutoreload(TIM4, acceleration_time);
	#ifdef DEBUG_STEPPER
	printf("\n***trapezoid_generator_reset***\n");
	//为便于数据统计分析确保打印格式如下：如：初始速度=xxxx  时间差=
	printf("初始速度=%6d  ",acc_step_rate);
	printf("中断时间=%5ld  ",acceleration_time);
	add_interrupt_time += acceleration_time;
  printf("时间=%8d\n",add_interrupt_time);
	#endif
	
//    SERIAL_ECHO_START;
//    SERIAL_ECHOPGM("advance :");
//    SERIAL_ECHO(current_block->advance/256.0);
//    SERIAL_ECHOPGM("advance rate :");
//    SERIAL_ECHO(current_block->advance_rate/256.0);
//    SERIAL_ECHOPGM("initial advance :");
//  SERIAL_ECHO(current_block->initial_advance/256.0);
//    SERIAL_ECHOPGM("final advance :");
//    SERIAL_ECHOLN(current_block->final_advance/256.0);
    
}


// "The Stepper Driver Interrupt" - This timer interrupt is the workhorse（重负荷）.  
// It pops blocks from the block_buffer and executes them by pulsing the stepper pins appropriately.
//从block缓冲区中取出block并适当的通过脉冲步进脚执行它们
//定时器4中断服务程序
void TIM4_IRQHandler(void)   //TIM4中断
{ 
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
  {	  
	  uint8_t i; //后面用到的局部变量定义（keil软件要求局部变量在定义时，必须紧挨前面的大花括号，不能直接定义到使用之前，否则会报错）	  
	  unsigned short timer;
      unsigned short step_rate;
	  
	  TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //清除TIMx的中断待处理位:TIM 中断源  
  
	  // If there is no current block, attempt to pop one from the buffer//如果没有当前block，尝试从缓冲区中取出	 
	  if (current_block == NULL) // Anything in the buffer?
	  {   
		current_block = plan_get_current_block(); //从缓冲区中读取block
		  
		if (current_block != NULL) 
		{
		  current_block->busy = true;  //标记block状态为繁忙
		  trapezoid_generator_reset();  //梯形发生器复位
		  counter_x = -(current_block->step_event_count >> 1);  //step_event_count所有轴要移动步数的最大值 //不知道为什么这么赋值
		  counter_y = counter_x;
		  counter_z = counter_x;
		  counter_e = counter_x;
		  step_events_completed = 0; //清零，用于记录当前block的完成度
		  
		  #ifdef Z_LATE_ENABLE //（非活动）
			if(current_block->steps_z > 0) {
			  enable_z();
			  TIM_SetAutoreload(TIM4, 2000-1);//1ms wait  
			  return;
			}
		  #endif  //Z_LATE_ENABLE    

		} 
		else 
		{
		  TIM_SetAutoreload(TIM4, 2000-1); //设定定时器重新装载的计数周期值，2000在此为1ms
		}    
	  } 

  if (current_block != NULL) 
  {
    // Set directions,This should be done once during init of trapezoid. Endstops -> interrupt
    out_bits = current_block->direction_bits; //读取block中的各轴的方向。各轴的方向值是存储在一个数中的

    // Set direction en check limit switches //设定方向，使能限位开关检测
    if ((out_bits & (1<<X_AXIS)) != 0) //取出X轴的方向（0：正方向 1：负方向）// stepping along -X axis  //沿-X方向步进
	{   
      #if !defined COREXY  
        X_DIR_PIN = INVERT_X_DIR;
      #endif //NOT COREXY
      count_direction[X_AXIS] = -1;
      if(check_endstops) //CHECK_ENDSTOPS开启限位开关检测
      {
		#if defined X_MIN_PIN  //触碰到最小值处的限位开关
          bool x_min_endstop= X_MIN_PIN != X_MIN_ENDSTOP_INVERTING ; //未触碰到行程开关时值为0，触碰到值为1
          if(x_min_endstop && old_x_min_endstop && (current_block->steps_x > 0)) 
		      {
            endstops_trigsteps[X_AXIS] = count_position[X_AXIS];  //记录当前位置
            endstop_x_hit=true;  //设定x轴endstop的状态
            step_events_completed = current_block->step_event_count; //设定该block已经完成
          }
          old_x_min_endstop = x_min_endstop;
		#endif //defined X_MIN_PIN
      }
    }
    else  // X轴正方向
	{ 
      #if !defined COREXY  
       X_DIR_PIN=!INVERT_X_DIR;
      #endif //NOT COREXY   
      count_direction[X_AXIS]=1;
      if(check_endstops) //CHECK_ENDSTOPS开启限位开关检测 
      {  
		#if defined X_MAX_PIN
          bool x_max_endstop= X_MAX_PIN != X_MAX_ENDSTOP_INVERTING;
          if(x_max_endstop && old_x_max_endstop && (current_block->steps_x > 0))
		      {
            endstops_trigsteps[X_AXIS] = count_position[X_AXIS];
            endstop_x_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_x_max_endstop = x_max_endstop;
		#endif
      }
    }

    if ((out_bits & (1<<Y_AXIS)) != 0)  // Y轴负方向
	{   
      #if !defined COREXY  //NOT COREXY
        Y_DIR_PIN=INVERT_Y_DIR;
      #endif
      count_direction[Y_AXIS] = -1;
      if(check_endstops) //CHECK_ENDSTOPS开启限位开关检测
      {
        #if defined(Y_MIN_PIN)
          bool y_min_endstop=Y_MIN_PIN != Y_MIN_ENDSTOP_INVERTING;
          if(y_min_endstop && old_y_min_endstop && (current_block->steps_y > 0)) 
		      {
            endstops_trigsteps[Y_AXIS] = count_position[Y_AXIS];
            endstop_y_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_y_min_endstop = y_min_endstop;
        #endif
      }
    }
    else  //Y轴正方向 
	{ 
      #if !defined COREXY  //NOT COREXY
        Y_DIR_PIN=!INVERT_Y_DIR;
      #endif
      count_direction[Y_AXIS]=1;
      if(check_endstops) //CHECK_ENDSTOPS开启限位开关检测
      {
        #if defined(Y_MAX_PIN)// && Y_MAX_PIN > -1
          bool y_max_endstop=Y_MAX_PIN != Y_MAX_ENDSTOP_INVERTING;
          if(y_max_endstop && old_y_max_endstop && (current_block->steps_y > 0))
		      {
            endstops_trigsteps[Y_AXIS] = count_position[Y_AXIS];
            endstop_y_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_y_max_endstop = y_max_endstop;
        #endif
      }
    } 
    
    if ((out_bits & (1<<Z_AXIS)) != 0) //Z轴负方向
	{ 
      Z_DIR_PIN=INVERT_Z_DIR;     
      count_direction[Z_AXIS] = -1;
      if(check_endstops) //CHECK_ENDSTOPS开启限位开关检测
      {
        #if defined(Z_MIN_PIN)
          bool z_min_endstop= Z_MIN_PIN != Z_MIN_ENDSTOP_INVERTING;
          if(z_min_endstop && old_z_min_endstop && (current_block->steps_z > 0)) 
		      {
            endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
            endstop_z_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_z_min_endstop = z_min_endstop;
        #endif
      }
    }
    else //Z轴正方向 
	{ 
      Z_DIR_PIN=!INVERT_Z_DIR;
      count_direction[Z_AXIS] = 1;
      if(check_endstops) //CHECK_ENDSTOPS开启限位开关检测
      {
        #if defined(Z_MAX_PIN)
          bool z_max_endstop=Z_MAX_PIN != Z_MAX_ENDSTOP_INVERTING;
          if(z_max_endstop && old_z_max_endstop && (current_block->steps_z > 0)) 
		      {
            endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
            endstop_z_hit=true;
            step_events_completed = current_block->step_event_count;
          }
          old_z_max_endstop = z_max_endstop;
        #endif
      }
    }
	
    #ifndef ADVANCE	  //（非活动）
	
      if ((out_bits & (1<<E_AXIS)) != 0) {  // -direction
        REV_E_DIR();
        count_direction[E_AXIS]=-1;
      }
      else { // +direction
        NORM_E_DIR();
        count_direction[E_AXIS]=1;
      } 
    #endif //!ADVANCE   
	  
//正常情况下一个中断周期步进电机转一步，即中断周期的定时频率即为步进电机的脉冲频率。
//block中的step_event_count（步事件计数：各轴中要移动步数的最大值）正是完成该block	  
	// Take multiple steps per interrupt (For high speed moves)	//每个中断周期走多步	
    for(i=0; i < step_loops; i++)  //step_loops会依据clac_timer()函数来确定
	{  

      #ifdef ADVANCE  //（非活动）	  
      counter_e += current_block->steps_e;
      if (counter_e > 0) {
        counter_e -= current_block->step_event_count;
        if ((out_bits & (1<<E_AXIS)) != 0) { // - direction
          e_steps[current_block->active_extruder]--;
        }
        else {
          e_steps[current_block->active_extruder]++;
        }
      }   
	  
      #endif //ADVANCE

     
	  #if !defined COREXY   
        counter_x += current_block->steps_x;
        if (counter_x > 0)   //steps大于step_event_count的一半
		    {
          X_STEP_PIN= !INVERT_X_STEP_PIN; //true
          counter_x -= current_block->step_event_count;  //这句不知道是什么意思？
          count_position[X_AXIS]+=count_direction[X_AXIS];  //计算X轴的当前位置 
          X_STEP_PIN= INVERT_X_STEP_PIN; //false
        }
  
        counter_y += current_block->steps_y;
        if (counter_y > 0) 
		    {
          Y_STEP_PIN= !INVERT_Y_STEP_PIN; //true
          counter_y -= current_block->step_event_count; 
          count_position[Y_AXIS]+=count_direction[Y_AXIS]; //计算Y轴的当前位置 
          Y_STEP_PIN= INVERT_Y_STEP_PIN; //false
        }
      #endif  //!defined COREXY 
  
      counter_z += current_block->steps_z;
      if (counter_z > 0) 
	    {
        Z_STEP_PIN= !INVERT_Z_STEP_PIN; //true      
        counter_z -= current_block->step_event_count;
        count_position[Z_AXIS]+=count_direction[Z_AXIS]; //计算Z轴的当前位置 
        Z_STEP_PIN= INVERT_Z_STEP_PIN; //false
      }

      #ifndef ADVANCE  	  
        counter_e += current_block->steps_e;
        if (counter_e > 0) 
		    {
          WRITE_E_STEP(!INVERT_E_STEP_PIN); //true
          counter_e -= current_block->step_event_count;
          count_position[E_AXIS]+=count_direction[E_AXIS]; //计算E轴的当前位置 
          WRITE_E_STEP(INVERT_E_STEP_PIN); //false
        }		
      #endif //!ADVANCE	 
      step_events_completed += 1;   //更新block的进度
      if(step_events_completed >= current_block->step_event_count) break;
    }
	   
	// Calculare new timer value //计算新的定时中断的时间  
    if (step_events_completed <= (unsigned long int)current_block->accelerate_until) //梯形加速阶段
	{ 
       MultiU24X24toH16(acc_step_rate, acceleration_time, current_block->acceleration_rate);
       acc_step_rate += current_block->initial_rate;
      
      // upper limit //加速度的上限值
      if(acc_step_rate > current_block->nominal_rate)
         acc_step_rate = current_block->nominal_rate;

      // step_rate to timer interval //步进电机频率对应的时间间隔
      timer = calc_timer(acc_step_rate); //更新下次中断的时间
      TIM_SetAutoreload(TIM4, timer);
      acceleration_time += timer;
			#ifdef DEBUG_STEPPER
				//printf("****加速阶段***\n");
			  //为便于数据统计分析确保打印格式如下：如：初始速度=xxxx  时间差=
				printf("加速速度=%6d  ",acc_step_rate); //每次中断的步进电机速度：（step/s）
				printf("中断时间=%5d  ",timer);
			  add_interrupt_time += timer;
				printf("时间=%8d\n",add_interrupt_time);
			#endif
			
      #ifdef ADVANCE  //（非活动） 
        for(i=0; i < step_loops; i++) 
	    {
          advance += advance_rate;
        }
        //if(advance > current_block->advance) advance = current_block->advance;
        // Do E steps + advance steps
        e_steps[current_block->active_extruder] += ((advance >>8) - old_advance);
        old_advance = advance >>8;        
      #endif
		
    } 
    else if (step_events_completed > (unsigned long int)current_block->decelerate_after) //梯形减速阶段
	{
	      MultiU24X24toH16(step_rate, deceleration_time, current_block->acceleration_rate);
	      
	      if(step_rate > acc_step_rate) // Check step_rate stays positive
		  { 
	        step_rate = current_block->final_rate;
	      }
	      else 
		  {
	        step_rate = acc_step_rate - step_rate; //更新目前的速度// Decelerate from aceleration end point.//从加速度结束点开始减速
	      }
	
	      // lower limit
	      if(step_rate < current_block->final_rate) 
			  step_rate = current_block->final_rate;
	        	
	      // step_rate to timer interval
	      timer = calc_timer(step_rate);
	      TIM_SetAutoreload(TIM4, timer); //更新下次中断的时间
	      deceleration_time += timer;
				#ifdef DEBUG_STEPPER
					//printf("****减速阶段***\n");
					//为便于数据统计分析确保打印格式如下：如：初始速度=xxxx  时间差=
					printf("减速速度=%6d  ",step_rate); //每次中断的步进电机速度：（step/s）
					printf("中断时间=%5d  ",timer);
				  add_interrupt_time += timer;
					printf("时间=%8d\n",add_interrupt_time);
			  #endif

	      #ifdef ADVANCE   
	        for(i=0; i < step_loops; i++) {
	          advance -= advance_rate;
	        }
	        if(advance < final_advance) advance = final_advance;
	        // Do E steps + advance steps
	        e_steps[current_block->active_extruder] += ((advance >>8) - old_advance);
	        old_advance = advance >>8;   			
	      #endif //ADVANCE	
	}
	else 
	{
	   TIM_SetAutoreload(TIM4,TIMER4_nominal-1);
	   // ensure we're running at the correct step rate, even if we just came off an acceleration
     step_loops = step_loops_nominal;
		#ifdef DEBUG_STEPPER
			//printf("****匀速阶段***\n");
			//为便于数据统计分析确保打印格式如下：如：初始速度=xxxx  时间差=
			printf("匀速速度=%6ld  ",current_block->nominal_rate); //每次中断的步进电机速度：（step/s）
			printf("中断时间=%5d  ",TIMER4_nominal);
			add_interrupt_time += TIMER4_nominal;
			printf("时间=%8d\n",add_interrupt_time);
		#endif		
	}
    // If current block is finished, reset pointer //当前block执行完，复位指针
    if (step_events_completed >= current_block->step_event_count) 
	{
      current_block = NULL;
      plan_discard_current_block();
    }   
  } 
 }
}

void st_init(void)
{	
     //下面这两个注释掉的函数，在实际中并未用到，和这两块相关的函数在最后，后面也有相应说明	
//	 digipot_init(); //Initialize Digipot Motor Current //初始化数字电位器（用于步进电机驱动上调节电流）
//   microstep_init(); //Initialize Microstepping Pins  //初始化细分引脚（用于设置步进电机驱动的细分模式）
//默认从硬件上将步进电机驱动细分模式设置为16细分，电流通过调节驱动板上的模拟电位器来调节	
	
   //初始化步进电机引脚并将步进电机设为关闭状态
	 GPIO_InitTypeDef         GPIO_InitStructure;	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE);	 
	
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
   
	//初始化X轴步进电机驱动的引脚
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;	
	 GPIO_Init(GPIOF, &GPIO_InitStructure);
	 X_STEP_PIN=INVERT_X_STEP_PIN; //脉冲脚初始化为false
   disable_x();	//关闭X轴步进电机使能
  
	 //初始化Y轴步进电机驱动的引脚
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;		
	 GPIO_Init(GPIOF, &GPIO_InitStructure);	
	 Y_STEP_PIN=INVERT_Y_STEP_PIN; //脉冲脚初始化为false
	 disable_y();	//关闭Y轴步进电机使能

   //初始化Z轴步进电机驱动的引脚
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;			
	 GPIO_Init(GPIOF, &GPIO_InitStructure);
	 Z_STEP_PIN=INVERT_Z_STEP_PIN; //脉冲脚初始化为false
   disable_z();	//关闭Z轴步进电机使能	 

   //初始化E0轴步进电机驱动的引脚
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;			
	 GPIO_Init(GPIOF, &GPIO_InitStructure);
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;			
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
	  E0_STEP_PIN=INVERT_E_STEP_PIN; //脉冲脚初始化为false
   disable_e0();	//关闭e0轴步进电机使能	 
   
	 //初始化E1轴步进电机驱动的引脚
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;			
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
	 E1_STEP_PIN=INVERT_E_STEP_PIN; //脉冲脚初始化为false
   disable_e1();	//关闭e1轴步进电机使能	 


	//初始化限位开关引脚并将限位开关的初始状态设为false（高电平触发）
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTC时钟

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	//初始化限位开关X的引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  //初始化限位开关Y的引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//初始化限位开关Z的引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
 	GPIO_Init(GPIOC, &GPIO_InitStructure); 	 	  
  
	 
  //初始化定时器TIM4，
	//定时时钟频率为72M/（35+1）=2MHZ 
	//自动重装载寄存器周期的值为0x4000
	TIM4_Int_Init(0x4000,35);
 	
  ENABLE_STEPPER_DRIVER_INTERRUPT();  
  
  enable_endstops(1); //激活限位开关检测 // Start with endstops active. After homing they can be disabled
 // sei();开启全局中断，后面找一下在STM32中开启全局中断的函数来替代，似乎没有程序也可正常运行
}


// Block until all buffered steps are executed
void st_synchronize(void)
{
    while( blocks_queued()) {
    manage_heater();
    manage_inactivity();
		lcd_update();
//	  interface_update(); //屏幕界面更新
  }
}

void st_set_position(const long x, const long y, const long z, const long e)
{
  CRITICAL_SECTION_START;
  count_position[X_AXIS] = x;
  count_position[Y_AXIS] = y;
  count_position[Z_AXIS] = z;
  count_position[E_AXIS] = e;
  CRITICAL_SECTION_END;
}

void st_set_e_position(const long e)
{
  CRITICAL_SECTION_START;
  count_position[E_AXIS] = e;
  CRITICAL_SECTION_END;
}

long st_get_position(uint8_t axis)
{
  long count_pos;
  CRITICAL_SECTION_START;
  count_pos = count_position[axis];
  CRITICAL_SECTION_END;
  return count_pos;
}

void finishAndDisableSteppers(void)
{
  st_synchronize(); 
  disable_x(); 
  disable_y(); 
  disable_z(); 
  disable_e0(); 
  disable_e1(); 
}

void quickStop(void)
{
  DISABLE_STEPPER_DRIVER_INTERRUPT();
  while(blocks_queued())
    plan_discard_current_block();
  current_block = NULL;
  ENABLE_STEPPER_DRIVER_INTERRUPT();
}

//下面digipot_xxx()的函数都是和数字电位器（用来调节驱动IC的电流）配置相关的函数，现在步进电机驱动板上
//多用的是模拟电位器（滑动变阻器），因此下面digipot_xxx()函数并没用到，后面看下如何优化这些函数。
void digipot_init(void) //Initialize Digipot Motor Current //空函数
{
  /*
	const uint8_t digipot_motor_current[] = DIGIPOT_MOTOR_CURRENT;  
	int i;     
    for(i=0;i<=4;i++) 
      digipot_current(i,digipot_motor_current[i]);
	*/
}

void digipot_current(uint8_t driver, uint8_t current) //空函数
{
	/*
    const uint8_t digipot_ch[] = DIGIPOT_CHANNELS;
//	printf("%d:%d\r\n",digipot_ch[driver],current);
    digitalPotWrite(digipot_ch[driver], (uint8_t)current);
	*/
}

void digitalPotWrite(uint8_t address, uint8_t value) //空函数
{
/*
    DIGIPOTSS_PIN=1; // take the SS pin low to select the chip
    SPI1_ReadWriteByte(address); //  send in the address and value via SPI:
    SPI1_ReadWriteByte(value);
    DIGIPOTSS_PIN=0; // take the SS pin high to de-select the chip:
	*/
}

//从这一直到最后都是配置步进电机驱动（A4988/A4982）细分模式相关的函数，实际中可通过MS1、MS2、MS3
//短接来设置细分模式，这些函数并没用到因此这些函数的内容都可为空，后面看一些如何优化这部分内容
void microstep_init(void)  //空函数
{ 
	/*
	int i;
  for(i=0;i<=4;i++) microstep_mode(i,8);
	*/
}

void microstep_ms(uint8_t driver, int8_t ms1, int8_t ms2, int8_t ms3) //空函数
{
	/*
  if(ms1 > -1) switch(driver)
  {
    case 0:X_MS1_PIN=ms1 ; break;
    case 1:Y_MS1_PIN=ms1 ; break;
    case 2:Z_MS1_PIN=ms1 ; break;
    case 3:E0_MS1_PIN=ms1 ; break;
    case 4:E1_MS1_PIN=ms1 ; break;
	default:  break;
  }
  if(ms2 > -1) 
  switch(driver)
  {
    case 0:X_MS2_PIN=ms2 ; break;
    case 1:Y_MS2_PIN=ms2 ; break;
    case 2:Z_MS2_PIN=ms2 ; break;
    case 3:E0_MS2_PIN=ms2 ; break;
    case 4:E1_MS2_PIN=ms2 ; break;
	default:  break;
  }
    if(ms3 > -1) switch(driver)
  {
    case 0:X_MS3_PIN=ms3 ; break;
    case 1:Y_MS3_PIN=ms3 ; break;
    case 2:Z_MS3_PIN=ms3 ; break;
    case 3:E0_MS3_PIN=ms3 ; break;
    case 4:E1_MS3_PIN=ms3 ; break;
	default:  break;
  }
	*/
}

void microstep_mode(uint8_t driver, uint8_t stepping_mode) //空函数
{ 
	/*
	switch(driver)
  {
    case 0: subsection_x_value=stepping_mode; break;
    case 1: subsection_y_value=stepping_mode; break;
    case 2: subsection_z_value=stepping_mode; break;
    case 3: subsection_e0_value=stepping_mode; break;
    case 4: subsection_e1_value=stepping_mode; break;
	default:  break;
  }
  switch(stepping_mode)
  {
    case 1: microstep_ms(driver,MICROSTEP1); break;
    case 2: microstep_ms(driver,MICROSTEP2); break;
    case 4: microstep_ms(driver,MICROSTEP4); break;
    case 8: microstep_ms(driver,MICROSTEP8); break;
    case 16: microstep_ms(driver,MICROSTEP16); break;
    case 32: microstep_ms(driver,MICROSTEP32); break;
    case 64: microstep_ms(driver,MICROSTEP64); break;
    case 128: microstep_ms(driver,MICROSTEP128); break;
	default:  break;
  }
	*/
}
void microstep_readings(void)
{
	printf("Motor_Subsection \n");
	printf("X: %d\n",subsection_x_value);
	printf("Y: %d\n",subsection_y_value);
	printf("Z: %d\n",subsection_z_value);
	printf("E0: %d\n",subsection_e0_value);
	printf("E1: %d\n",subsection_e1_value);
}

