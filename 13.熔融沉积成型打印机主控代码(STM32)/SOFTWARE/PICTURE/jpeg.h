/*----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor include file               (C)ChaN, 2012
/----------------------------------------------------------------------------*/
#ifndef __JPEG_H//_TJPGDEC
#define __JPEG_H//_TJPGDEC
/*---------------------------------------------------------------------------*/
/* System Configurations */
//////////////////////////////////////////用户配置区//////////////////////////////////


#define	JD_SZBUF			1024	/* Size of stream input buffer */
#define JD_FORMAT			1		/* Output pixel format 0:RGB888 (3 BYTE/pix), 1:RGB565 (1 WORD/pix) */
#define	JD_USE_SCALE		1		/* Use descaling feature for output */
#define JD_TBLCLIP			1		/* Use table for saturation (might be a bit faster but increases 1K bytes of code size) */


#define JPEG_USE_MALLOC		1 		//定义是否使用malloc,这里我们选择使用malloc
#define JPEG_WBUF_SIZE  	4096 	//定义工作区数组大小,最少应不小于3092字节.	

//////////////////////////////////////////////END/////////////////////////////////////
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"
#include "sys.h"

	
	
/* Error code */
typedef enum {
	JDR_OK = 0,	/* 0: Succeeded */
	JDR_INTR,	/* 1: Interrupted by output function */	
	JDR_INP,	/* 2: Device error or wrong termination of input stream */
	JDR_MEM1,	/* 3: Insufficient memory pool for the image */
	JDR_MEM2,	/* 4: Insufficient stream input buffer */
	JDR_PAR,	/* 5: Parameter error */
	JDR_FMT1,	/* 6: Data format error (may be damaged data) */
	JDR_FMT2,	/* 7: Right format but not supported */
	JDR_FMT3	/* 8: Not supported JPEG standard */
} JRESULT;



/* Rectangular structure */
typedef struct {
	WORD left, right, top, bottom;
} JRECT;



/* Decompressor object structure */
typedef struct JDEC JDEC;
struct JDEC {
	UINT dctr;				/* Number of bytes available in the input buffer */
	BYTE* dptr;				/* Current data read ptr */
	BYTE* inbuf;			/* Bit stream input buffer */
	BYTE dmsk;				/* Current bit in the current read byte */
	BYTE scale;				/* Output scaling ratio */
	BYTE msx, msy;			/* MCU size in unit of block (width, height) */
	BYTE qtid[3];			/* Quantization table ID of each component */
	SHORT dcv[3];			/* Previous DC element of each component */
	WORD nrst;				/* Restart inverval */
	UINT width, height;		/* Size of the input image (pixel) */
	BYTE* huffbits[2][2];	/* Huffman bit distribution tables [id][dcac] */
	WORD* huffcode[2][2];	/* Huffman code word tables [id][dcac] */
	BYTE* huffdata[2][2];	/* Huffman decoded data tables [id][dcac] */
	LONG* qttbl[4];			/* Dequaitizer tables [id] */
	void* workbuf;			/* Working buffer for IDCT and RGB output */
	BYTE* mcubuf;			/* Working buffer for the MCU */
	void* pool;				/* Pointer to available memory pool */
	UINT sz_pool;			/* Size of momory pool (bytes available) */
	UINT (*infunc)(JDEC*, BYTE*, UINT);/* Pointer to jpeg stream input function */
	void* device;			/* Pointer to I/O device identifiler for the session */
};



/* TJpgDec API functions */
JRESULT jd_prepare (JDEC*, UINT(*)(JDEC*,BYTE*,UINT), void*, UINT, void*);
JRESULT jd_decomp (JDEC*, UINT(*)(JDEC*,void*,JRECT*), BYTE);
u8 jpg_decode(const u8 *filename,u8 fast);

#ifdef __cplusplus
}
#endif

#endif /* _TJPGDEC */
