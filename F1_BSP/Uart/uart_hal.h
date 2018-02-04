/*
 * uart_hal.h
 *
 *  Created on: 2018年2月3日
 *      Author: Zero
 */

#ifndef STM32F1XX_HAL_DRIVER_INC_UART_HAL_H_
#define STM32F1XX_HAL_DRIVER_INC_UART_HAL_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "data_type.h"

#define UART_DMA	0
#define	REC_SIZE	512
#define UART_MAX_TIME	0xFFFE

typedef struct
{
#if !UART_DMA
	u8		RecByte[1];
#endif
	u8		RecBuff[REC_SIZE];
	u16		TimeCounter;
	u16		RecSize;
	bool	RecIntrFlg;
	bool	UartFlg;
}UartRecType;

#if UART_DMA
	void UART_DMARec_Callback(UART_HandleTypeDef *huart);
#else
	void UART_TIRec_Callback(UART_HandleTypeDef *huart);
#endif

void Uart_Proc(void);
u16	 Uart_Send(UART_HandleTypeDef *huart, u8 *buff, u16 size);

#ifdef __cplusplus
}
#endif

#endif /* STM32F1XX_HAL_DRIVER_INC_UART_HAL_H_ */
