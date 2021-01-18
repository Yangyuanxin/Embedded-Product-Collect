#ifndef ERROR_H
#define ERROR_H


// Keep in sync with the lists error_message and error_type
typedef enum {
    /* Shared errors */
    ERROR_SUCCESS = 0,
    ERROR_FAILURE,
    ERROR_INTERNAL,

    /* Target flash errors */
    ERROR_RESET,
    ERROR_ALGO_DL,
    ERROR_ALGO_DATA_SEQ,
    ERROR_INIT,
    ERROR_SECURITY_BITS,
    ERROR_UNLOCK,
    ERROR_ERASE_SECTOR,
    ERROR_ERASE_ALL,
    ERROR_WRITE,

    // Add new values here

    ERROR_COUNT
} error_t;


typedef unsigned char error_type_t;

#define ERROR_TYPE_INTERNAL 	0x1
#define ERROR_TYPE_TRANSIENT 	0x2
#define ERROR_TYPE_USER 		0x4
#define ERROR_TYPE_TARGET 		0x8

#define ERROR_TYPE_MASK 		0xF


const char *error_get_string(error_t error);

error_type_t error_get_type(error_t error);


#endif
