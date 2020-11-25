
#line 1 "Source\UART0_mode_2.c" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
  
#line 1 "C:\Keil\C51\Inc\stdio.h" /0






 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 typedef unsigned int size_t;
 
 
 #pragma SAVE
 #pragma REGPARMS
 extern char _getkey (void);
 extern char getchar (void);
 extern char ungetchar (char);
 extern char putchar (char);
 extern int printf   (const char *, ...);
 extern int sprintf  (char *, const char *, ...);
 extern int vprintf  (const char *, char *);
 extern int vsprintf (char *, const char *, char *);
 extern char *gets (char *, int n);
 extern int scanf (const char *, ...);
 extern int sscanf (char *, const char *, ...);
 extern int puts (const char *);
 
 #pragma RESTORE
 
 
 
#line 18 "Source\UART0_mode_2.c" /0
 
  
#line 1 "..\..\Include\N76E885.h" /0




 
 
 
 
 
 sfr P0          = 0x80;
 sfr SP          = 0x81;
 sfr DPL         = 0x82;
 sfr DPH         = 0x83;
 sfr RWK         = 0x86;
 sfr PCON        = 0x87;
 
 sfr TCON        = 0x88;
 sfr TMOD        = 0x89;
 sfr TL0         = 0x8A;
 sfr TL1         = 0x8B;
 sfr TH0         = 0x8C;
 sfr TH1         = 0x8D;
 sfr CKCON       = 0x8E;
 sfr WKCON       = 0x8F;
 
 sfr P1          = 0x90;
 sfr SFRS        = 0x91;  
 sfr CAPCON0     = 0x92;
 sfr CAPCON1     = 0x93;
 sfr CAPCON2     = 0x94;
 sfr CKDIV       = 0x95;
 sfr CKSWT       = 0x96;  
 sfr CKEN        = 0x97;  
 
 sfr SCON        = 0x98;
 sfr SBUF        = 0x99;
 sfr SBUF_1      = 0x9A;
 sfr EIE         = 0x9B;
 sfr EIE1        = 0x9C;
 sfr CHPCON      = 0x9F;  
 
 sfr P2          = 0xA0;
 sfr AUXR1       = 0xA2;
 sfr BODCON0     = 0xA3;  
 sfr IAPTRG      = 0xA4;  
 sfr IAPUEN      = 0xA5;
 sfr IAPAL       = 0xA6;
 sfr IAPAH       = 0xA7;
 
 sfr IE          = 0xA8;
 sfr SADDR       = 0xA9;
 sfr WDCON       = 0xAA;  
 sfr BODCON1     = 0xAB;  
 sfr P3M1        = 0xAC;
 sfr P3S         = 0xAC;  
 sfr P3M2        = 0xAD;
 sfr P3SR        = 0xAD;  
 sfr IAPFD       = 0xAE;
 sfr IAPCN       = 0xAF;
 
 sfr P3          = 0xB0;
 sfr P0M1        = 0xB1;
 sfr P0S         = 0xB1;  
 sfr P0M2        = 0xB2;
 sfr P0SR        = 0xB2;  
 sfr P1M1        = 0xB3;
 sfr P1S         = 0xB3;  
 sfr P1M2        = 0xB4;
 sfr P1SR        = 0xB4;  
 sfr P2M1        = 0xB5;
 sfr P2S         = 0xB5;  
 sfr P2M2        = 0xB6;
 sfr P2SR        = 0xB6;  
 sfr IPH         = 0xB7;
 
 sfr IP          = 0xB8;
 sfr SADEN       = 0xB9;
 sfr SADEN_1     = 0xBA;
 sfr SADDR_1     = 0xBB;
 sfr I2DAT       = 0xBC;
 sfr I2STAT      = 0xBD;
 sfr I2CLK       = 0xBE;
 sfr I2TOC       = 0xBF;
 
 sfr I2CON       = 0xC0;
 sfr I2ADDR      = 0xC1;
 sfr ADCRL       = 0xC2;
 sfr ADCRH       = 0xC3;
 sfr T3CON       = 0xC4;
 sfr RL3         = 0xC5;
 sfr RH3         = 0xC6;
 sfr TA          = 0xC7;
 
 sfr T2CON       = 0xC8;
 sfr T2MOD       = 0xC9;
 sfr RCMP2L      = 0xCA;
 sfr RCMP2H      = 0xCB;
 sfr TL2         = 0xCC;
 sfr TH2         = 0xCD;
 sfr ADCMPL      = 0xCE;
 sfr ADCMPH      = 0xCF;
 
 sfr PSW         = 0xD0;
 sfr PWMPH       = 0xD1;
 sfr PWM01H      = 0xD2;
 sfr PWM23H      = 0xD3;
 sfr PWM67H      = 0xD4;
 sfr PWM45H      = 0xD5;
 sfr PNP         = 0xD6;
 sfr FBD         = 0xD7;
 
 sfr PWMCON0     = 0xD8;
 sfr PWMPL       = 0xD9;
 sfr PWM01L      = 0xDA;
 sfr PWM23L      = 0xDB;
 sfr PWM67L      = 0xDC;
 sfr PWM45L      = 0xDD;
 sfr PIO         = 0xDE;
 sfr PWMCON1     = 0xDF;
 
 sfr ACC         = 0xE0;
 sfr ADCCON1     = 0xE1;
 sfr ADCCON2     = 0xE2;
 sfr ADCDLY      = 0xE3;
 sfr C0L         = 0xE4;
 sfr C0H         = 0xE5;
 sfr C1L         = 0xE6;
 sfr C1H         = 0xE7;
 
 sfr ADCCON0     = 0xE8;
 sfr PICON       = 0xE9;
 sfr PINEN       = 0xEA;
 sfr PIPEN       = 0xEB;
 sfr PIF         = 0xEC;
 sfr C2L         = 0xED;
 sfr C2H         = 0xEE;
 sfr EIP         = 0xEF;
 
 sfr B           = 0xF0;
 sfr ADCAQT      = 0xF2;
 sfr SPCR        = 0xF3;
 sfr SPSR        = 0xF4;
 sfr SPDR        = 0xF5;
 sfr P0DIDS      = 0xF6;
 sfr EIPH        = 0xF7;
 
 sfr SCON_1      = 0xF8;
 sfr PDTEN       = 0xF9;  
 sfr PDTCNT      = 0xFA;  
 sfr PMEN        = 0xFB;
 sfr PMD         = 0xFC;
 sfr EIP1        = 0xFE;
 sfr EIPH1       = 0xFF;
 
 
 
 sbit SM0_1      = SCON_1^7;
 sbit FE_1       = SCON_1^7; 
 sbit SM1_1      = SCON_1^6; 
 sbit SM2_1      = SCON_1^5; 
 sbit REN_1      = SCON_1^4; 
 sbit TB8_1      = SCON_1^3; 
 sbit RB8_1      = SCON_1^2; 
 sbit TI_1       = SCON_1^1; 
 sbit RI_1       = SCON_1^0; 
 
 
 sbit ADCF       = ADCCON0^7;
 sbit ADCS       = ADCCON0^6;
 sbit ETGSEL1    = ADCCON0^5;
 sbit ETGSEL0    = ADCCON0^4;
 sbit ADCHS3     = ADCCON0^3;
 sbit ADCHS2     = ADCCON0^2;
 sbit ADCHS1     = ADCCON0^1;
 sbit ADCHS0     = ADCCON0^0;
 
 
 sbit PWMRUN     = PWMCON0^7;
 sbit LOAD       = PWMCON0^6;
 sbit PWMF       = PWMCON0^5;
 sbit CLRPWM     = PWMCON0^4;
 sbit INTTYP1    = PWMCON0^3;
 sbit INTTYP0    = PWMCON0^2;
 sbit INTSEL1    = PWMCON0^1;
 sbit INTSEL0    = PWMCON0^0;
 
 
 sbit CY         = PSW^7;
 sbit AC         = PSW^6;
 sbit F0         = PSW^5;
 sbit RS1        = PSW^4;
 sbit RS0        = PSW^3;
 sbit OV         = PSW^2;
 sbit P          = PSW^0;
 
 
 sbit TF2        = T2CON^7;
 sbit TR2        = T2CON^2;
 sbit CM_CR2     = T2CON^0;
 
 
 sbit I2CEN      = I2CON^6;
 sbit STA        = I2CON^5;
 sbit STO        = I2CON^4;
 sbit SI         = I2CON^3;
 sbit AA         = I2CON^2;
 
 
 sbit PADC       = IP^6;
 sbit PBOD       = IP^5;
 sbit PS         = IP^4;
 sbit PT1        = IP^3;
 sbit PX1        = IP^2;
 sbit PT0        = IP^1;
 sbit PX0        = IP^0;
 
 
 sbit P30        = P3^0;
 sbit P31        = P3^1;
 sbit P32        = P3^2;
 sbit P33        = P3^3;
 sbit P34        = P3^4;
 sbit P35        = P3^5;
 sbit P36        = P3^6;
 sbit P37        = P3^7;
 
 
 sbit EA         = IE^7;
 sbit EADC       = IE^6;
 sbit EBOD       = IE^5;
 sbit ES         = IE^4;
 sbit ET1        = IE^3;
 sbit EX1        = IE^2;
 sbit ET0        = IE^1;
 sbit EX0        = IE^0;
 
 
 sbit P20        = P2^0;
 sbit RXD        = P2^0; 
 sbit T0         = P2^0; 
 sbit IC0        = P2^0; 
 sbit AIN9       = P2^0;
 
 sbit P21        = P2^1;
 sbit MOSI       = P2^1;
 sbit T1         = P2^1;
 sbit IC1        = P2^1;
 
 sbit P22        = P2^2;
 sbit MISO       = P2^2;
 sbit IC2        = P2^2;
 
 sbit P23        = P2^3;
 sbit SDA        = P2^3;
 sbit FB         = P2^3;
 sbit STADC      = P2^3;
 
 sbit P24        = P2^4;
 sbit RXD_1      = P2^4;
 
 sbit P25        = P2^5;
 sbit TXD_1      = P2^5;
 
 sbit P26        = P2^6;
 sbit CLO        = P2^6;
 sbit AIN8       = P2^6;
 sbit PWM7       = P2^6;
 
 
 sbit SM0        = SCON^7;
 sbit FE         = SCON^7; 
 sbit SM1        = SCON^6; 
 sbit SM2        = SCON^5; 
 sbit REN        = SCON^4; 
 sbit TB8        = SCON^3; 
 sbit RB8        = SCON^2; 
 sbit TI         = SCON^1; 
 sbit RI         = SCON^0; 
 
 
 sbit P10        = P1^0;
 sbit PWM0       = P1^0;
 sbit P11        = P1^1;
 sbit PWM1       = P1^1;
 sbit P12        = P1^2;
 
 
 sbit TF1        = TCON^7;
 sbit TR1        = TCON^6;
 sbit TF0        = TCON^5;
 sbit TR0        = TCON^4;
 sbit IE1        = TCON^3;
 sbit IT1        = TCON^2;
 sbit IE0        = TCON^1;
 sbit IT0        = TCON^0;
 
 
 sbit P00        = P0^0;
 sbit INT0       = P0^0;
 sbit AIN0       = P0^0;
 sbit VREF       = P0^0;
 
 sbit P01        = P0^1;
 sbit INT1       = P0^1;
 sbit AIN1       = P0^1;
 
 sbit P02        = P0^2;
 sbit PWM2       = P0^2;
 sbit AIN2       = P0^2;
 
 sbit P03        = P0^3;
 sbit PWM3       = P0^3;
 sbit TXD        = P0^3;
 sbit AIN3       = P0^3;
 
 sbit P04        = P0^4;
 sbit SS         = P0^4;
 sbit AIN4       = P0^4;
 
 sbit P05        = P0^5;
 sbit PWM4       = P0^5;
 sbit SPICK      = P0^5;
 sbit AIN5       = P0^5;
 
 sbit P06        = P0^6;
 sbit SCL        = P0^6;
 sbit AIN6       = P0^6;
 sbit PWM5       = P0^6;
 
 sbit P07        = P0^7;
 sbit AIN7       = P0^7;
 sbit PWM6       = P0^7;
 
 
 
