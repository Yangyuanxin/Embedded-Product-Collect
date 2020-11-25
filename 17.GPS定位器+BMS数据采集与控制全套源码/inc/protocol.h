#ifndef __GPS_PROTOCOL__
#define __GPS_PROTOCOL__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"

#define PROTOCOL_HEADER_GOOME   0x67
#define PROTOCOL_HEADER_CONCOX   0x78   //len 1字节
#define PROTOCOL_HEADER_CONCOX_NEW   0x79   //len 2字节
#define PROTOCOL_HEADER_JT808   0x7e
#define PROTOCOL_HEADER_ESCAPE   0x7d
#define MAX_GPS_MSGS_PER_SEND  5
#define MAX_JT808_MSG_LEN 1024

/* msg flow:
   login ->    THEN    login_ack<-    THEN   iccid->
   heart <->
   gps or lbs or alarm ->   THEN   ack<-
   remote_msg <-  THEN    ack->

   (中文地址请求) 设备接收到123短信,上发到服务器,  收到时间经纬度和短信内容后, 通过短信发给源发送方
   
   only goome:
   alarm_ask <-  THEN     ack->

   only jt808:
   改imei号前,要logout
*/
void protocol_msg_receive(SocketType *socket);

GM_ERRCODE protocol_send_transprent_msg(SocketType *socket, char *json_str);
GM_ERRCODE protocol_send_login_msg(SocketType *socket);
GM_ERRCODE protocol_send_device_msg(SocketType *socket);

GM_ERRCODE protocol_send_logout_msg(SocketType *socket);
GM_ERRCODE protocol_send_heartbeat_msg(SocketType *socket);
U32 protocol_send_gps_msg(SocketType *socket);

GM_ERRCODE protocol_send_remote_ack(SocketType *socket, u8 *pRet, u16 retlen);
GM_ERRCODE protocol_send_general_ack(SocketType *socket);
GM_ERRCODE protocol_send_param_get_ack(SocketType *socket);

#endif



