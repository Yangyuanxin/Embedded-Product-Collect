// Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
// Licence: GPL

#ifndef __MARLIN_MAIN_H
#define __MARLIN_MAIN_H


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"
#include "Configuration.h"
#include "pins.h"
#include "adc.h"
#include "include_conf.h"



#define OK_KEY (Get_Adc_Average(ADC_Channel_15,10)>3250)

void get_command(void);
void process_commands(void);
void manage_inactivity(void);

#define  enable_x()  X_ENABLE_PIN=X_ENABLE_ON
#define disable_x()	 X_ENABLE_PIN=!X_ENABLE_ON

#define  enable_y()  Y_ENABLE_PIN=Y_ENABLE_ON
#define disable_y()	 Y_ENABLE_PIN=!Y_ENABLE_ON

#define  enable_z()  Z_ENABLE_PIN=Z_ENABLE_ON
#define disable_z()	 Z_ENABLE_PIN=!Z_ENABLE_ON

#define  enable_e0() E0_ENABLE_PIN=E_ENABLE_ON
#define disable_e0() E0_ENABLE_PIN=!E_ENABLE_ON

#define  enable_e1() E1_ENABLE_PIN=E_ENABLE_ON
#define disable_e1() E1_ENABLE_PIN=!E_ENABLE_ON

enum AxisEnum {X_AXIS=0, Y_AXIS=1, Z_AXIS=2, E_AXIS=3};

#define SERIAL_ERROR_START	printf("Error:")
#define SERIAL_ECHO_START	printf("echo:")
void loop(void);
void setup(void);
void FlushSerialRequestResend(void);
void ClearToSend(void);


void get_coordinates(void);
void prepare_move(void);
void kill(void);
void Stop(void);

bool IsStopped(void);

void enquecommand(const char *cmd); //put an ascii command at the end of the current buffer.
//void enquecommand_P(const char *cmd); //put an ascii command at the end of the current buffer, read from flash
void prepare_arc_move(u8 isclockwise);

void clamp_to_software_endstops(float target[3]);

#define CRITICAL_SECTION_START  __disable_irq();
#define CRITICAL_SECTION_END    __enable_irq();
extern float homing_feedrate[];
extern bool axis_relative_modes[];
extern int feedmultiply;
extern int extrudemultiply; // Sets extrude multiply factor (in percent)
extern float current_position[NUM_AXIS] ;
extern float add_homeing[3];
extern float min_pos[3];
extern float max_pos[3];
extern int fanSpeed;
extern unsigned long starttime;
#ifdef BARICUDA
extern int ValvePressure;
extern int EtoPPressure;
#endif

#ifdef FWRETRACT
extern bool autoretract_enabled;
extern bool retracted;
extern float retract_length, retract_feedrate, retract_zlift;
extern float retract_recover_length, retract_recover_feedrate;
#endif

extern unsigned long starttime;
extern unsigned long stoptime;

extern bool lcd_contiune_print_after_poweroff;
extern unsigned long poweroff_sdpos;
extern float poweroff_position_z;
extern char poweroff_printing_filename[30];

// Handling multiple extruders pins
extern uint8_t active_extruder;
#endif
