//焊台
//
//由ATmega328控制的Hakko T12烙铁头。
//
//此版本的代码实现：
//-尖端的温度测量
//-加热器的直接或PID控制
//-通过旋转编码器进行温度控制
//-短按旋转编码器开关即可进入升压模式
//-长按旋转编码器开关的设置菜单
//-手柄运动检测（通过检查振动开关）
//-铁未连接检测（通过识别无效的温度读数）
//-时间驱动的睡眠/断电模式（如果未使用烙铁的话）（运动检测）
//-测量输入电压，Vcc和ATmega的内部温度
//-OLED上的信息显示
//-蜂鸣器
//-校准和管理不同的焊接技巧
//-将用户设置存储到EEPROM
//-提示变化检测
//
//电源应在16V / 2A至24V / 3A的范围内稳定。
//
//为了进行校准，您需要一个烙铁头温度计。为了达到最佳效果
//打开焊台后至少等待三分钟
//开始校准过程。
//
//控制器：ATmega328p
//核心：Barebones ATmega（https://github.com/carlosefr/atmega）
//时钟速度：外部16 MHz
//
//建议不要使用引导加载程序！
//
// 2019/2020 by Stefan Wagner在John Glavinos的大力支持下
//项目页面：https：//easyeda.com/wagiminator
//许可证：http：//creativecommons.org/licenses/by-sa/3.0/



// Libraries
#include <U8glib.h>             // https://github.com/olikraus/u8glib
#include <PID_v1.h>             // https://github.com/mblythe86/C-PID-Library/tree/master/PID_v1
#include <EEPROM.h>             // 用于将用户设置存储到EEPROM中
#include <avr/sleep.h>          // 用于在ADC采样期间休眠

// 固件版本
#define VERSION       "v1.7"

// 旋转编码器类型
#define ROTARY_TYPE   1         // 0:2增量/步；1:4增量/步（默认）

// 引脚
#define SENSOR_PIN    A0        // 烙铁头温度检测
#define VIN_PIN       A1        // 输入电压检测
#define BUZZER_PIN     5        // 蜂鸣器
#define BUTTON_PIN     6        // 旋转编码器微动开关
#define ROTARY_1_PIN   7        // 旋转编码器 1脚
#define ROTARY_2_PIN   8        // 旋转编码器 2脚
#define CONTROL_PIN    9        // 加热器MOSFET PWM控制
#define SWITCH_PIN    10        // 手柄振动开关

// 默认温度控制值（建议焊接温度：300-380°C）
#define TEMP_MIN      150       // 最低可选温度
#define TEMP_MAX      400       // 最大可选温度
#define TEMP_DEFAULT  320       // 默认启动设定值
#define TEMP_SLEEP    150       // 睡眠模式下的温度
#define TEMP_BOOST     50       // 增强模式下的温度升高值
#define TEMP_STEP      10       // 旋转编码器温度变化步进

// 默认烙铁头温度校准值
#define TEMP200       216       // ADC = 200时的温度
#define TEMP280       308       // ADC = 280时的温度
#define TEMP360       390       // ADC = 360时的温度
#define TEMPCHP       30        // 校准时芯片温度
#define TIPMAX        8         // 最大烙铁数
#define TIPNAMELENGTH 6         // 烙铁名称的最大长度（包括结束符）
#define TIPNAME       "BC1.5"   // 默认烙铁名称

// 默认计时器值（0=禁用）
#define TIME2SLEEP     5        // 进入睡眠模式的时间（分钟）
#define TIME2OFF      15        // 关闭加热器的时间（分钟）
#define TIMEOFBOOST   40        // 保持在增强模式的时间（秒）

// 控制值
#define TIME2SETTLE   950       // 允许运算放大器（OpAmp）输出等待的时间（以微秒为单位），检测温度时要断开烙铁供电
#define SMOOTHIE      0.05      // OpAmp输出平滑因子（1=无平滑；默认值为0.05）
#define PID_ENABLE    false     // 启用PID控制
#define BEEP_ENABLE   true      // 启用/禁用蜂鸣器
#define MAINSCREEN    0         // 主屏幕类型（0:大数字；1:更多信息）

// EEPROM标识符
#define EEPROM_IDENT   0xE76C   // 确定EEPROM是否在之前由本程序擦写过

// 定义积极和保守的PID整定参数
double aggKp=11, aggKi=0.5, aggKd=1;
double consKp=11, consKi=3, consKd=5;

// 用户可以更改并存储在EEPROM中的默认值
uint16_t  DefaultTemp = TEMP_DEFAULT;
uint16_t  SleepTemp   = TEMP_SLEEP;
uint8_t   BoostTemp   = TEMP_BOOST;
uint8_t   time2sleep  = TIME2SLEEP;
uint8_t   time2off    = TIME2OFF;
uint8_t   timeOfBoost = TIMEOFBOOST;
uint8_t   MainScrType = MAINSCREEN;
bool      PIDenable   = PID_ENABLE;
bool      beepEnable  = BEEP_ENABLE;

