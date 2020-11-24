void sys_log_write(char *event, char *msg);
void gpx_record(char *gpx_msg);
void gpx_open(void);
void gpx_close(void);
uint8_t *load_logo(int *width, int *height);
void ilook_cfg_load(void);


typedef struct{
  //系统配置
  char GPX_onoff;             //GPX记录打开/关闭
  char TimeZone;              //时区
  char GPS_PosConvert;        //坐标转换
  char AltitudeType;          //海拔类型,=0 气压海拔，=1 GPS海拔，=2 综合海拔
  char GpxSaveCnt;            //GPX几个点快速存储
  char WaveType;              //=0海拔, =1温度，=2气压
  tm   tCountDown;            //倒计时器
  //界面加载管理
  char GotoWin_onoff;
  char WeatherWin_onoff;
  char PositionWin_onoff;
  char ShakeCountGame_onoff;
  char DebugWin_onoff;
  char WhenWhereWin_onoff;
  char TimerWin_onoff;
  char NpsWin_onoff;
  char AltitudeTempWin_onoff;
  char TravelWin_onoff;
  //时间设定
  int  T_LogoDisplay;         //LOGO显示时间
  int  T_ScreenAutoCloseTime; //sec,屏幕自动关闭时间
  int  T_GPSSearchTimeMax;    //sec,gps允许搜星最长时间
  int  T_GPSSleepSec_Car;     //在开车状态的GPS间歇开机时间
  int  T_GPSSleepSec_Walk;    //在步行状态的GPS间歇开机时间
  int  T_TravelRestMax;       //旅途最长休息时间
  int  T_WeatherInterval;     //天气采集间隔
  int  T_ScreenCloseLongTime; //一些特殊界面的长延时关屏
  int  T_WeatherWave;         //波形采集密度
  //GSENSOR设定
  unsigned char g_slope_th;
  unsigned char g_slope_dur;
  unsigned char g_ig_incr_step;
  unsigned char g_ig_dec_step;
  int g_ig_wkup_level;
  int g_ig_move_level;
  int g_ig_max_cnt;
  int g_mmt_flt_scale;
  int g_mmt_offset;
  //OLED亮度
  unsigned char oled_contrast;//OLED亮度
  unsigned char oled_fosc;    //OLED频率
  unsigned char flip_onoff;   //OLED反转
  //OTHER
  int gpx_min_distance;
  
}SYS_CFG_TYPE;






