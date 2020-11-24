/*----------------------------------------------------------------------------------------*/
/* cfg_file.c                                                                             */
/* 项目描述:                                                                              */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
/* 2015/1/25         	   manwjh 建立 V0.1                                                 */
/*                                                                                        */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "ff.h"
#include "datetime.h"
#include "stdio.h"
#include "string.h"
#include "my_string.h"
#include "ilook_file.h"

#define GPX_PATH        "Gpx"
#define CFG_FILE_NAME   "time.txt"
#define LOG_FILE_NAME   "syslog.txt"
#define GPX_FILE_NAME   "yymmdd.gpx"
//
FIL cfgFIL;
//
extern const char prj_info[];
extern const char prj_version[];
extern tm *sys_time(void *p);

SYS_CFG_TYPE iLookCfg;

/*----------------------------------------------------------------------------------------*/
/*                                                                                        */
/*                                读取iLook.cfg配置文件                                    */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
int get_para(char *in_bf, char *out_bf)
{
	int i;
    char tmp;
    
	*out_bf = 0;

    //搜索=
    i=0;
    while(i++<100){
        tmp = *in_bf++;
        if( tmp== '=' )
            break;
    };
    
    //搜索第一个字母/数字/引号
    while(1){
        tmp = *in_bf++;
        if( ischar(tmp) )
            break;
    };
    
    //COPY字符到out_bf
    *out_bf++ = tmp;
    while(1){
        tmp = *in_bf++;
        if( ischar(tmp) )
            *out_bf++ = tmp;
        else{
            *out_bf = 0x0;
            break;
        }
    }

    return 1;
}