// 烙铁的默认值
uint16_t  CalTemp[TIPMAX][4] = {TEMP200, TEMP280, TEMP360, TEMPCHP};
char      TipName[TIPMAX][TIPNAMELENGTH] = {TIPNAME};
uint8_t   CurrentTip   = 0;
uint8_t   NumberOfTips = 1;

// 菜单项
const char *SetupItems[]       = { "Setup Menu", "Tip Settings", "Temp Settings",
                                   "Timer Settings", "Control Type", "Main Screen",
                                   "Buzzer", "Information", "Return" };
const char *TipItems[]         = { "Tip:", "Change Tip", "Calibrate Tip", 
                                   "Rename Tip", "Delete Tip", "Add new Tip", "Return" };
const char *TempItems[]        = { "Temp Settings", "Default Temp", "Sleep Temp", 
                                   "Boost Temp", "Return" };
const char *TimerItems[]       = { "Timer Settings", "Sleep Timer", "Off Timer", 
                                   "Boost Timer", "Return" };
const char *ControlTypeItems[] = { "Control Type", "Direct", "PID" };
const char *MainScreenItems[]  = { "Main Screen", "Big Numbers", "More Infos" };
const char *StoreItems[]       = { "Store Settings ?", "No", "Yes" };
const char *SureItems[]        = { "Are you sure ?", "No", "Yes" };
const char *BuzzerItems[]      = { "Buzzer", "Disable", "Enable" };
const char *DefaultTempItems[] = { "Default Temp", "deg C" };
const char *SleepTempItems[]   = { "Sleep Temp", "deg C" };
const char *BoostTempItems[]   = { "Boost Temp", "deg C" };
const char *SleepTimerItems[]  = { "Sleep Timer", "Minutes" };
const char *OffTimerItems[]    = { "Off Timer", "Minutes" };
const char *BoostTimerItems[]  = { "Boost Timer", "Seconds" };
const char *DeleteMessage[]    = { "Warning", "You cannot", "delete your", "last tip!" };
const char *MaxTipMessage[]    = { "Warning", "You reached", "maximum number", "of tips!" };

// 引脚状态变化中断变量
volatile uint8_t  a0, b0, c0, d0;
volatile bool     ab0;
volatile int      count, countMin, countMax, countStep;
volatile bool     handleMoved;
 
// 温度控制变量
uint16_t  SetTemp, ShowTemp, gap, Step;
double    Input, Output, Setpoint, RawTemp, CurrentTemp, ChipTemp;

// 电压读数变量
uint16_t  Vcc, Vin;
 
// 状态变量
bool      inSleepMode = false;
bool      inOffMode   = false;
bool      inBoostMode = false;
bool      inCalibMode = false;
bool      isWorky     = true;
bool      beepIfWorky = true;
bool      TipIsPresent= true;

// 定时变量
uint32_t  sleepmillis;
uint32_t  boostmillis;
uint32_t  buttonmillis;
uint8_t   goneMinutes;
uint8_t   goneSeconds;
uint8_t   SensorCounter = 255;

// 指定变量指针和初始PID整定参数
PID ctrl(&Input, &Output, &Setpoint, aggKp, aggKi, aggKd, REVERSE);
 
// Setup u8g对象：根据使用的OLED取消注释
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_FAST|U8G_I2C_OPT_NO_ACK);



void setup() { 
  // 设置引脚模式
  pinMode(SENSOR_PIN,   INPUT);
  pinMode(VIN_PIN,      INPUT);
  pinMode(BUZZER_PIN,   OUTPUT);
  pinMode(CONTROL_PIN,  OUTPUT);
  pinMode(ROTARY_1_PIN, INPUT_PULLUP);
  pinMode(ROTARY_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN,   INPUT_PULLUP);
  pinMode(SWITCH_PIN,   INPUT_PULLUP);
  
  analogWrite(CONTROL_PIN, 255);        // 这将会关闭加热器，500Hz左右
  digitalWrite(BUZZER_PIN, LOW);        // 蜂鸣器不使用时必须低电平

  // setup ADC
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);  // 将ADC预分频器设置为128
  ADCSRA |= bit (ADIE);                 // 使能ADC中断
  interrupts ();                        // 启用全局中断

  // 旋转编码器的设置引脚状态变化中断
  PCMSK0 = bit (PCINT0);                // 在Pin8上配置引脚状态变化中断
  PCICR  = bit (PCIE0);                 // 启用引脚状态变化中断
  PCIFR  = bit (PCIF0);                 // 清除中断标志位

  // 开启 OLED
  if      ( u8g.getMode() == U8G_MODE_R3G3B2 )   u8g.setColorIndex(255);
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) u8g.setColorIndex(3);
  else if ( u8g.getMode() == U8G_MODE_BW )       u8g.setColorIndex(1);
  else if ( u8g.getMode() == U8G_MODE_HICOLOR )  u8g.setHiColorByRGB(255,255,255);

  // 从EEPROM中获取默认值
  getEEPROM();

  // 读取电源电压（mV）
  Vcc = getVCC(); Vin = getVIN();

  // 读取并设置当前烙铁温度
  SetTemp  = DefaultTemp;
  RawTemp  = denoiseAnalog(SENSOR_PIN);//原始温度
  ChipTemp = getChipTemp();//芯片温度
  calculateTemp();//计算烙铁当前的温度

  // 如果烙铁温度远低于设定值，打开加热器
  if ((CurrentTemp + 20) < DefaultTemp) analogWrite(CONTROL_PIN, 0);

  // 设置PID输出范围并启动PID
  ctrl.SetOutputLimits(0, 255);
  ctrl.SetMode(AUTOMATIC);

  // 设置初始旋转编码器值
  a0 = PINB & 1; b0 = PIND>>7 & 1; ab0 = (a0 == b0);
  setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, DefaultTemp);
  
  // 重置睡眠计时器
  sleepmillis = millis();

  // 设置完成时发出长嘟嘟声
  beep(); beep();
}
 

