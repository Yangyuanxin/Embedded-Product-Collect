#ifndef FLASH_BLOB_H
#define FLASH_BLOB_H

#include <stdint.h>


typedef struct {
    uint32_t breakpoint;
    uint32_t static_base;
    uint32_t stack_pointer;
} program_syscall_t;

/* armfly : const 属性全部去掉了*/
typedef struct {   
    uint32_t  init;
    uint32_t  uninit;
    uint32_t  erase_chip;
    uint32_t  erase_sector;
    uint32_t  program_page;
    
    uint32_t  verify;
    uint32_t  check_blank;
    uint32_t  cacul_crc32;
    uint32_t  read_extid;
    
    program_syscall_t sys_call_s;
    uint32_t  program_buffer;
    uint32_t  program_buffer_size;    
    uint32_t  algo_start;
    uint32_t  algo_size;
    uint32_t *algo_blob;
    char algo_file_name[256];
} program_target_t;

typedef struct {   
    uint32_t  init;
    uint32_t  uninit;
    uint32_t  erase_chip;
    uint32_t  erase_sector;
    uint32_t  program_page;
    
    uint32_t  verify;
    uint32_t  check_blank;
    uint32_t  cacul_crc32;
    uint32_t  read_extid;
    
    program_syscall_t sys_call_s;
    uint32_t  program_buffer;
    uint32_t  program_buffer_size;    
    uint32_t  algo_start;
    uint32_t  algo_size;
//    uint32_t *algo_blob;
//    char algo_file_name[256];
} program_target_temp_t;

typedef struct {
    const uint32_t start;
    const uint32_t size;
} sector_info_t;


#endif
