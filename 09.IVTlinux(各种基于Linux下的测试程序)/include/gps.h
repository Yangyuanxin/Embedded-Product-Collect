/********************************************************************************
 *      Copyright:  (C) 2017 huiersen
 *                  All rights reserved.
 *
 *       Filename:  gps.h
 *    Description:  This head file
 *
 *        Version:  1.0.0(2017å¹´06æœˆ19æ—¥)
 *         Author:  pengyu
 *      ChangeLog:  1, Release initial version on "2017å¹´06æœˆ19æ—¥ 12æ—¶25åˆ†59ç§’"
 *
 ********************************************************************************/

#ifndef __GPS_H__
#define __GPS_H__

typedef unsigned int UINT;
typedef int BYTE;

typedef struct __gprmc__
{
   UINT time;			/* gps¶¨Î»Ê±¼ä */
   char pos_state;		/*gps×´Ì¬Î»*/
   char	ns;				/*Î³¶È°ëÇòN(±±°ëÇò)»òS(ÄÏ°ëÇò)*/
   char	ew;				/*¾­¶È°ëÇòE(¶«¾­)»òW(Î÷¾­)*/
   float latitude;		/*Î³¶È */ 
   float longitude;		/* ¾­¶È */  
   float speed;  		/* ËÙ¶È */ 
   float direction;		/*º½Ïò */  
   UINT date;  			/*ÈÕÆÚ  */  
   float declination; 	/* ´ÅÆ«½Ç */  
   char dd;
   char mode;			/* GPSÄ£Ê½Î» */ 
}GPRMC;

int initGpsUart();
int gpsModuleRun();

#endif