void ilook_cfg_load(void)
{
  char tmp_str[100];
  char n[10];
  int i_tmp;
  uint32_t t;

  //系统默认值
  iLookCfg.TimeZone = 8;
  iLookCfg.WaveType = 2;
  //
  iLookCfg.WeatherWin_onoff = 1;
  iLookCfg.DebugWin_onoff = 0;
  iLookCfg.TimerWin_onoff = 1;
  //
  iLookCfg.T_LogoDisplay = 2;
  iLookCfg.T_ScreenAutoCloseTime = 60;
  iLookCfg.T_GPSSearchTimeMax = 90;
  iLookCfg.T_WeatherInterval = 1;
  iLookCfg.T_WeatherWave = 10;
  //
  iLookCfg.g_slope_th = 35;         //0x18-0x03,
  iLookCfg.g_slope_dur = 0;
  iLookCfg.g_ig_incr_step = 20;
  iLookCfg.g_ig_dec_step = 1;
  iLookCfg.g_ig_wkup_level = 40;
  iLookCfg.g_ig_move_level = 300;
  iLookCfg.g_ig_max_cnt = 6000;
  iLookCfg.g_mmt_flt_scale = 5;
  iLookCfg.g_mmt_offset = 15;
  //
  iLookCfg.oled_contrast = 0x7F;    //oled对比度
  iLookCfg.oled_fosc = 0xa0;        //oled显示频率设定
  iLookCfg.flip_onoff = 0;
  //other
  if( f_open(&cfgFIL,CFG_FILE_NAME,FA_READ)==FR_OK ){
    while( 1 ){
      //读取CFG文件一行
      if( f_gets(tmp_str,100,&cfgFIL)==NULL )
        break;

      //系统配置
      if( strstr(tmp_str,"GPX_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.GPX_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"TimeZone") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<3 ){
          iLookCfg.TimeZone = DecStr2Int(n,2);
        }
      }else if( strstr(tmp_str,"GPS_PosConvert") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.GPS_PosConvert = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"AltitudeType") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.AltitudeType = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"GpxSaveCnt") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<4 ){
          iLookCfg.GpxSaveCnt = DecStr2Int(n,3);
        }
      }else if( strstr(tmp_str,"WaveType") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.WaveType = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"over") ){
        get_para(tmp_str,n);
        if( isdecstring(n)==8 ){
          t= DecStr2Int(n,8);
          iLookCfg.tCountDown.year = t/10000;
          t = t-iLookCfg.tCountDown.year*10000;
          //
          iLookCfg.tCountDown.month = t/100;
          t = t-iLookCfg.tCountDown.month*100;
          //
          iLookCfg.tCountDown.mday = t%100;
        }
      }
      
      //时间配置
      else if( strstr(tmp_str,"T_AutoClose") ){
        get_para(tmp_str,n);
        iLookCfg.T_ScreenAutoCloseTime = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_GPSSearchTimeMax")  ){
        get_para(tmp_str,n);
        iLookCfg.T_GPSSearchTimeMax = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_GPSSleepSec_Car") ){
        get_para(tmp_str,n);
        iLookCfg.T_GPSSleepSec_Car = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_GPSSleepSec_Walk") ){
        get_para(tmp_str,n);
        iLookCfg.T_GPSSleepSec_Walk = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_TravelRestMax") ){
        get_para(tmp_str,n);
        iLookCfg.T_TravelRestMax = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_WeatherInterval") ){
        get_para(tmp_str,n);
        iLookCfg.T_WeatherInterval = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_LogoDisplay") ){
        get_para(tmp_str,n);
        iLookCfg.T_LogoDisplay = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_ScreenCloseLongTime") ){
        get_para(tmp_str,n);
        iLookCfg.T_ScreenCloseLongTime = DecStr2Int(n,5);
      }else if( strstr(tmp_str,"T_WeatherWave") ){
        get_para(tmp_str,n);
        iLookCfg.T_WeatherWave = DecStr2Int(n,5);
      }
      //界面自定义
      else if( strstr(tmp_str,"DebugWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.DebugWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"GotoWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.GotoWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"WeatherWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.WeatherWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"PositionWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.PositionWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"ShakeCountGame_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.ShakeCountGame_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"WhenWhereWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.WhenWhereWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"TimerWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.TimerWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"NpsWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.NpsWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"AltitudeTempWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.AltitudeTempWin_onoff = DecStr2Int(n,1);
        }
      }else if( strstr(tmp_str,"TravelWin_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.TravelWin_onoff = DecStr2Int(n,1);
        }
      }
      //Gsensor设定
      else if( strstr(tmp_str,"g_slope_th") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,3);
        if( i_tmp<=255 ){
          iLookCfg.g_slope_th = i_tmp;
        }
      }else if( strstr(tmp_str,"g_slope_dur") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,3);
        if( i_tmp<=255 ){
          iLookCfg.g_slope_dur = i_tmp;
        }
      }else if( strstr(tmp_str,"g_ig_incr_step") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,3);
        if( i_tmp<=255 ){
          iLookCfg.g_ig_incr_step = i_tmp;
        }
      }else if( strstr(tmp_str,"g_ig_dec_strep") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,3);
        if( i_tmp<=255 ){
          iLookCfg.g_ig_dec_step = i_tmp;
        }
      }else if( strstr(tmp_str,"g_ig_wkup_level") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,5);
        if( i_tmp<=10000 ){
          iLookCfg.g_ig_wkup_level = i_tmp;
        }
      }else if( strstr(tmp_str,"g_ig_max_cnt") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,5);
        if( i_tmp<10000 ){
          iLookCfg.g_ig_max_cnt = i_tmp;
        }
      }else if( strstr(tmp_str,"g_ig_move_level") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,5);
        if( i_tmp<=10000 ){
          iLookCfg.g_ig_move_level = i_tmp;
        }
      }else if( strstr(tmp_str,"g_mmt_flt_scale") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,2);
        if( i_tmp<=99 ){
          iLookCfg.g_mmt_flt_scale = i_tmp;
        }
      }else if( strstr(tmp_str,"g_mmt_offset") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,2);
        if( i_tmp<=20 ){
          iLookCfg.g_mmt_offset = i_tmp;
        }
      }
      //OLED设定
      else if( strstr(tmp_str,"oled_contrast") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,3);
        if( i_tmp<=255 ){
          iLookCfg.oled_contrast = i_tmp;
        }
      }else if( strstr(tmp_str,"oled_fosc") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,3);
        if( i_tmp<=255 ){
          iLookCfg.oled_fosc = i_tmp;
        }
      }else if( strstr(tmp_str,"flip_onoff") ){
        get_para(tmp_str,n);
        if( isdecstring(n)<2 ){
          iLookCfg.flip_onoff = DecStr2Int(n,1);
        }
      }
      //OTHER设定
      else if( strstr(tmp_str,"gpx_min_distance") ){
        get_para(tmp_str,n);
        i_tmp = DecStr2Int(n,4);
        if( i_tmp<1000 ){
          iLookCfg.gpx_min_distance = i_tmp;
        }
      }
    }
  }
  //
  if( iLookCfg.T_ScreenAutoCloseTime>10*60 )
    iLookCfg.T_ScreenAutoCloseTime = 10*60;

  if( (iLookCfg.T_GPSSearchTimeMax<60)&&(iLookCfg.T_GPSSearchTimeMax>5*60) )
    iLookCfg.T_GPSSearchTimeMax=100; 
  
  if( iLookCfg.T_GPSSleepSec_Car>6*60 )
    iLookCfg.T_GPSSleepSec_Car=60;
  
  if( iLookCfg.T_GPSSleepSec_Car>10*60 )
    iLookCfg.T_GPSSleepSec_Walk = 60;
  
  if( (iLookCfg.T_TravelRestMax<10*60)&&(iLookCfg.T_TravelRestMax>8*60*60) )
    iLookCfg.T_TravelRestMax  = 60*60;
  
  if( (iLookCfg.T_WeatherInterval<1)&&(iLookCfg.T_WeatherInterval>60*60) )
    iLookCfg.T_WeatherInterval = 60;
    
  if( (iLookCfg.T_LogoDisplay<1)&&(iLookCfg.T_LogoDisplay>60) )
    iLookCfg.T_LogoDisplay = 2;
}



