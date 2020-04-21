/*
 * at_command.c
 *
 *  Created on: 17 апр. 2020 г.
 *      Author: Konstantin
 */
#include "at_command.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

char *answer_p = NULL; // Указатель на ответ в буффере

/*  Функция возвращает указатель на структуру данных для отправления AT-команды модему
 *  после использования указателя, выделенную память нужно освободить функцией free();
 *
 */
at_command_data* fill_at_command_data (char* at_command, char* at_response, uint32_t timeout,
										usart_it_flag usart_need_it, usart_rx_tx_flag usart_need_rx_tx)
{
	at_command_data *const at_command_p = (at_command_data *) calloc (1, sizeof(at_command_data));

	if (at_command_p != NULL)
	{
		at_command_p->at_command = at_command;
		at_command_p->at_response = at_response;
		at_command_p->timeout = timeout;
		at_command_p->usart_need_it = usart_need_it;
		at_command_p->usart_need_rx_tx = usart_need_rx_tx;
	}

	return at_command_p;
}

/* Функция сравнивает ожидаемый овет с полученным ответом.
 *
 * Если есть совпадение, в передаваемый указатель **p записывается указатель
 * на первое вхождение искомой строки в буффер
 *
 * Возвращается статус сравннения
 */
at_compare_status at_compare_answer (char* answer_buf, char* expected_response, char **p)
{
	at_compare_status result;
	char *local_p;

	if (answer_buf[0] == '\0') // Если первый символ в сравниваемой строке = '\0', то возможно SIM800 выдал в ответ первым символом '\0' ошибочно и дальше идет нормальная посылка
	{
		answer_buf++;
	}

	local_p = strstr(answer_buf, expected_response);

	(*p) = local_p;

	if (local_p != NULL )  result = AT_OK; // Есть совпадение
	else result = AT_ERROR;

	return result;
}



//********************ОТПРАВЛЕНИЕ И ПРИЕМ ДАННЫХ ПО UART**************************//

/*  Активация ожидания ответа
 *  Отправка данных по UART
 *
 */
void at_command_transmit_receive (at_command_data* at_command)
{
	__HAL_UART_CLEAR_IDLEFLAG(&GSM_MODEM_HUART);
	__HAL_UART_CLEAR_FLAG(&GSM_MODEM_HUART, UART_CLEAR_RTOF);

	if ((at_command->usart_need_it & NEED_IDLE_IT) == NEED_IDLE_IT)
	{
		__HAL_UART_ENABLE_IT(&GSM_MODEM_HUART, UART_IT_IDLE);  // Enable serial port idle interrupt
		__HAL_UART_CLEAR_IDLEFLAG(&GSM_MODEM_HUART);
	}

	if ((at_command->usart_need_it & NEED_TIMEOUT_IT) == NEED_TIMEOUT_IT)
	{
		HAL_UART_ReceiverTimeout_Config(&GSM_MODEM_HUART, (GSM_RECEIVE_TIMEOUT * 1000) / 10); // timeout мс * 1000 мс / 10 мкс . 10 мкc = 1 TimeoutValue. 1/115200 = 8.68 ~ 10.0
		HAL_UART_EnableReceiverTimeout(&GSM_MODEM_HUART);
		__HAL_UART_ENABLE_IT(&GSM_MODEM_HUART, UART_IT_RTO);	  // Enable Receive Timeout interrupt*/
		__HAL_UART_CLEAR_FLAG(&GSM_MODEM_HUART, UART_CLEAR_RTOF);
	}

	if ((at_command->usart_need_rx_tx & NEED_RX) == NEED_RX)
	{
		HAL_UART_Receive_DMA(&GSM_MODEM_HUART, (uint8_t *) modem_rx_buf, RXBUFSIZE);
	}


	if((at_command->usart_need_rx_tx & NEED_TX) == NEED_TX)
	{
		Show_Message_in_Debug ("*******TX*******\r\n", strlen(at_command->at_command));
		Show_Message_in_Debug (at_command->at_command, strlen(at_command->at_command));
		Show_Message_in_Debug ("****************\r\n", strlen(at_command->at_command));
		HAL_UART_Transmit_DMA(&GSM_MODEM_HUART, (uint8_t *) at_command->at_command, strlen(at_command->at_command));
	}
}

/*  Функция возобновляет приём данных по UART, после возникновения события UART_RTO_IT
 *  Прием по UART завершается, т.к. прерывание UART_RTO_IT обрабатывается в stm32l4xx_it в HAL_UART_IRQHandler() как ошибка.
 */
void at_restart_dma_rx (void)
{
	HAL_UART_Receive_DMA(&GSM_MODEM_HUART, (uint8_t *) modem_rx_buf, RXBUFSIZE);
}

/*
 *
 */
void at_uart_work_finish (void)
{
   	__HAL_UART_DISABLE_IT(&GSM_MODEM_HUART, UART_IT_RTO);
	__HAL_UART_CLEAR_FLAG(&GSM_MODEM_HUART, UART_CLEAR_RTOF);

	__HAL_UART_DISABLE_IT(&GSM_MODEM_HUART, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&GSM_MODEM_HUART);

	if (HAL_UART_DMAStop(&GSM_MODEM_HUART) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_UART_DisableReceiverTimeout(&GSM_MODEM_HUART) != HAL_OK)
	{
		Error_Handler();
	}
}

