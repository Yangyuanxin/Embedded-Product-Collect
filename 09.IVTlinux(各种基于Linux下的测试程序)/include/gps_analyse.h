#ifndef __GPS_ANALYSE_H__
#define __GPS_ANALYSE_H__

#include "gps.h"

int print_gps(GPRMC *gps_date);
int gps_analyse(char *buff,GPRMC *gps_data);

#endif
