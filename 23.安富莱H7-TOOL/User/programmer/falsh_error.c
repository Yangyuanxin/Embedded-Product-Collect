/**
 * @file    error.c
 * @brief   collection of known errors and accessor for the friendly string
 */

#include "error.h"

static const char *const error_message[] = {

    /* Shared errors */

    // ERROR_SUCCESS
    "Operation was successful",
    // ERROR_FAILURE
    "An error has occurred",
    // ERROR_INTERNAL
    "An internal error has occurred",


    /* Target flash errors */

    // ERROR_RESET
    "The interface firmware FAILED to reset/halt the target MCU",
    // ERROR_ALGO_DL
    "The interface firmware FAILED to download the flash programming algorithms to the target MCU",
    // ERROR_ALGO_DATA_SEQ
    "The interface firmware FAILED to download the flash data contents to be programmed",
    // ERROR_INIT
    "The interface firmware FAILED to initialize the target MCU",
    // ERROR_SECURITY_BITS
    "The interface firmware ABORTED programming. Image is trying to set security bits",
    // ERROR_UNLOCK
    "The interface firmware FAILED to unlock the target for programming",
    // ERROR_ERASE_SECTOR
    "Flash algorithm erase sector command FAILURE",
    // ERROR_ERASE_ALL
    "Flash algorithm erase all command FAILURE",
    // ERROR_WRITE
    "Flash algorithm write command FAILURE",
};

static error_type_t error_type[] = {

    /* These should always stay the same for each error type. */

    // ERROR_SUCCESS
    0,
    // ERROR_FAILURE
    ERROR_TYPE_INTERNAL,
    // ERROR_INTERNAL
    ERROR_TYPE_INTERNAL,


    /* Target flash errors */

    // ERROR_RESET
    ERROR_TYPE_TARGET,
    // ERROR_ALGO_DL
    ERROR_TYPE_TARGET,
    // ERROR_ALGO_DATA_SEQ
    ERROR_TYPE_TARGET,
    // ERROR_INIT
    ERROR_TYPE_TARGET,
    // ERROR_SECURITY_BITS
    ERROR_TYPE_USER,
    // ERROR_UNLOCK
    ERROR_TYPE_TARGET,
    // ERROR_ERASE_SECTOR
    ERROR_TYPE_TARGET,
    // ERROR_ERASE_ALL
    ERROR_TYPE_TARGET,
    // ERROR_WRITE
    ERROR_TYPE_TARGET,
};

const char *error_get_string(error_t error)
{
    const char *msg = 0;

    if (error < ERROR_COUNT) {
        msg = error_message[error];
    }

    return msg;
}

error_type_t error_get_type(error_t error)
{
    error_type_t type = ERROR_TYPE_INTERNAL;

    if (error < ERROR_COUNT) {
        type = error_type[error];
    }

    return type;
}