/*----------------------------------------------------------------------------------------*/
/*                                SYS LOG 文件处理                                        */
/*[12-12-2014 19:22] 内容标签: ... (错误类型码)                                            */ 
/*----------------------------------------------------------------------------------------*/
#if 1
void sys_log_write(char *event, char *msg)
{
}
#else
void sys_log_write(char *event, char *msg)
{
  FILINFO f_info;
  tm *t = sys_time(0x0);
  char str_time[30],ini_log=0;
  
  sprintf( str_time,"[%04d-%02d-%02d %02d:%02d:%02d]",t->year,t->month,t->mday,t->hour,t->min,t->sec );
  //检查LOG文件是否存在
  if( f_stat( LOG_FILE_NAME,&f_info )!= FR_OK ){
    if( f_create(&logFIL,LOG_FILE_NAME,0) == FR_OK ){
      ini_log = 1;
    }
  }
  //写入LOG信息
  if( f_open(&logFIL,LOG_FILE_NAME,FA_READ | FA_WRITE)==FR_OK ){
    f_lseek(&logFIL,logFIL.fsize);
    if( ini_log ){
      f_printf(&logFIL,"%s\r\n%s\r\n%s\r\n\r\n",
                          "[CarBetter LOG file]",
                          prj_info,
                          prj_version
                          );
    }
    f_printf(&logFIL,"%s%s : %s\r\n",str_time,event,msg);
    f_close(&logFIL);
  }
}
#endif

/*----------------------------------------------------------------------------------------*/
/*                                加载logo.bmp文件                                         */
/*                                                                                        */ 
/*----------------------------------------------------------------------------------------*/
typedef struct tagBITMAPFILEHEADER  {    
//  volatile uint16_t Type;     
  volatile uint32_t FlSize;  
  volatile uint32_t Reserved1;  
  volatile uint32_t bDataOffset;
  volatile uint32_t bHeaderSize;
  volatile uint32_t Width;
  volatile uint32_t Height;
  volatile uint16_t Planes;
  volatile uint16_t BitsPerPixel;
  volatile uint32_t Compression;
  volatile uint32_t bDataSize;
}BITMAPFILEHEADER;


uint8_t *load_logo(int *width, int *height)
{
  FIL logo_fp;
  static uint8_t buff[512];
  UINT br;
  
  BITMAPFILEHEADER *bf;
  
  //写入LOG信息
  if( f_open(&logo_fp,"logo.bmp",FA_READ)==FR_OK ){
    f_read(&logo_fp,buff,0x26,&br);
     bf = (BITMAPFILEHEADER*)(buff+2);
    if( (buff[0]==0x42)&&(buff[1]==0x4d)
      &&(bf->Reserved1==0)
      &&(bf->BitsPerPixel==1)
      &&(bf->bDataSize==512)
      &&(bf->Width==128)&&(bf->Height==32) ){
      //
      *width = 128;
      *height = 32;
      //
      f_lseek(&logo_fp,bf->bDataOffset);
      f_read(&logo_fp,buff,bf->bDataSize,&br);
      f_close(&logo_fp);
      return buff;
    }
  }
  
  f_close(&logo_fp);
  return 0;
}

