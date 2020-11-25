typedef enum 
{
    E_HXTEN,  //External 2~24MHz
    E_LXTEN,  //External 32.768KHz
    E_HIRCEN, //Internal 22.1184MHz
    E_LIRCEN, //Internal 10KHz
    E_OSCEN,  //External OSC
} E_SYSTEM_CLOCK;

void  InitialUART0_Timer1_Type1(UINT32 u32Baudrate); //T1M = 0, SMOD = 0
void  InitialUART0_Timer1_Type2(UINT32 u32Baudrate); //T1M = 0, SMOD = 1
void  InitialUART0_Timer1_Type3(UINT32 u32Baudrate); //T1M = 1, SMOD = 0
void  InitialUART0_Timer1_Type4(UINT32 u32Baudrate); //T1M = 1, SMOD = 1
void  InitialUART0_Timer3_Type5(UINT32 u32Baudrate); //Timer3 as Baudrate, SMOD=0, Prescale=0
void  InitialUART0_Timer3_Type6(UINT32 u32Baudrate); //Timer3 as Baudrate, SMOD=1, Prescale=0
void  InitialUART1_Timer3(UINT32 u32Baudrate);
void  Send_Data_To_UART0(UINT8 c);
UINT8 Receive_Data_From_UART0(void);
void  Send_Data_To_UART1(UINT8 c);
UINT8 Receive_Data_From_UART1(void);

void  InitialUART1(UINT32 u32Baudrate);
void  Set_All_GPIO_Quasi_Mode(void);
void  SW_Reset(void);
void  System_Clock_Select(E_SYSTEM_CLOCK clock);
void  Fsys_Clock_Output_On_P26(void);

extern bit BIT_TMP;

