/*
 * uart_hal.c
 *
 *  Created on: 2018年2月3日
 *      Author: Zero
 */

/** @brief	UART interrupt function 模板 移植至stm32fxxx_it.c
  *
  *		void USART2_IRQHandler(void)
  *		{
  * 	#if UART_DMA
  * 		UART_DMARec_Callback(&huart2);
  * 	#else
  *			UART_TIRec_Callback(&huart2);
  * 	#endif
  *			HAL_UART_IRQHandler(&huart2);
  * 	}
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "uart_hal.h"

extern UART_HandleTypeDef huart2;
UartRecType Uart2;

#if UART_DMA

void UART_DMARec_Callback(UART_HandleTypeDef *huart)
{
	u32 temp;
	if(huart->Instance == USART2)
	{
		if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
		{
			__HAL_UART_CLEAR_IDLEFLAG(huart);
			HAL_UART_DMAStop(huart);
			temp = huart->hdmarx->Instance->CNDTR;
			Uart2.RecSize =  REC_SIZE - temp;
			Uart2.TimeCounter = 0;
			while(HAL_UART_Receive_DMA(huart, Uart2.RecBuff, REC_SIZE) != HAL_OK)
			{
				if(Uart2.TimeCounter++ > UART_MAX_TIME)
				{
					Uart2.UartFlg = true;
					break;
				}
			}
			Uart2.RecIntrFlg = true;
		}
	}
}

#else

void UART_TIRec_Callback(UART_HandleTypeDef *huart)
{
	if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		Uart2.RecIntrFlg = true;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		Uart2.RecBuff[Uart2.RecSize++] = Uart2.RecByte[0];
		Uart2.TimeCounter = 0;
		while(HAL_UART_Receive_IT(&huart2, Uart2.RecByte, 1) != HAL_OK)
		{
			if(Uart2.TimeCounter++ > UART_MAX_TIME)
			{
				Uart2.UartFlg = true;
				break;
			}
		}
	}
}

#endif

u16 Uart_Send(UART_HandleTypeDef *huart, u8 *buff, u16 size)
{
	if(HAL_UART_Transmit(huart, buff, size, 0xffff) != HAL_OK)
	{
		return false;
	}
	return size;
}

void Uart_Proc()
{
	if(Uart2.RecIntrFlg)
	{
		Uart2.RecIntrFlg = false;
//		HAL_UART_Transmit(&huart2, Uart2.RecBuff, Uart2.RecSize, 0xffff);
		Uart2.RecSize = 0;
	}
	if(Uart2.UartFlg)
	{
		Uart2.TimeCounter = 0;
		Uart2.UartFlg = false;
#if UART_DMA
		while(HAL_UART_Receive_DMA(&huart2, Uart2.RecBuff, REC_SIZE) != HAL_OK)
#else
		while(HAL_UART_Receive_IT(&huart2, Uart2.RecByte, 1) != HAL_OK)
#endif
		{
			if(Uart2.TimeCounter++ > UART_MAX_TIME)
			{
				Uart2.UartFlg = true;
				break;
			}
		}
	}
}