void loop() {
  ROTARYCheck();      // 检查旋转编码器（温度/增压设置，进入设置菜单）
  SLEEPCheck();       // 检查并激活/停用睡眠模式
  SENSORCheck();      // 读取烙铁的温度和振动开关
  Thermostat();       // 温度控制
  MainScreen();       // 更新home界面
}




// 检查旋转编码器；设置温度，切换增强模式，相应进入设置菜单
void ROTARYCheck() {
  // 根据旋转编码器值设定工作温度
  SetTemp = getRotary();
  
  // 检查旋转编码器按键
  uint8_t c = digitalRead(BUTTON_PIN);
  if ( !c && c0 ) {//按键按下
    beep();
    buttonmillis = millis();
    while( (!digitalRead(BUTTON_PIN)) && ((millis() - buttonmillis) < 500) );
    if ((millis() - buttonmillis) >= 500) SetupScreen();
    else {
      inBoostMode = !inBoostMode;
      if (inBoostMode) boostmillis = millis();
      handleMoved = true;
    }
  }
  c0 = c;

  // 在升压模式下检查定时器
  if (inBoostMode && timeOfBoost) {
    goneSeconds = (millis() - boostmillis) / 1000;
    if (goneSeconds >= timeOfBoost) {
      inBoostMode = false;              // 停止增压模式
      beep();                           // 增压模式结束时发出蜂鸣音
      beepIfWorky = true;               // 达到工作温度时再次发出蜂鸣音
    }
  }
}


// 检查并激活/停用睡眠模式
void SLEEPCheck() {
  if (handleMoved) {                    // 如果手柄被移动
    if (inSleepMode) {                  // 处于睡眠模式与否？
      if ((CurrentTemp + 20) < SetTemp) // 如果温度远低于设定值
        analogWrite(CONTROL_PIN, 0);    // 那现在就启动加热器
      beep();                           // 唤醒时发出哔声
      beepIfWorky = true;               // 达到工作温度时再次发出蜂鸣音
    }
    handleMoved = false;                // 重置手柄移动标志
    inSleepMode = false;                // 重置睡眠标志
    inOffMode   = false;                // 重置关闭模式标志
    sleepmillis = millis();             // 重置睡眠计时
  }

  // 检查自移动手柄以来经过的时间
  goneMinutes = (millis() - sleepmillis) / 60000;
  if ( (!inSleepMode) && (time2sleep > 0) && (goneMinutes >= time2sleep) ) {inSleepMode = true; beep();}
  if ( (!inOffMode)   && (time2off   > 0) && (goneMinutes >= time2off  ) ) {inOffMode   = true; beep();}
}


