/*
*********************************************************************************************************
*
*    此文件为用户自己编写，这里用到ST官方例程相关代码。可以根据不同的需求修改此文件内容。2016-08-26  xd 
*
*********************************************************************************************************
*/

/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Credits go to Adam Dunkels (and the current maintainers) of this software.
 *
 * Christiaan Simons rewrote this file to get a more stable echo example.
 *
 **/

 /* This file was modified by ST */


#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "bsp.h"
#include "modbus_slave.h"    /* tcp + modbus协议 */
#include "param.h"

#if LWIP_TCP

static struct tcp_pcb *tcp_echoserver_pcb;

/* ECHO protocol states */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* structure for maintaing connection infos to be passed as argument 
   to LwIP callbacks*/
struct tcp_echoserver_struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};


static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);

/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
void tcp_echoserver_init(void)
{
  /* create new tcp pcb */
  tcp_echoserver_pcb = tcp_new();
    
  if (tcp_echoserver_pcb != NULL)
  {
    err_t err;
    
    /* bind echo_pcb to port 7 (ECHO protocol) */
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, g_tParam.LocalTCPPort);        /* 端口号为g_tParam.Ctrl_Port */
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb */
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
      
      /* initialize LwIP tcp_accept callback function */
      tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);    /* 初始化接收回调函数 */
    }
    else 
    {
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
      //printf("Can not bind pcb\n");
    }
  }
  else
  {
    //printf("Can not create new pcb\n");
  }
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used 
  * @retval err_t: error status
  */
/* 客户端连接时，会调用此回调函数，将es->state = ES_ACCEPTED 表示连接状态 */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* allocate structure es to maintain tcp connection informations */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    
    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, es);
    
    /* initialize lwip tcp_recv callback function for newpcb  */ 
    tcp_recv(newpcb, tcp_echoserver_recv);        /* 初始化数据接收回调函数 */
    
    /* initialize lwip tcp_err callback function for newpcb  */
    tcp_err(newpcb, tcp_echoserver_error);        /* 初始化错误回调函数 */
    
    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, tcp_echoserver_poll, 1);    /* tcp_poll回调函数 */
    
    ret_err = ERR_OK;
  }
  else
  {
    /*  close tcp connection */
    tcp_echoserver_connection_close(newpcb, es);
    /* return memory error */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct tcp_echoserver_struct *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_echoserver_struct *)arg;
  
//        DebugToggle1();
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)                    /* 从客户端接收到空数据 */
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoserver_connection_close(tpcb, es);
    }
    else
    {
    #if 0                            /* 既然还没完成，此部分代码也没什么用，所以去除该部分 */
      /* we're not done yet */
      /* acknowledge received packet */
      tcp_sent(tpcb, tcp_echoserver_sent);        
      
      /* send remaining data*/
      tcp_echoserver_send(tpcb, es);
    #endif
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)            /* 从客户端接收到一个非空的数据,但是由于某种原因err!=ERR_OK */
  {
    /* free received pbuf*/
    es->p = NULL;
    pbuf_free(p);
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)            /* 链接状态，第一次收到数据会进入此分支，之后会在es->state == ES_RECEIVED分支 */
  {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(tpcb, tcp_echoserver_sent);        /* 初始化Lwip协议栈tcp_sent的回调函数，此函数可以放在tcp_echoserver_accept中一起做 */
    
    /* send back the received data (echo) */
    tcp_echoserver_send(tpcb, es);    /* ST官方例程接收到数据立刻发送出去，所以删除这部分代码 */
    
    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {
    /* more data received from client and previous data has been already sent*/
    if(es->p == NULL)
    {
//        DebugToggle2();
      es->p = p;
  
      /* send back received data */
      tcp_echoserver_send(tpcb, es);    /* ST官方例程接收到数据立刻发送出去，所以删除这部分代码 */
    }
    else
    {
      struct pbuf *ptr;
        
      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }
  
  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs. 
  * @param  arg: pointer on argument parameter 
  * @param  err: not used
  * @retval None
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      //tcp_echoserver_send(tpcb, es);            /* 此时不能发送任何数据 */
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  None
  * @retval None
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}


/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
extern int32_t g_LastTime;
//int32_t g_NowTime;
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
    
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;
      
    #if 1        /* 默认数据包就在一个结构体中，没有用到链表解析 xd 2016-08-27 */
      {
//          uint8_t *p;
          bsp_LedToggle(1);        /* 信号指示灯翻转 */
        
//          p = ptr->payload;
         #if 1
            if (ptr->len > 6)
            {
                uint16_t i;
                memcpy(g_tModS.TCP_Head, ptr->payload, 6);
                g_tModS.TCP_Flag = 1;
                MODS_Poll((uint8_t *)((uint8_t *)ptr->payload + 6), ptr->len - 6);
                //MODBUS_Poll(ptr->payload, ptr->len);
                for (i = 0; i < g_tModS.TxCount; i++)
                {
                    g_tModS.TxBuf[g_tModS.TxCount - i - 1 + 6] = g_tModS.TxBuf[g_tModS.TxCount - i - 1];
                }
                memcpy(g_tModS.TxBuf, g_tModS.TCP_Head, 6);
                g_tModS.TxCount -= 2;
                g_tModS.TxBuf[4] = g_tModS.TxCount >> 8;
                g_tModS.TxBuf[5] = g_tModS.TxCount;
                wr_err = tcp_write(tpcb, g_tModS.TxBuf, g_tModS.TxCount + 6, 1); 
            }
        #else
            MODBUS_Poll(ptr->payload, ptr->len);
            wr_err = tcp_write(tpcb, g_tModS.TxBuf, g_tModS.TxCount, 1); 
        #endif
      } 
    #else        /* 此分支为解析链表数据，并发送接收的数据,st官方例程代码 */
        /* enqueue data for transmission */
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);    /* pbuf为数据结构体，ptr->payload为数据缓冲区 */
    #endif

    if (wr_err == ERR_OK)
    {
        u16_t plen;

        plen = ptr->len;

        /* continue with next pbuf in chain (if any) */
        es->p = ptr->next;                            /* 指向下一个pbuf */

        if(es->p != NULL)
        {
        /* increment reference count for es->p */
        pbuf_ref(es->p);                            /* pbuf的ref加1，表示用到次数（Lwip协议栈源码） */
        }

        /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
        pbuf_free(ptr);

        /* Update tcp window size to be advertized : should be called when received
        data (with the amount plen) has been processed by the application layer */
        tcp_recved(tpcb, plen);            /* 更新tcp窗口大小 */
    }
    else if(wr_err == ERR_MEM)
    {
        /* we are low on memory, try later / harder, defer to poll */
        es->p = ptr;
    }
     else
   {
     /* other problem ?? */
       uint8_t a;
       a = a;
       
   }
       /* 
        上面用tcp_write函数将数据写入tpcb，但此时数据并没有发送出去。
        而是在tcp_input函数调用tcp_output(tpcb)数据才发送出去。
        如果要增加发送速度，可以在这里调用tcp_output(tpcb);
    */
  }
  
    /* 2016-09-13 by xd 立刻发送 */
    tcp_output(tpcb);                /* 将数据立刻发送出去 */
}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
    mem_free(es);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);
}

#endif /* LWIP_TCP */
