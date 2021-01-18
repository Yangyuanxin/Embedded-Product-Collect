/**
  ******************************************************************************
  * @file    tcp_echoclient.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   tcp echoclient application using LwIP RAW API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "tcp_echoclient.h"
#include "main.h"
#include "param.h"


#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"

//#include "main.h"
#include <stdio.h>
#include <string.h>

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

u8_t  recev_buf[50];
__IO uint32_t message_count=0;

//u8_t   data[100] = "123456";

struct tcp_pcb *echoclient_pcb;
TCP_USER_T g_tClient;        /* TCP用户结构体 2016-09-09 by xd*/
uint8_t g_fTcpState;

/* Private function prototypes -----------------------------------------------*/
static err_t tcp_echoclient_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
//void tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_echoclient_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoclient_send(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_echoclient_connected(void *arg, struct tcp_pcb *tpcb, err_t err);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  Connects to the TCP echo server
* @param  None
* @retval None
*/
void tcp_echoclient_connect(void)
{
  ip_addr_t DestIPaddr;
  
  /* create new tcp pcb */
  echoclient_pcb = tcp_new();        /* 创建新的PCB */
  
  if (echoclient_pcb != NULL)
  {
    //IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );    /* 设置目的IP地址 */
    IP4_ADDR( &DestIPaddr, g_tParam.RemoteIPAddr[0], g_tParam.RemoteIPAddr[1], g_tParam.RemoteIPAddr[2], g_tParam.RemoteIPAddr[3]);    /* 设置目的IP地址 */
    
    /* connect to destination address/port */
    tcp_connect(echoclient_pcb, &DestIPaddr, g_tParam.RemoteTcpPort, tcp_echoclient_connected);        /* 连接到目的地址的指定端口上,连接成功后回调tcp_client_connected()函数 */
  }
  else
  {
    /* deallocate the pcb */
    memp_free(MEMP_TCP_PCB, echoclient_pcb);
#ifdef SERIAL_DEBUG
    printf("\n\r can not create tcp pcb");
#endif 
  }
}


/*
*********************************************************************************************************
*    函 数 名: tcp_echoclient_connected
*    功能说明: LwIP TCP连接建立后调用的回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK 
  * @retval err_t: returned error 
  */
static err_t tcp_echoclient_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  struct echoclient *es = NULL;
  
  if (err == ERR_OK)   
  {
    /* allocate structure es to maintain tcp connection informations */
    es = (struct echoclient *)mem_malloc(sizeof(struct echoclient));    /* 分配内存 */
  
    if (es != NULL)
    {
      es->state = ES_CONNECTED;        /* 状态为连接成功 */
        
        g_tClient.TcpState = 1;        /* 连接成功 */
        
      es->pcb = tpcb;
      es->p_tx = NULL;            /* 2016-09-05 xd 增加。连接时，清空数据缓冲区 */
      //sprintf((char*)data, "sending tcp client message %d", message_count);
        
      /* allocate pbuf */
      //es->p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)data) , PBUF_POOL);
         
      //if (es->p_tx)
      {       
        /* copy data to pbuf */
        //pbuf_take(es->p_tx, (char*)data, strlen((char*)data));
        
        /* pass newly allocated es structure as argument to tpcb */
        tcp_arg(tpcb, es);                        /* 使用es更新tpcb的callback arg */
  
        /* initialize LwIP tcp_recv callback function */ 
        tcp_recv(tpcb, tcp_echoclient_recv);    /* 初始化LwIP接收回调函数 */
  
        /* initialize LwIP tcp_sent callback function */
        tcp_sent(tpcb, tcp_echoclient_sent);    /* 初始化LwIP发送回调函数 */
  
        /* initialize LwIP tcp_poll callback function */
        tcp_poll(tpcb, tcp_echoclient_poll, 1);    /* 初始化LwIP的tcp_poll回调函数 */
    
        /* send data */
        //tcp_echoclient_send(tpcb,es);        
        
        return ERR_OK;
      }
    }
    else
    {
      /* close connection */
      tcp_echoclient_connection_close(tpcb, es);    /* 关闭连接 */
      
      /* return memory allocation error */
      return ERR_MEM;              /* 返回内存分配错误 */
    }
  }
  else
  {
    /* close connection */
    tcp_echoclient_connection_close(tpcb, es);        /* 关闭连接 */
  }
  
  g_tClient.TcpState = 0;        /* 连接失败 */ 
  return err;
}
    
/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback 
  * @param tpcb: tcp connection control block 
  * @param err: receive error code 
  * @retval err_t: retuned error  
  */