//读取温度、振动开关和电源电压
void SENSORCheck() {
  analogWrite(CONTROL_PIN, 255);              // 关闭加热器以测量温度
  delayMicroseconds(TIME2SETTLE);             // 等待电压稳定下来
  
  double temp = denoiseAnalog(SENSOR_PIN);    // 读取温度的ADC值
  uint8_t d = digitalRead(SWITCH_PIN);        // 检查手柄振动开关
  if (d != d0) {handleMoved = true; d0 = d;}  // 移动手柄时设置标志
  if (! SensorCounter--) Vin = getVIN();      // 时而获取输入电压
  
  analogWrite(CONTROL_PIN, Output);           // 再次打开加热器
  
  RawTemp += (temp - RawTemp) * SMOOTHIE;     // 稳定ADC温度读数
  calculateTemp();                            // 计算实际温度值

  // 在设定点附近稳定显示温度
  if ((ShowTemp != Setpoint) || (abs(ShowTemp - CurrentTemp) > 5)) ShowTemp = CurrentTemp;
  if (abs(ShowTemp - Setpoint) <= 1) ShowTemp = Setpoint;

  // 如果温度在工作范围内，则设置状态变量；如果刚刚达到工作温度，则发出哔声
  gap = abs(SetTemp - CurrentTemp);
  if (gap < 5) {
    if (!isWorky && beepIfWorky) beep();
    isWorky = true;
    beepIfWorky = false;
  }
  else isWorky = false;

  // 检查烙铁是否存在或当前已插入
  if (ShowTemp > 500) TipIsPresent = false;   // 烙铁是否已拔出 ?
  if (!TipIsPresent && (ShowTemp < 500)) {    // 新的烙铁是否已连接 ?
    analogWrite(CONTROL_PIN, 255);            // 关闭加热器
    beep();                                   // 哔哔声提示信息
    TipIsPresent = true;                      // 现在烙铁已连接
    ChangeTipScreen();                        // 显示烙铁选择屏幕
    updateEEPROM();                           // 更新EEPROM中的设置
    handleMoved = true;                       // 重置所有计时器
    RawTemp  = denoiseAnalog(SENSOR_PIN);     // 重新启动温度平滑算法
    c0 = LOW;                                 // 旋转编码器开关状态置零
    setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, SetTemp);  // 重置旋转编码器
  }
}


// 根据ADC读数和校准值计算实际温度值
void calculateTemp() {
  if      (RawTemp < 200) CurrentTemp = map (RawTemp,   0, 200,                     21, CalTemp[CurrentTip][0]);
  else if (RawTemp < 280) CurrentTemp = map (RawTemp, 200, 280, CalTemp[CurrentTip][0], CalTemp[CurrentTip][1]);
  else                    CurrentTemp = map (RawTemp, 280, 360, CalTemp[CurrentTip][1], CalTemp[CurrentTip][2]);
}


// 控制加热器
void Thermostat() {
  // 根据当前工作模式定义设定值
  if      (inOffMode)   Setpoint = 0;//关机模式
  else if (inSleepMode) Setpoint = SleepTemp;//睡眠模式
  else if (inBoostMode) Setpoint = SetTemp + BoostTemp;//增强模式
  else                  Setpoint = SetTemp; //标准模式

  // 控制加热器（PID或直接控制）
  gap = abs(Setpoint - CurrentTemp);//温差
  if (PIDenable) {
    Input = CurrentTemp;
    if (gap < 30) ctrl.SetTunings(consKp, consKi, consKd);//使用保守的PID参数
    else ctrl.SetTunings(aggKp, aggKi, aggKd); //使用激进的PID参数
    ctrl.Compute();
  } else {
    // 如果当前温度低于设定值，打开加热器
    if ((CurrentTemp + 0.5) < Setpoint) Output = 0; //全速加热
	else Output = 255;//停止加热
  }
  analogWrite(CONTROL_PIN, Output);     // 设置温度PWM
}


// 在蜂鸣器上发出短哔声
void beep(){
  if (beepEnable) {
    for (uint8_t i=0; i<255; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(125);
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(125);
    }
  }
}


// 设置旋转编码器的起始值
void setRotary(int rmin, int rmax, int rstep, int rvalue) {
  countMin  = rmin << ROTARY_TYPE;
  countMax  = rmax << ROTARY_TYPE;
  countStep = rstep;
  count     = rvalue << ROTARY_TYPE;  
}


// 读取当前旋转编码器值
int getRotary() {
  return (count >> ROTARY_TYPE);
}


// 从EEPROM读取用户设置；如果EEPROM值无效，则写入默认值
void getEEPROM() {
  uint16_t identifier = (EEPROM.read(0) << 8) | EEPROM.read(1);
  if (identifier == EEPROM_IDENT) {
    DefaultTemp = (EEPROM.read(2) << 8) | EEPROM.read(3);
    SleepTemp   = (EEPROM.read(4) << 8) | EEPROM.read(5);
    BoostTemp   =  EEPROM.read(6);
    time2sleep  =  EEPROM.read(7);
    time2off    =  EEPROM.read(8);
    timeOfBoost =  EEPROM.read(9);
    MainScrType =  EEPROM.read(10);
    PIDenable   =  EEPROM.read(11);
    beepEnable  =  EEPROM.read(12);
    CurrentTip  =  EEPROM.read(13);
    NumberOfTips = EEPROM.read(14);

    uint8_t i, j;
    uint16_t counter = 15;
    for (i = 0; i < NumberOfTips; i++) {
      for (j = 0; j < TIPNAMELENGTH; j++) {
        TipName[i][j] = EEPROM.read(counter++);
      }
      for (j = 0; j < 4; j++) {
        CalTemp[i][j]  = EEPROM.read(counter++) << 8;
        CalTemp[i][j] |= EEPROM.read(counter++);
      }
    }
  }
  else {
    EEPROM.update(0, EEPROM_IDENT >> 8); EEPROM.update(1, EEPROM_IDENT & 0xFF);
    updateEEPROM();
  }
}


