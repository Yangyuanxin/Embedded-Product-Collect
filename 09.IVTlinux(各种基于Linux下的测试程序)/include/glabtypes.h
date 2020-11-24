#ifndef GBLTYPES_INCLUDED
#define GBLTYPES_INCLUDED

#define PACKED __attribute__((__packed__))

#define SD_TRUE				1
#define SD_FALSE			0

#define SUCCESS  			1
#define FAILURE 			0

#define TRUE				1
#define FALSE				0

#define DEBUG 				printf//(format, arg...)  do {  printf(  "%s: " format "\n",__FUNCTION__, ## arg); } while (0)

#define ST_UCHAR			unsigned char	
#define ST_UINT				unsigned int		
#define ST_ULONG			unsigned long
#define ST_INT16			signed short 
#define ST_UINT16			unsigned short
#define ST_ULONGLONG		unsigned long long 

typedef enum __bool
{
	false=0,
	true=1
}bool;
typedef bool BOOL;

#ifndef max
#define max(a, b)	(((a) > (b))? (a): (b))
#endif
#ifndef min
#define min(a, b)	(((a) < (b))? (a): (b))
#endif

/*¶ÁĞ´½á¹¹Ìå*/
typedef struct _info_rw
{
	ST_UCHAR read;
	ST_UCHAR write;
}PACKED info_rw;

#endif

