#ifndef _PARA_H
#define _PARA_H

#ifndef _PARA_C
#define EXT_PARA extern 
#else
#define EXT_PARA
#endif

#define NUM_PARA        80
//参数变量定义
typedef union 
{	
  struct
  {
        UINT16 bak;
		UINT16 addr;			//本机地址
        
         u16 a_jiayao_time;      //A加药时间设定
        u16 b_jiayao_time;      //b加药时间设定           
        u16 jiayao_time_3;      //3号罐加药时间设定
		u16 jiayao_time_4;      //4号罐加药时间设定
        
        
        u16 one_h;               
        u16 one_l;               
        u16 two_h;              
        u16 two_l;
        u16 three_h;      
        u16 three_l;      
        
        u16 jianyao_value_3;    //3#计量泵每吨水加药量
        u16 jianyao_value_4;    //4#计量泵每吨水加药量
        u16 bak3;
        
        u16 pw_single_times;        //一次排污时长，S
        u16 pw_loop_min_times;      //最小排污周期,S
        
        
		UINT16 password;        //密码
        u16 jiankong_commpara_buf[4];
        u16 gprs_commpara_buf[4];
        

/***********10 WORK_PARA_OFFSET***************/
       
        
        u16 max_liuliang;//最大流量//28
        u16 max_ph;//最大ph值
        u16 max_orp;
        
        u16 fulx;               //流量设定，开始加药时的流量


		/***********18 OFFSET***************/
        u16 paiwu_time;         //排污时间间设定      
        u16 paiwu_gap_time;  //多长时间排一次污

		u16 orp_h;               
        u16 orp_l;               
        u16 ph_h;              
        u16 ph_l;
        u16 conductance_h;      
        u16 conductance_l; 
        
        /***********28系数偏移***************/      
        u16 orp_proportion;
        u16 fulx_proportion;
        u16 by2_proportion;
        u16 by3_proportion;
        u16 by_proportion;
        u16 water_proportion;
        
        /***********34***************/
        u16 auto_max_pulse;
        

        u16 max_temperature;
        u16 max_conductance;
        u16 cond_paiwu_h;//由于电导高开始排污的限值
        
        u16 auto_mode;
        
        u16 sensor_commpara_buf[5];
  }value;        
  u16 value_group[NUM_PARA];        
}PARA;

// EXT_PARA u16 temp_local_addr;
// EXT_PARA u16 temp_local_password;

EXT_PARA PARA para;
EXT_PARA PARA para_temp;

EXT_PARA u8 para_enable_flag;  //参数有效标志。如果重新传完程序后，参数会被覆盖掉，要提示操作人员重新设置参数。至少是地址。

EXT_PARA u8 change_para_type_flag;
EXT_PARA u8 comm_para_change_flag;

//以下用于在液晶操作定值与参数固化时，在参数修改过程中，将改过的参数保存到
//如下位置，若固化时将其值挪到对应的保存区中，需要注意的是，在每次修改参数前，
//需要对其初始化，并且置change_para_type_flag对应的标志


EXT_PARA void Load_TmepPara_Buf(void);

EXT_PARA UINT8 para_need_save_flag;
EXT_PARA void InitPara(void);
EXT_PARA void SavePara(void); 
#endif
