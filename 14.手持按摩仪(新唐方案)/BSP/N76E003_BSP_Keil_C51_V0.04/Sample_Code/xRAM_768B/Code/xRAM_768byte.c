#include <stdio.h>
#include "N76E003.h"
#include "Function_define.h"

/******************************************************
 The C file combine with assembler code compiler
  Please always confirm the .c file option mark enable 
	"Generate assembler SRC file" and "Assembler SRC file
******************************************************/


/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/
void main (void) 
{

	Set_All_GPIO_Quasi_Mode;
               
#pragma asm 
			MOV DPH,0
			MOV DPL,0
			
			;Write XRAM
L1:
			MOV A,DPL
			MOVX @DPTR,A
			INC DPTR
			MOV A,DPH
			CJNE A,#03H,L1


			;read check
			MOV DPL,#0
			MOV DPH,#0
L2:
			MOV A,DPL
			MOV 11H,A
			MOVX A,@DPTR
			CJNE A,11H,L3
			INC DPTR
			MOV A,DPH
			CJNE A,#03H,L2
			MOV P1,#69
			sjmp $
			;read check end	


			;error alart
L3:
			MOV R0,#05h
			MOV A,#55H
			MOVX @R0,A
			MOV P1,A
			sjmp $
			
#pragma endasm 
             

}