// writes user settings to EEPROM using updade function to minimize write cycles
void updateEEPROM() {
  EEPROM.update( 2, DefaultTemp >> 8);
  EEPROM.update( 3, DefaultTemp & 0xFF);
  EEPROM.update( 4, SleepTemp >> 8);
  EEPROM.update( 5, SleepTemp & 0xFF);
  EEPROM.update( 6, BoostTemp);
  EEPROM.update( 7, time2sleep);
  EEPROM.update( 8, time2off);
  EEPROM.update( 9, timeOfBoost);
  EEPROM.update(10, MainScrType);
  EEPROM.update(11, PIDenable);
  EEPROM.update(12, beepEnable);
  EEPROM.update(13, CurrentTip);
  EEPROM.update(14, NumberOfTips);

  uint8_t i, j;
  uint16_t counter = 15;
  for (i = 0; i < NumberOfTips; i++) {
    for (j = 0; j < TIPNAMELENGTH; j++) EEPROM.update(counter++, TipName[i][j]);
    for (j = 0; j < 4; j++) {
      EEPROM.update(counter++, CalTemp[i][j] >> 8);
      EEPROM.update(counter++, CalTemp[i][j] & 0xFF);
    }
  }
}


// draws the main screen
void MainScreen() {
  u8g.firstPage();
  do {
    // draw setpoint temperature
    u8g.setFont(u8g_font_9x15);
    u8g.setFontPosTop();
    u8g.drawStr( 0, 0,  "SET:");
    u8g.setPrintPos(40,0);
    u8g.print(Setpoint, 0);

    // draw status of heater
    u8g.setPrintPos(83,0);
    if (ShowTemp > 500)    u8g.print(F("ERROR"));
    else if (inOffMode)    u8g.print(F("  OFF"));
    else if (inSleepMode)  u8g.print(F("SLEEP"));
    else if (inBoostMode)  u8g.print(F("BOOST"));
    else if (isWorky)      u8g.print(F("WORKY"));
    else if (Output < 180) u8g.print(F(" HEAT"));
    else                   u8g.print(F(" HOLD"));

    // rest depending on main screen type
    if (MainScrType) {
      // draw current tip and input voltage
      float fVin = (float)Vin / 1000;     // convert mv in V
      u8g.setPrintPos( 0,52); u8g.print(TipName[CurrentTip]);
      u8g.setPrintPos(83,52); u8g.print(fVin, 1); u8g.print(F("V"));
      // draw current temperature
      u8g.setFont(u8g_font_freedoomr25n);
      u8g.setFontPosTop();
      u8g.setPrintPos(37,22);
      if (ShowTemp > 500) u8g.print(F("000")); else u8g.print(ShowTemp);
    } else {
      // draw current temperature in big figures
      u8g.setFont(u8g_font_fub42n);
      u8g.setFontPosTop();
      u8g.setPrintPos(15,20);
      if (ShowTemp > 500) u8g.print(F("000")); else u8g.print(ShowTemp);
    }
  } while(u8g.nextPage());
}


// setup screen
void SetupScreen() {
  analogWrite(CONTROL_PIN, 255);      // shut off heater
  beep();
  uint16_t SaveSetTemp = SetTemp;
  uint8_t selection = 0;
  bool repeat = true;
  
  while (repeat) {
    selection = MenuScreen(SetupItems, sizeof(SetupItems), selection);
    switch (selection) {
      case 0:   TipScreen(); repeat = false; break;
      case 1:   TempScreen(); break;
      case 2:   TimerScreen(); break;
      case 3:   PIDenable = MenuScreen(ControlTypeItems, sizeof(ControlTypeItems), PIDenable); break;
      case 4:   MainScrType = MenuScreen(MainScreenItems, sizeof(MainScreenItems), MainScrType); break;
      case 5:   beepEnable = MenuScreen(BuzzerItems, sizeof(BuzzerItems), beepEnable); break;
      case 6:   InfoScreen(); break;
      default:  repeat = false; break;
    }
  }  
  updateEEPROM();
  handleMoved = true;
  SetTemp = SaveSetTemp;
  setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, SetTemp);
}


// tip settings screen
void TipScreen() {
  uint8_t selection = 0;
  bool repeat = true;  
  while (repeat) {
    selection = MenuScreen(TipItems, sizeof(TipItems), selection);
    switch (selection) {
      case 0:   ChangeTipScreen();   break;
      case 1:   CalibrationScreen(); break;
      case 2:   InputNameScreen();   break;
      case 3:   DeleteTipScreen();   break;
      case 4:   AddTipScreen();      break;
      default:  repeat = false;      break;
    }
  }
}


