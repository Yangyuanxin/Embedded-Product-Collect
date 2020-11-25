 

 
#define TRUE	   	(1)
#define FALSE   	(0)

#define CMD_CONNECT					0xAE
#define CMD_SYNC_PACKNO			0xA4
#define CMD_GET_FWVER				0xA6
#define FW_VERSION					0x27
#define CMD_RUN_APROM				0xAB
#define CMD_GET_DEVICEID		0xB1
#define CMD_READ_CONFIG			0xA2
#define	CMD_UPDATE_CONFIG		0xA1
#define CMD_UPDATE_APROM		0xA0
#define PAGE_ERASE_AP				0x22
#define BYTE_READ_AP				0x00
#define BYTE_PROGRAM_AP			0x21
#define BYTE_READ_ID				0x0C
#define PAGE_ERASE_CONFIG		0xE2
#define BYTE_READ_CONFIG		0xC0
#define BYTE_PROGRAM_CONFIG	0xE1
#define	READ_UID						0x04
#define PAGE_SIZE           128
#define APROM_SIZE          8*1024  