/*
 *
 */
void USART_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	//Stop this DMA transmission
    //HAL_UART_DMAStop(&GSM_MODEM_HUART); //Не отстанавливать работу DMA, т.к. если пришла не ожидаемая посылка, необходимо дальше принимать данные
											// Если здесь пришла правильная посылка, то отключение DMA будет в функции завершения работы с UART at_uart_work_finish

    //Calculate the length of the received data
    uart_rx_counter  = RXBUFSIZE - __HAL_DMA_GET_COUNTER(&GSM_MODEM_DMA_RX);

	//Test function: Print out the received data
    char str[40];
    sprintf(str, "Receive Data(length = %d)%s", (int) uart_rx_counter, "\r\n");
    Show_Message_in_Debug (str, strlen(str));

    SendFSM_Messages (SIM800_UART_IDLE_IT);
}

/*
 *
 */
void USART_UART_RTOCallback(UART_HandleTypeDef *huart)
{
	//Stop this DMA transmission
	// HAL_UART_DMAStop(&GSM_MODEM_HUART);  // Не отключать DMA, т.к. если пришла не ожидаемая посылка, необходимо дальше принимать данные
											// Отключение приема данных в fsm_at_command_send в at_uart_work_finish ();

	 //Calculate the length of the received data
	 uart_rx_counter  = RXBUFSIZE - __HAL_DMA_GET_COUNTER(&GSM_MODEM_DMA_RX);

	 //Test function: Print out the received data
	 char str[40];
	 sprintf(str, "Receive Data(length = %d)%s", (int) uart_rx_counter, "\r\n");
	 Show_Message_in_Debug (str, strlen(str));

	 SendFSM_Messages (SIM800_UART_TIMEOUT_IT);

	 HAL_UART_Receive_DMA(&GSM_MODEM_HUART, (uint8_t *) modem_rx_buf, RXBUFSIZE); // Возобновить приём по UART через DMA
}

/* 	Эта функция используется в файле stm32l4xx_it.c
 *  Для обработки прерывания RX_IDLE, т.к. в библиотечной функции нет такого callback
 */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(GSM_MODEM_HUART.Instance == GSM_UART)                                   //Determine whether it is serial port 1
    {
        if(RESET != __HAL_UART_GET_FLAG(&GSM_MODEM_HUART, UART_FLAG_IDLE))   //Judging whether it is idle interruption
        {
        	//__HAL_UART_DISABLE_IT(&GSM_MODEM_HUART, UART_IT_IDLE);  // Disable serial port idle interrupt
        															// Отключение прерывания в fsm_at_command_send в at_uart_work_finish ();
            __HAL_UART_CLEAR_IDLEFLAG(&GSM_MODEM_HUART);            // Clear idle interrupt sign (otherwise it will continue to enter interrupt)
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_RTOF);
            Show_Message_in_Debug ("\r\nGSM_UART Idle IRQ Detected\r\n", strlen("\r\nGSM_UART Idle IRQ Detected\r\n"));

            USART_UART_IDLECallback(huart);                // Call interrupt handler
        }
    }
}

/*
 *
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	 if (huart->Instance == GSM_UART)
	 {

	    if (huart->ErrorCode == HAL_UART_ERROR_NONE)
	    {

		}
	    else if (huart->ErrorCode == HAL_UART_ERROR_RTO)
	    {
	    	__HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_RTOF);
	    	__HAL_UART_CLEAR_IDLEFLAG(huart);
	    	//__HAL_UART_DISABLE_IT(huart, UART_IT_RTO); // Не отключать прерывания, т.к. если пришла не ожидаемая посылка, необходимо дальше принимать данные
	    												 // Отключение прерывания в fsm_at_command_send в at_uart_work_finish ();

	    	 Show_Message_in_Debug ("\r\nGSM_UART Timeout IRQ Detected\r\n", strlen("\r\nGSM_UARTTimeout IRQ Detected\r\n"));

	    	 USART_UART_RTOCallback(huart);
	    }
		else
		{
			__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

			if (huart->ErrorCode == HAL_UART_ERROR_ORE)  	// Overrun error
			{
			__HAL_UART_CLEAR_OREFLAG(huart);
			}
			else if (huart->ErrorCode == HAL_UART_ERROR_PE) // Parity error
			{
			__HAL_UART_CLEAR_PEFLAG(huart);
			}
			else if (huart->ErrorCode == HAL_UART_ERROR_NE) // Noise error
			{
			__HAL_UART_CLEAR_NEFLAG(huart);
			}
			else if (huart->ErrorCode == HAL_UART_ERROR_FE) // Frame error
			{
			__HAL_UART_CLEAR_FEFLAG(huart);
			}
			else if (huart->ErrorCode == HAL_UART_ERROR_DMA) // DMA transfer error
			{

			}
			else if (huart->ErrorCode == 12)  // Oveerrun + Frame error
			{
			__HAL_UART_CLEAR_OREFLAG(huart);
			__HAL_UART_CLEAR_FEFLAG(huart);
			}
		}
	 }
}

