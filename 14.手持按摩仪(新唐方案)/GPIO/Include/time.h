#ifndef __TIME_H__
#define TMR0_INIT        33333 
#define TMR1_INIT        33333
#define TMR2_INIT        25000
#define TMR3_INIT        50000


#define DO             201//211// 1265   //262HZ	  211
#define RE             178//188 //1125   //294HZ	  188
#define MI             157//167// 1003   //330HZ	  167
#define FA             148// 158// 947   //349HZ	  158
#define SO             131// 141// 843   //392HZ	  141
#define SO1            119// 141// 843   //392HZ	  141
#define LA             115// 125// 750   //440HZ	  125
#define SI             101// 111// 667   //494HZ	  111

#define POW_RF          P06     //RISE VOLTAGE FOUR SHIFT
#define MODE            P16     //MODE SELECT
#define LED1            P30
#define LED2            P12
#define LED3            P13
#define LED4            P15
#define RED             P11
#define YELLOW          P10
#define BLUE            P00
#define MOS             P30
#define BUZ             P17
#define MOTO            P03
#define FDC633_CTL      P07
#define AIC3415_CTL     P01


 void Time_Init(void);
#endif