;********************************************************************************************************
;                                               uC/CRC
;           ERROR DETECTING CODE (EDC) & ERROR CORRECTING CODE (ECC) CALCULATION UTILITIES
;
;                    Copyright 2007-2020 Silicon Laboratories Inc. www.silabs.com
;
;                                 SPDX-License-Identifier: APACHE-2.0
;
;               This software is subject to an open source license and is distributed by
;                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
;                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
;
;********************************************************************************************************


;********************************************************************************************************
;
;                              CYCLIC REDUNDANCY CHECK (CRC) CALCULATION
;
;                                            ARM-Cortex-M3
;                                            IAR Compiler
;
; Filename : edc_crc_a.asm
; Version  : V1.10.00
;********************************************************************************************************
; Note(s)  : (1) Assumes ARM CPU mode configured for Little Endian.
;********************************************************************************************************


;********************************************************************************************************
;                                          PUBLIC FUNCTIONS
;********************************************************************************************************

;        PUBLIC  CRC_ChkSumCalcTbl_16Bit
;        PUBLIC  CRC_ChkSumCalcTbl_16Bit_ref
;        PUBLIC  CRC_ChkSumCalcTbl_32Bit
;        PUBLIC  CRC_ChkSumCalcTbl_32Bit_ref


;********************************************************************************************************
;                                     CODE GENERATION DIRECTIVES
;********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)

	AREA    OSKERNEL, CODE, READONLY, ALIGN=2
	PRESERVE8

	EXPORT  CRC_ChkSumCalcTbl_16Bit
    EXPORT  CRC_ChkSumCalcTbl_16Bit_ref
    EXPORT  CRC_ChkSumCalcTbl_32Bit        
	EXPORT  CRC_ChkSumCalcTbl_32Bit 	
    
	THUMB

;$PAGE
;********************************************************************************************************
;                                       CRC_ChkSumCalcTbl_16Bit()
;
; Description : Calculate a 16-bit CRC using a table without reflection.
;
; Argument(s) : init_val        Initial CRC value.
;
;               ptbl            Pre-computed CRC table to use in calculation.
;
;               pdata           Pointer to data buffer over which CRC is generated.
;
;               nbr_octets      Number of data octets to use for calculation
;
; Return(s)   : 16-bit CRC.
;
; Caller(s)   : Application.
*********************************************************************************************************

; CPU_INT16  CRC_ChkSumCalcTbl_16Bit (CPU_INT16U   init_val,   @       ==>  R0 == sum
;                                     CPU_INT16U  *ptbl,       @       ==>  R1
;                                     CPU_INT08U  *pdata,      @       ==>  R2
;                                     CPU_INT32U   nbr_octets) @       ==>  R3
;                                                  ix          @       ==>  R4
;                                                  tbl_val     @       ==>  R5
;                                                  temp        @       ==>  R6
;                                                  0x1FF       @       ==>  R7

CRC_ChkSumCalcTbl_16Bit
        STMDB       SP!, {R4-R7}

        MOVW        R7, #0x1FE
        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_16Bit_END


CRC_ChkSumCalcTbl_16Bit_CHKALIGN32
        AND         R6, R2, #0x03
        CMP         R6, #0
        BEQ         CRC_ChkSumCalcTbl_16Bit_ALIGN32


CRC_ChkSumCalcTbl_16Bit_PRE
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0,  LSR #8                   ; ix     ^= (crc >> 8);
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;
        ADD         R6,  R6, #1

        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_16Bit_END

        CMP         R6, #4
        BNE         CRC_ChkSumCalcTbl_16Bit_PRE
        B           CRC_ChkSumCalcTbl_16Bit_ALIGN32