// temperature settings screen
void TempScreen() {
  uint8_t selection = 0;
  bool repeat = true;  
  while (repeat) {
    selection = MenuScreen(TempItems, sizeof(TempItems), selection);
    switch (selection) {
      case 0:   setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, DefaultTemp);
                DefaultTemp = InputScreen(DefaultTempItems); break;
      case 1:   setRotary(20, 200, TEMP_STEP, SleepTemp);
                SleepTemp = InputScreen(SleepTempItems); break;
      case 2:   setRotary(10, 100, TEMP_STEP, BoostTemp);
                BoostTemp = InputScreen(BoostTempItems); break;
      default:  repeat = false; break;
    }
  }
}


// timer settings screen
void TimerScreen() {
  uint8_t selection = 0;
  bool repeat = true;  
  while (repeat) {
    selection = MenuScreen(TimerItems, sizeof(TimerItems), selection);
    switch (selection) {
      case 0:   setRotary(0, 30, 1, time2sleep);
                time2sleep = InputScreen(SleepTimerItems); break;
      case 1:   setRotary(0, 60, 5, time2off);
                time2off = InputScreen(OffTimerItems); break;
      case 2:   setRotary(0, 180, 10, timeOfBoost);
                timeOfBoost = InputScreen(BoostTimerItems); break;
      default:  repeat = false; break;
    }
  }
}


// menu screen
uint8_t MenuScreen(const char *Items[], uint8_t numberOfItems, uint8_t selected) {
  bool isTipScreen = (Items[0] == "Tip:");
  uint8_t lastselected = selected;
  int8_t  arrow = 0;
  if (selected) arrow = 1;
  numberOfItems >>= 1;
  setRotary(0, numberOfItems - 2, 1, selected);
  bool    lastbutton = (!digitalRead(BUTTON_PIN));

  do {
    selected = getRotary();
    arrow = constrain(arrow + selected - lastselected, 0, 2);
    lastselected = selected;
    u8g.firstPage();
      do {
        u8g.setFont(u8g_font_9x15);
        u8g.setFontPosTop();
        u8g.drawStr( 0, 0,  Items[0]);
        if (isTipScreen) u8g.drawStr( 54, 0,  TipName[CurrentTip]);
        u8g.drawStr( 0, 16 * (arrow + 1), ">");
        for (uint8_t i=0; i<3; i++) {
          uint8_t drawnumber = selected + i + 1 - arrow;
          if (drawnumber < numberOfItems)
            u8g.drawStr( 12, 16 * (i + 1), Items[selected + i + 1 - arrow]);
        }
      } while(u8g.nextPage());
    if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
  } while (digitalRead(BUTTON_PIN) || lastbutton);

  beep();
  return selected;
}


void MessageScreen(const char *Items[], uint8_t numberOfItems) {
  bool lastbutton = (!digitalRead(BUTTON_PIN));
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_9x15);
    u8g.setFontPosTop();
    for (uint8_t i = 0; i < numberOfItems; i++) u8g.drawStr( 0, i * 16,  Items[i]);
  } while(u8g.nextPage());
  do {
    if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
  } while (digitalRead(BUTTON_PIN) || lastbutton);
  beep();
}


// input value screen
uint16_t InputScreen(const char *Items[]) {
  uint16_t  value;
  bool      lastbutton = (!digitalRead(BUTTON_PIN));

  do {
    value = getRotary();
    u8g.firstPage();
      do {
        u8g.setFont(u8g_font_9x15);
        u8g.setFontPosTop();
        u8g.drawStr( 0, 0,  Items[0]);
        u8g.setPrintPos(0, 32); u8g.print(">"); u8g.setPrintPos(10, 32);        
        if (value == 0)  u8g.print(F("Deactivated"));
        else            {u8g.print(value);u8g.print(" ");u8g.print(Items[1]);}
      } while(u8g.nextPage());
    if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
  } while (digitalRead(BUTTON_PIN) || lastbutton);

  beep();
  return value;
}


// information display screen
void InfoScreen() {
  bool lastbutton = (!digitalRead(BUTTON_PIN));

  do {
    Vcc = getVCC();                     // read input voltage
    float fVcc = (float)Vcc / 1000;     // convert mV in V
    Vin = getVIN();                     // read supply voltage
    float fVin = (float)Vin / 1000;     // convert mv in V
    float fTmp = getChipTemp();         // read cold junction temperature
    u8g.firstPage();
      do {
        u8g.setFont(u8g_font_9x15);
        u8g.setFontPosTop();
        u8g.setPrintPos(0,  0); u8g.print(F("Firmware: ")); u8g.print(VERSION);
        u8g.setPrintPos(0, 16); u8g.print(F("Tmp: "));  u8g.print(fTmp, 1); u8g.print(F(" C"));
        u8g.setPrintPos(0, 32); u8g.print(F("Vin: "));  u8g.print(fVin, 1); u8g.print(F(" V"));
        u8g.setPrintPos(0, 48); u8g.print(F("Vcc:  ")); u8g.print(fVcc, 1); u8g.print(F(" V"));
      } while(u8g.nextPage());
    if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
  } while (digitalRead(BUTTON_PIN) || lastbutton);

  beep();
}


