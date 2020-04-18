/*
 * at_command.h
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#ifndef SIM800_INC_AT_COMMAND_H_
#define SIM800_INC_AT_COMMAND_H_

 /* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "usart.h"
#include "dma.h"
#include "FSM_Messages.h"

#define	GSM_MODEM_HUART		huart4
#define GSM_MODEM_DMA_RX	hdma_uart4_rx
#define GSM_MODEM_DMA_TX	hdma_uart4_tx
#define GSM_UART			UART4

/*  Флаг необходимоти включения прерывания по приему данных
 *
 */
typedef enum
{
	NO_ADDITION_IT = 0x00,
	NEED_IDLE_IT,
	NEED_TIMEOUT_IT
} usart_it_flag;

/* Статус сравнения ожидаемого ответа и полученного фактического ответа
 *
 */
typedef enum
{
  AT_OK    = (0x00),
  AT_ERROR = (0x01)
} at_compare_status;

typedef struct
{
	char* at_command;
	char* at_response;
	uint32_t timeout;
	usart_it_flag usart_need_it;
} at_command_data;

extern char *answer_p; // Указатель на ответ в буффере

at_command_data* fill_at_command_data (char* at_command, char* at_response, uint32_t timeout,
										usart_it_flag usart_need_it);

void at_command_transmit_receive (at_command_data* at_command);

at_compare_status at_compare_answer (char* answer_buf, char* expected_response, char **p);

void at_uart_work_finish (void);


#endif /* SIM800_INC_AT_COMMAND_H_ */