CRC_ChkSumCalcTbl_16Bit_ALIGN32_LOOP
        LDR         R6, [R2], #4                            ; temp    = *pdata++;

        EOR         R4,  R6,  R0,  LSR #8                   ; ix      =  temp ^ (crc >> 8);
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  ((ix >> 0) * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        EOR         R4,  R6,  R0                            ; ix      =  temp ^ crc;
        AND         R4,  R7,  R4,  LSR #7                   ; ix      =  ((ix >> 8) * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        EOR         R4,  R6,  R0,  LSL #8                   ; ix      =  temp ^ (crc << 8);
        AND         R4,  R7,  R4,  LSR #15                  ; ix      =  ((ix >> 16) * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        EOR         R4,  R6,  R0,  LSL #16                  ; ix      =  temp ^ (crc << 16);
        AND         R4,  R7,  R4,  LSR #23                  ; ix      =  ((ix >> 24) * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        SUB         R3,  R3, #4                             ; nbytes -= 4;


CRC_ChkSumCalcTbl_16Bit_ALIGN32
        CMP         R3, #(04*01*01)
        BCS         CRC_ChkSumCalcTbl_16Bit_ALIGN32_LOOP
        BCC         CRC_ChkSumCalcTbl_16Bit_POST


CRC_ChkSumCalcTbl_16Bit_POST_LOOP
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0,  LSR #8                   ; ix     ^= (crc >> 8);
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;


CRC_ChkSumCalcTbl_16Bit_POST
        CMP         R3, #0
        BNE         CRC_ChkSumCalcTbl_16Bit_POST_LOOP


CRC_ChkSumCalcTbl_16Bit_END
        UXTH        R0, R0
        LDMIA       SP!, {R4-R7}
        BX          LR                                      ; return


;$PAGE
;********************************************************************************************************
;                                     CRC_ChkSumCalcTbl_16Bit_ref()
;
; Description : Calculate a 16-bit CRC using a table with reflection.
;
; Argument(s) : init_val        Initial CRC value.
;
;               ptbl            Pre-computed CRC table to use in calculation.
;
;               pdata           Pointer to data buffer over which CRC is generated.
;
;               nbr_octets      Number of data octets to use for calculation
;
; Return(s)   : 16-bit CRC.
;
; Caller(s)   : Application.
*********************************************************************************************************

; CPU_INT16  CRC_ChkSumCalcTbl_16Bit_ref (CPU_INT16U   init_val,   @       ==>  R0 == sum
;                                         CPU_INT16U  *ptbl,       @       ==>  R1
;                                         CPU_INT08U  *pdata,      @       ==>  R2
;                                         CPU_INT32U   nbr_octets) @       ==>  R3
;                                                      ix          @       ==>  R4
;                                                      tbl_val     @       ==>  R5
;                                                      temp        @       ==>  R6
;                                                      0x1FF       @       ==>  R7

CRC_ChkSumCalcTbl_16Bit_ref
        STMDB       SP!, {R4-R7}

        MOVW        R7, #0x1FE
        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_16Bit_ref_END


CRC_ChkSumCalcTbl_16Bit_ref_CHKALIGN32
        AND         R6, R2, #0x03
        CMP         R6, #0
        BEQ         CRC_ChkSumCalcTbl_16Bit_ref_ALIGN32


CRC_ChkSumCalcTbl_16Bit_ref_PRE
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0                            ; ix     ^=  crc;
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;
        ADD         R6,  R6, #1

        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_16Bit_ref_END

        CMP         R6, #4
        BNE         CRC_ChkSumCalcTbl_16Bit_ref_PRE
        B           CRC_ChkSumCalcTbl_16Bit_ref_ALIGN32


CRC_ChkSumCalcTbl_16Bit_ref_ALIGN32_LOOP
        LDR         R6, [R2], #4                            ; temp    = *pdata++;

        EOR         R4,  R6,  R0                            ; ix      =  temp ^ crc;
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        EOR         R4,  R0,  R6,  LSR #8                   ; ix     ^=  crc ^ (temp >> 8);
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        EOR         R4,  R0,  R6,  LSR #16                  ; ix      =  crc ^ (temp >> 16);
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        EOR         R4,  R0,  R6,  LSR #24                  ; ix      =  crc ^ (temp >> 24);
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        SUB         R3,  R3, #4                             ; nbytes -= 4;


CRC_ChkSumCalcTbl_16Bit_ref_ALIGN32
        CMP         R3, #(04*01*01)
        BCS         CRC_ChkSumCalcTbl_16Bit_ref_ALIGN32_LOOP
        BCC         CRC_ChkSumCalcTbl_16Bit_ref_POST


CRC_ChkSumCalcTbl_16Bit_ref_POST_LOOP
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0                            ; ix     ^=  crc;
        AND         R4,  R7,  R4,  LSL #1                   ; ix      =  (ix * 2) & 0x1FE;
        LDRH        R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;


CRC_ChkSumCalcTbl_16Bit_ref_POST
        CMP         R3, #0
        BNE         CRC_ChkSumCalcTbl_16Bit_ref_POST_LOOP


CRC_ChkSumCalcTbl_16Bit_ref_END
        UXTH        R0, R0
        LDMIA       SP!, {R4-R7}
        BX          LR                                      ; return


;$PAGE
;********************************************************************************************************
;                                       CRC_ChkSumCalcTbl_32Bit()
;
; Description : Calculate a 32-bit CRC using a table without reflection.
;
; Argument(s) : init_val        Initial CRC value.
;
;               ptbl            Pre-computed CRC table to use in calculation.
;
;               pdata           Pointer to data buffer over which CRC is generated.
;
;               nbr_octets      Number of data octets to use for calculation
;
; Return(s)   : 32-bit CRC.
;
; Caller(s)   : Application.
*********************************************************************************************************

; CPU_INT32  CRC_ChkSumCalcTbl_32Bit (CPU_INT32U   init_val,   @       ==>  R0 == sum
;                                     CPU_INT32U  *ptbl,       @       ==>  R1
;                                     CPU_INT08U  *pdata,      @       ==>  R2
;                                     CPU_INT32U   nbr_octets) @       ==>  R3
;                                                  ix          @       ==>  R4
;                                                  tbl_val     @       ==>  R5
;                                                  temp        @       ==>  R6
;                                                  0x3FF       @       ==>  R7

CRC_ChkSumCalcTbl_32Bit
        STMDB       SP!, {R4-R7}

        MOVW        R7, #0x3FC
        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_32Bit_END


CRC_ChkSumCalcTbl_32Bit_CHKALIGN32
        AND         R6, R2, #0x03
        CMP         R6, #0
        BEQ         CRC_ChkSumCalcTbl_32Bit_ALIGN32


CRC_ChkSumCalcTbl_32Bit_PRE
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0,  LSR #24                  ; ix     ^= (crc >> 24);
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;
        ADD         R6,  R6, #1

        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_32Bit_END

        CMP         R6, #4
        BNE         CRC_ChkSumCalcTbl_32Bit_PRE
        B           CRC_ChkSumCalcTbl_32Bit_ALIGN32


CRC_ChkSumCalcTbl_32Bit_ALIGN32_LOOP
        LDR         R6, [R2], #4                            ; temp    = *pdata++;

        EOR         R4,  R6,  R0,  LSR #24                  ; ix      =  temp ^ (crc >> 24);
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        EOR         R4,  R6,  R0,  LSR #16                  ; ix      =  temp ^ (crc >> 16);
        AND         R4,  R7,  R4,  LSR #6                   ; ix      =  ((ix >> 8) * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        EOR         R4,  R6,  R0,  LSR #8                   ; ix      =  temp ^ (crc >> 8);
        AND         R4,  R7,  R4,  LSR #14                  ; ix      =  ((ix >> 16) * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        EOR         R4,  R6,  R0                            ; ix      =  temp ^ crc;
        AND         R4,  R7,  R4,  LSR #22                  ; ix      =  ((ix >> 24) * 4) & 0x3FF;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        SUB         R3,  R3, #4                             ; nbytes -= 4;


CRC_ChkSumCalcTbl_32Bit_ALIGN32
        CMP         R3, #(04*01*01)
        BCS         CRC_ChkSumCalcTbl_32Bit_ALIGN32_LOOP
        BCC         CRC_ChkSumCalcTbl_32Bit_POST


CRC_ChkSumCalcTbl_32Bit_POST_LOOP
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0,  LSR #24                  ; ix     ^= (crc >> 24);
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSL #8                   ; crc     = (crc << 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;


CRC_ChkSumCalcTbl_32Bit_POST
        CMP         R3, #0
        BNE         CRC_ChkSumCalcTbl_32Bit_POST_LOOP


CRC_ChkSumCalcTbl_32Bit_END
        LDMIA       SP!, {R4-R7}
        BX          LR                                      ; return


;$PAGE
;********************************************************************************************************
;                                     CRC_ChkSumCalcTbl_32Bit_ref()
;
; Description : Calculate a 32-bit CRC using a table with reflection.
;
; Argument(s) : init_val        Initial CRC value.
;
;               ptbl            Pre-computed CRC table to use in calculation.
;
;               pdata           Pointer to data buffer over which CRC is generated.
;
;               nbr_octets      Number of data octets to use for calculation
;
; Return(s)   : 16-bit CRC.
;
; Caller(s)   : Application.
*********************************************************************************************************

; CPU_INT32  CRC_ChkSumCalcTbl_32Bit_ref (CPU_INT32U   init_val,   @       ==>  R0 == sum
;                                         CPU_INT32U  *ptbl,       @       ==>  R1
;                                         CPU_INT08U  *pdata,      @       ==>  R2
;                                         CPU_INT32U   nbr_octets) @       ==>  R3
;                                                      ix          @       ==>  R4
;                                                      tbl_val     @       ==>  R5
;                                                      temp        @       ==>  R6
;                                                      0x3FF       @       ==>  R7

CRC_ChkSumCalcTbl_32Bit_ref
        STMDB       SP!, {R4-R7}

        MOVW        R7, #0x3FC
        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_32Bit_ref_END


CRC_ChkSumCalcTbl_32Bit_ref_CHKALIGN32
        AND         R6, R2, #0x03
        CMP         R6, #0
        BEQ         CRC_ChkSumCalcTbl_32Bit_ref_ALIGN32


CRC_ChkSumCalcTbl_32Bit_ref_PRE
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0                            ; ix     ^=  crc;
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;
        ADD         R6,  R6, #1

        CMP         R3, #0
        BEQ         CRC_ChkSumCalcTbl_32Bit_ref_END

        CMP         R6, #4
        BNE         CRC_ChkSumCalcTbl_32Bit_ref_PRE
        B           CRC_ChkSumCalcTbl_32Bit_ref_ALIGN32


CRC_ChkSumCalcTbl_32Bit_ref_ALIGN32_LOOP
        LDR         R6, [R2], #4                            ; temp    = *pdata++;

        EOR         R4,  R0,  R6                            ; ix      =  temp ^ crc;
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        EOR         R4,  R0,  R6,  LSR #8                   ; ix      =  crc ^ (temp >> 8);
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        EOR         R4,  R0,  R6,  LSR #16                  ; ix      =  crc ^ (temp >> 16);
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        EOR         R4,  R0,  R6,  LSR #24                  ; ix      =  crc ^ (temp >> 24);
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FC;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        SUB         R3,  R3, #4                             ; nbytes -= 4;


CRC_ChkSumCalcTbl_32Bit_ref_ALIGN32
        CMP         R3, #(04*01*01)
        BCS         CRC_ChkSumCalcTbl_32Bit_ref_ALIGN32_LOOP
        BCC         CRC_ChkSumCalcTbl_32Bit_ref_POST


CRC_ChkSumCalcTbl_32Bit_ref_POST_LOOP
        LDRB        R4, [R2], #1                            ; ix      = *pdata++;
        EOR         R4,  R4,  R0                            ; ix     ^=  crc;
        AND         R4,  R7,  R4,  LSL #2                   ; ix      =  (ix * 4) & 0x3FF;
        LDR         R5, [R1, R4]                            ; tbl_val = *(ptbl + ix);
        EOR         R0,  R5,  R0,  LSR #8                   ; crc     = (crc >> 8) ^ tbl_val;

        SUB         R3,  R3, #1                             ; nbytes--;


CRC_ChkSumCalcTbl_32Bit_ref_POST
        CMP         R3, #0
        BNE         CRC_ChkSumCalcTbl_32Bit_ref_POST_LOOP


CRC_ChkSumCalcTbl_32Bit_ref_END
        LDMIA       SP!, {R4-R7}
        BX          LR                                      ; return

        END