// change tip screen
void ChangeTipScreen() {
  uint8_t selected = CurrentTip;
  uint8_t lastselected = selected;
  int8_t  arrow = 0;
  if (selected) arrow = 1;
  setRotary(0, NumberOfTips - 1, 1, selected);
  bool    lastbutton = (!digitalRead(BUTTON_PIN));

  do {
    selected = getRotary();
    arrow = constrain(arrow + selected - lastselected, 0, 2);
    lastselected = selected;
    u8g.firstPage();
      do {
        u8g.setFont(u8g_font_9x15);
        u8g.setFontPosTop();
        u8g.drawStr( 0, 0,  F("Select Tip"));
        u8g.drawStr( 0, 16 * (arrow + 1), ">");
        for (uint8_t i=0; i<3; i++) {
          uint8_t drawnumber = selected + i - arrow;
          if (drawnumber < NumberOfTips)
            u8g.drawStr( 12, 16 * (i + 1), TipName[selected + i - arrow]);
        }
      } while(u8g.nextPage());
    if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
  } while (digitalRead(BUTTON_PIN) || lastbutton);

  beep();
  CurrentTip = selected;
}


// temperature calibration screen
void CalibrationScreen() {
  uint16_t CalTempNew[4]; 
  for (uint8_t CalStep = 0; CalStep < 3; CalStep++) {
    SetTemp = CalTemp[CurrentTip][CalStep];
    setRotary(100, 500, 1, SetTemp);
    beepIfWorky = true;
    bool    lastbutton = (!digitalRead(BUTTON_PIN));

    do {
      SENSORCheck();      // reads temperature and vibration switch of the iron
      Thermostat();       // heater control
      
      u8g.firstPage();
      do {
        u8g.setFont(u8g_font_9x15);
        u8g.setFontPosTop();
        u8g.drawStr( 0, 0,  F("Calibration"));
        u8g.setPrintPos(0, 16); u8g.print(F("Step: ")); u8g.print(CalStep + 1); u8g.print(" of 3");
        if (isWorky) {
          u8g.setPrintPos(0, 32); u8g.print(F("Set measured"));
          u8g.setPrintPos(0, 48); u8g.print(F("temp: ")); u8g.print(getRotary());
        } else {
          u8g.setPrintPos(0, 32); u8g.print(F("ADC:  ")); u8g.print(uint16_t(RawTemp));
          u8g.setPrintPos(0, 48); u8g.print(F("Please wait..."));
        }
      } while(u8g.nextPage());
    if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
    } while (digitalRead(BUTTON_PIN) || lastbutton);

  CalTempNew[CalStep] = getRotary();
  beep(); delay (10);
  }

  analogWrite(CONTROL_PIN, 255);              // shut off heater
  delayMicroseconds(TIME2SETTLE);             // wait for voltage to settle
  CalTempNew[3] = getChipTemp();              // read chip temperature
  if ((CalTempNew[0] + 10 < CalTempNew[1]) && (CalTempNew[1] + 10 < CalTempNew[2])) {
    if (MenuScreen(StoreItems, sizeof(StoreItems), 0)) {
      for (uint8_t i = 0; i < 4; i++) CalTemp[CurrentTip][i] = CalTempNew[i];
    }
  }
}


// input tip name screen
void InputNameScreen() {
  uint8_t  value;

  for (uint8_t digit = 0; digit < (TIPNAMELENGTH - 1); digit++) {
    bool      lastbutton = (!digitalRead(BUTTON_PIN));
    setRotary(31, 96, 1, 65);
    do {
      value = getRotary();
      if (value == 31) {value = 95; setRotary(31, 96, 1, 95);}
      if (value == 96) {value = 32; setRotary(31, 96, 1, 32);}
      u8g.firstPage();
        do {
          u8g.setFont(u8g_font_9x15);
          u8g.setFontPosTop();
          u8g.drawStr( 0, 0,  F("Enter Tip Name"));
          u8g.setPrintPos(9 * digit, 48); u8g.print(char(94));
          u8g.setPrintPos(0, 32);
          for (uint8_t i = 0; i < digit; i++) u8g.print(TipName[CurrentTip][i]);
          u8g.setPrintPos(9 * digit, 32); u8g.print(char(value));
        } while(u8g.nextPage());
      if (lastbutton && digitalRead(BUTTON_PIN)) {delay(10); lastbutton = false;}
    } while (digitalRead(BUTTON_PIN) || lastbutton);
    TipName[CurrentTip][digit] = value;
    beep(); delay (10);
  }
  TipName[CurrentTip][TIPNAMELENGTH - 1] = 0;
  return value;
}