static err_t tcp_echoclient_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
  struct echoclient *es;
  err_t ret_err;
  struct pbuf *q;
    uint32_t RxPos = 0;        /* 当前接收数据的位置 */

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct echoclient *)arg;
  
  /* if we receive an empty tcp frame from server => close connection */
  if (p == NULL)            /* 如果从服务器接受到空的TCP包 -> 关闭连接 */
  { 
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p_tx == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoclient_connection_close(tpcb, es);
    }
    else
    {    
      /* send remaining data*/
      //tcp_echoclient_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
  else if(err != ERR_OK)        /* 否则：接收到非空的数据，但是由于某些原因，错误 */
  {
    /* free received pbuf*/
    pbuf_free(p);        /* 释放接受pbuf */

    ret_err = err;
  }
  else if(es->state == ES_CONNECTED)    /* 连接状态，并且接受到正确的数据包 */
  {
    /* increment message count */
    message_count++;        /* 接受到数据的数量 */
      
    if (p != NULL)
    {
        memset(g_tClient.RxData, 0, TCP_Rx_SIZE);        /* 清空接收缓冲区 */
        
        for(q = p;q != NULL;q = q->next)     /* 遍历整个pbuf链表 */
        {
            if (q->len > (TCP_Rx_SIZE - RxPos))        /* 判断数据长度，如果大于接收buf，则应答错误 */
            {
                break;        /* 数据长度过大，应答错误 */
            }
            else            /* 将数据存入buf */
            {
                memcpy(g_tClient.RxData + RxPos, q->payload, q->len);
            }
            RxPos += q->len;          /* 接收到的数据长度 */
        }

        //strcpy((char*)g_tClient.TxData, (char*)g_tClient.RxData);    
        
        g_tClient.TcpState = 2;        /* 接收到数据 */
        
        /* Acknowledge data reception */
        tcp_recved(tpcb, p->tot_len);      /* 获取接收数据，通知LwIP可以获取更多数据 */

        pbuf_free(p);        /* 数据已放入接收缓冲区,可以释放内存了 */
        //tcp_echoclient_connection_close(tpcb, es);    /* 这里不需要断开连接吧 */
        ret_err = ERR_OK;
    }
  }

  /* data received when connection already closed */
  else    /* 当连接已经关闭，接受到数据 */
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    es->p_tx=NULL;
    /* free pbuf and do nothing */
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}


/*
*********************************************************************************************************
*    函 数 名: tcp_client_usersent
*    功能说明: 用户发送数据，2016-09-08 by xd
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void tcp_client_usersent(struct tcp_pcb *_tpcb)
{
    struct echoclient *es = NULL;
    es =_tpcb->callback_arg;
    
    if(es != NULL)  /* 连接处于空闲可以发数据 */
    {
        es->p_tx = pbuf_alloc(PBUF_TRANSPORT, g_tClient.TxCount , PBUF_POOL);        /* 分配内存，内存的大小就是数据长度ptr->len */
        
         /* copy data to pbuf */
        pbuf_take(es->p_tx, (char*)g_tClient.TxData, g_tClient.TxCount);        /* 将data[]中的数据复制到 es->p_tx */
        
        tcp_echoclient_send(_tpcb, es);
        
        if (es->p_tx)
        {
            pbuf_free(es->p_tx);    /* 释放内存 */
        }
    }
}

/**
  * @brief function used to send data
  * @param  tpcb: tcp control block
  * @param  es: pointer on structure of type echoclient containing info on data 
  *             to be sent
  * @retval None 
  */
static void tcp_echoclient_send(struct tcp_pcb *tpcb, struct echoclient * es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p_tx != NULL) && 
         (es->p_tx->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p_tx;
      
    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK)
    { 
      /* continue with next pbuf in chain (if any) */
      es->p_tx = ptr->next;
      
      if(es->p_tx != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p_tx);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later, defer to poll */
     es->p_tx = ptr;
   }
   else
   {
     /* other problem ?? */
   }
   
       /* 2016-09-08 by xd 立刻发送 */
    tcp_output(tpcb);                /* 将数据立刻发送出去 */
   
  }
}

/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t tcp_echoclient_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct echoclient *es;

  es = (struct echoclient*)arg;
  if (es != NULL)
  {
    if (es->p_tx != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoclient_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /* close tcp connection */
        tcp_echoclient_connection_close(tpcb, es);
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
  * @param  arg: pointer on argument passed to callback
  * @param  tcp_pcb: tcp connection control block
  * @param  len: length of data sent 
  * @retval err_t: returned error code
  */
static err_t tcp_echoclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct echoclient *es;

  LWIP_UNUSED_ARG(len);

  es = (struct echoclient *)arg;
  
  if(es->p_tx != NULL)
  {
    /* still got pbufs to send */
    tcp_echoclient_send(tpcb, es);
  }

  return ERR_OK;
}


/*
*********************************************************************************************************
*    函 数 名: tcp_echoclient_connection_close
*    功能说明: 关闭与服务器的连接
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on echoclient structure
  * @retval None
  */
void tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es )
{
  /* remove callbacks */
  tcp_recv(tpcb, NULL);        
  tcp_sent(tpcb, NULL);
  tcp_poll(tpcb, NULL,0);

  if (es != NULL)
  {
    mem_free(es);
  }

  /* close tcp connection */
  tcp_close(tpcb);
  
/* 以下4个函数 2016-09-19 by xd */
  g_tClient.TcpState = 0;    /* 断开连接 */
  tcp_abort(tpcb);            /* 终止tcp控制块 */
  tcp_arg(tpcb,NULL); 
  tcp_err(tpcb,NULL);
}

#endif /* LWIP_TCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