#line 19 "Source\UART0_mode_2.c" /0
 
  
#line 1 "..\..\Include\Typedef.h" /0
 typedef bit                   BIT;
 typedef unsigned char         UINT8;
 typedef unsigned int          UINT16;
 typedef unsigned long         UINT32;
 
 typedef unsigned char         uint8_t;
 typedef unsigned int          uint16_t;
 typedef unsigned long         uint32_t;
#line 20 "Source\UART0_mode_2.c" /0
 
  
#line 1 "..\..\Include\Define.h" /0
  
#line 1 "C:\Keil\C51\Inc\intrins.h" /0






 
 
 
 
 
 #pragma SAVE
 
 
#line 15 "C:\Keil\C51\Inc\intrins.h" /1
 
 
 
#line 18 "C:\Keil\C51\Inc\intrins.h" /0
 
 extern void          _nop_     (void);
 extern bit           _testbit_ (bit);
 extern unsigned char _cror_    (unsigned char, unsigned char);
 extern unsigned int  _iror_    (unsigned int,  unsigned char);
 extern unsigned long _lror_    (unsigned long, unsigned char);
 extern unsigned char _crol_    (unsigned char, unsigned char);
 extern unsigned int  _irol_    (unsigned int,  unsigned char);
 extern unsigned long _lrol_    (unsigned long, unsigned char);
 extern unsigned char _chkfloat_(float);
 