// delete tip screen
void DeleteTipScreen() {
  if (NumberOfTips == 1) {MessageScreen(DeleteMessage, sizeof(DeleteMessage));}
  else if (MenuScreen(SureItems, sizeof(SureItems), 0)) {
    if (CurrentTip == (NumberOfTips - 1)) {CurrentTip--;}
    else {
      for (uint8_t i = CurrentTip; i < (NumberOfTips - 1); i++) {
        for (uint8_t j = 0; j < TIPNAMELENGTH; j++) TipName[i][j] = TipName[i+1][j];
        for (uint8_t j = 0; j < 4; j++)             CalTemp[i][j] = CalTemp[i+1][j];
      }
    }
    NumberOfTips--;
  }
}


// add new tip screen
void AddTipScreen() {
  if (NumberOfTips < TIPMAX) {
    CurrentTip = NumberOfTips++; InputNameScreen();
    CalTemp[CurrentTip][0] = TEMP200; CalTemp[CurrentTip][1] = TEMP280;
    CalTemp[CurrentTip][2] = TEMP360; CalTemp[CurrentTip][3] = TEMPCHP;
  } else MessageScreen(MaxTipMessage, sizeof(MaxTipMessage));
}


// average several ADC readings in sleep mode to denoise
uint16_t denoiseAnalog (byte port) {
  uint16_t result = 0;
  ADCSRA |= bit (ADEN) | bit (ADIF);    // enable ADC, turn off any pending interrupt
  if (port >= A0) port -= A0;           // set port and
  ADMUX = (0x0F & port) | bit(REFS0);   // reference to AVcc 
  set_sleep_mode (SLEEP_MODE_ADC);      // sleep during sample for noise reduction
  for (uint8_t i=0; i<32; i++) {        // get 32 readings
    sleep_mode();                       // go to sleep while taking ADC sample
    while (bitRead(ADCSRA, ADSC));      // make sure sampling is completed
    result += ADC;                      // add them up
  }
  bitClear (ADCSRA, ADEN);              // disable ADC
  return (result >> 5);                 // devide by 32 and return value
}


// 根据1.1V参考电压读取ADC通道8，获取芯片内部温度
double getChipTemp() {
  uint16_t result = 0;
  ADCSRA |= bit (ADEN) | bit (ADIF);    // enable ADC, turn off any pending interrupt
  ADMUX = bit (REFS1) | bit (REFS0) | bit (MUX3); // set reference and mux
  delay(20);                            // wait for voltages to settle
  set_sleep_mode (SLEEP_MODE_ADC);      // sleep during sample for noise reduction
  for (uint8_t i=0; i<32; i++) {        // get 32 readings
    sleep_mode();                       // go to sleep while taking ADC sample
    while (bitRead(ADCSRA, ADSC));      // make sure sampling is completed
    result += ADC;                      // add them up
  }
  bitClear (ADCSRA, ADEN);              // disable ADC  
  result >>= 2;                         // devide by 4
  return ((result - 2594) / 9.76);      // calculate internal temperature in degrees C
}


// get input voltage in mV by reading 1.1V reference against AVcc
uint16_t getVCC() {
  uint16_t result = 0;
  ADCSRA |= bit (ADEN) | bit (ADIF);    // enable ADC, turn off any pending interrupt
  // set Vcc measurement against 1.1V reference
  ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
  delay(1);                             // wait for voltages to settle
  set_sleep_mode (SLEEP_MODE_ADC);      // sleep during sample for noise reduction
  for (uint8_t i=0; i<16; i++) {        // get 16 readings
    sleep_mode();                       // go to sleep while taking ADC sample
    while (bitRead(ADCSRA, ADSC));      // make sure sampling is completed
    result += ADC;                      // add them up
  }
  bitClear (ADCSRA, ADEN);              // disable ADC  
  result >>= 4;                         // devide by 16
  return (1125300L / result);           // 1125300 = 1.1 * 1023 * 1000 
}


// get supply voltage in mV
uint16_t getVIN() {
  long result;
  result = denoiseAnalog (VIN_PIN);     // read supply voltage via voltage divider
  return (result * Vcc / 179.474);      // 179.474 = 1023 * R13 / (R12 + R13)
}


// ADC interrupt service routine
EMPTY_INTERRUPT (ADC_vect);             // nothing to be done here


// Pin change interrupt service routine for rotary encoder
ISR (PCINT0_vect) {
  uint8_t a = PINB & 1;
  uint8_t b = PIND>>7 & 1;

  if (a != a0) {              // A changed
    a0 = a;
    if (b != b0) {            // B changed
      b0 = b;
      count = constrain(count + ((a == b) ? countStep : -countStep), countMin, countMax);
      if (ROTARY_TYPE && ((a == b) != ab0)) {
        count = constrain(count + ((a == b) ? countStep : -countStep), countMin, countMax);;
      }
      ab0 = (a == b);
      handleMoved = true;
    }
  }
}
