#include "UART_RX_DMA.H"

//DMA空闲中断函数
void DMAInUART_IDLE(DMA_rxStr* pRx_str)
{
	if (__HAL_UART_GET_FLAG(pRx_str->huart, UART_FLAG_IDLE)) //IDLE中断。备注：IDLE中断标志的清除是读一次USART_SR寄存器，接着读一次USART_DR寄存器
	{

		if (__HAL_DMA_GET_COUNTER(pRx_str->hdma_usart_rx) <= pRx_str->length)
		{
			pRx_str->num = pRx_str->length - __HAL_DMA_GET_COUNTER(pRx_str->hdma_usart_rx);
			HAL_UART_AbortReceive(pRx_str->huart);				// huart->RxState = HAL_UART_STATE_READY，并HAL_DMA_Abort(hdma_usart_rx)
			__HAL_DMA_SET_COUNTER(pRx_str->hdma_usart_rx, pRx_str->length); // 这个寄存器只能在通道不工作(DMA_CCRx的EN=0)时写入  
		}

		__HAL_UART_CLEAR_IDLEFLAG(pRx_str->huart);
		HAL_UART_Receive_DMA(pRx_str->huart, pRx_str->buf, pRx_str->length);
	}

}

/* 串口接收中断中调用的函数 */
void UsartInIT_IDLE(DMA_rxStr* pRx_str)
{
	DMAInUART_IDLE(pRx_str);
}

/**
		配置并启动DMA
*/
void UartRX_Start(UART_HandleTypeDef* huart, DMA_HandleTypeDef* hdma_usart_rx, DMA_rxStr* dmaStr)
{
	dmaStr->length = RXBUF_LEN;	
	dmaStr->callback_IDLE = UsartInIT_IDLE;
	dmaStr->huart = huart;
	dmaStr->hdma_usart_rx = hdma_usart_rx;

	HAL_UART_Receive_DMA(dmaStr->huart, dmaStr->buf, dmaStr->length);
	__HAL_UART_ENABLE_IT(dmaStr->huart, UART_IT_IDLE);						// 开启空闲接收中断 
	__HAL_UART_CLEAR_IDLEFLAG(dmaStr->huart); 								// 清空闲中断

}