#line 29 "C:\Keil\C51\Inc\intrins.h" /1
 
 
#line 31 "C:\Keil\C51\Inc\intrins.h" /0
 
 extern void          _push_    (unsigned char _sfr);
 extern void          _pop_     (unsigned char _sfr);
 
 
 #pragma RESTORE
 
 
 
#line 1 "..\..\Include\Define.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 21 "Source\UART0_mode_2.c" /0
 
  
#line 1 "..\..\Include\SFR_Macro.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 22 "Source\UART0_mode_2.c" /0
 
 static xdata uint8_t uart_rcvbuf[64] _at_ 0x0;
 static data uint8_t bufhead _at_ 0x30 ;
 static data uint8_t tx_length _at_ 0x31 ;
 static data uint8_t lcmd _at_ 0x32 ;
 static data uint8_t g_timer1Counter _at_ 0x33;
 static data uint8_t count _at_ 0x34;
 static data uint16_t g_timer0Counter _at_ 0x35; 
 static bit bUartDataReady;
 static bit g_timer0Over;
 static bit g_timer1Over;
 
 
 
 void Send_64byte_To_UART0(void)
 {
 for(count=0;count<64;count++)
 {
 TI = 0;
 SBUF = uart_rcvbuf[count];
 while(TI==0);
 }
 }
 
 
 
 void Serial_ISR (void) interrupt 4 
 {
 if (RI == 1)
 {   
 uart_rcvbuf[bufhead++]=	SBUF;		
  RI = 0;                                          
 }
 if (TI == 1)
 {       
  TI = 0;                                          
 }
 if(bufhead ==1)
 {
 g_timer1Over=0;
 g_timer1Counter=75;  
 }
 if(bufhead == 64)
 {
 bUartDataReady = (1);
 bufhead = 0;
 }		
 }
 
 void Timer0_ISR (void) interrupt 1
 {
 if(g_timer0Counter)
 {
 g_timer0Counter--;
 if(!g_timer0Counter)
 {
 g_timer0Over=1;
 }
 }
 
 if(g_timer1Counter)
 {
 g_timer1Counter--;
 if(!g_timer1Counter)
 {
 g_timer1Over=1;
 }
 }
 
 } 


 
 void main (void)
 {
 
 P0M1 = 0x00;
 P0M2 = 0x00;
 
 
 P2M1 = 0x00;
 P2M2 = 0x00;
 
 
 
 SCON = 0x52;      
 TMOD |= 0x22;     
 
  PCON |= 0x80;
  CKCON |= 0x10;
  T3CON &= ~0x20;         
 TH1 = 256 - (1382400/115200);                 
  TR1 = 1; 					 
 
 TH0=TL0=0;  
  TR0 = 1;    
  IPH |= 0x10;    
 IE = 0x92;                                       
 g_timer0Over=0;
 g_timer0Counter=5000;
 
 
 while(1)
 {
 if(bUartDataReady == (1))
 {
 lcmd = uart_rcvbuf[0];
 if(lcmd == 0xAE) 
 {
 
 
 EA=0;  
 uart_rcvbuf[4]++;
 Send_64byte_To_UART0();						
 goto _ISP;
 }
 else
 {
 bUartDataReady = (0);
 bufhead = 0;
 }
 }
 
 if(g_timer0Over==1)
 {			 
 goto _APROM;
 }
 
 if(g_timer1Over==1)
 {			 
 if(bufhead<64&&bufhead>0)
 {
 bufhead=0;				 
 }
 }	
 
 
 }   
 _ISP:
 while(1);
 
 
 _APROM:
 TA = 0xAA;
 TA = 0x55;
 CHPCON &= 0xFD;                   
 TA = 0xAA;
 TA = 0x55;
 CHPCON |= 0x80;                    
 
 
 
 while(1);	
 }
 
