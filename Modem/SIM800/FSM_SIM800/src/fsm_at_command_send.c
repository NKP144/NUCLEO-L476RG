/*
 * fsm_at_command_send.c
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#include "fsm_at_command_send.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

FSM_AT_SEND_STATE_e FSM_AT_SEND_STATE;
FSM_AT_SEND_STATE_e _FSM_AT_SEND_STATE;

FSM_AT_SEND_EVENTS_e FSM_AT_SEND_EVENT;

at_command_data at_command;
at_command_data *at_command_p = NULL;

static void start_AT_SEND_timer (void);
static FSM_AT_SEND_EVENTS_e at_send_get_event (void);
static void fsm_go_idle_state (void);



/*
 *
 */
void FSM_AT_SEND_init(void)
{
	FSM_AT_SEND_STATE = AT_SEND_IDLE_S;
	_FSM_AT_SEND_STATE = FSM_AT_SEND_STATE;
	ResetFSM_Timer (AT_COMMAND_TIMER);
}

/*
 *
 */
void FSM_AT_SEND (void)
{
	FSM_AT_SEND_EVENT = at_send_get_event ();

		switch (FSM_AT_SEND_STATE) {
		case AT_SEND_IDLE_S:
			switch (FSM_AT_SEND_EVENT)
			{
			case AT_SEND_IDLE_E:
				break;
			case AT_SEND_BEGIN_E: FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
								  _FSM_AT_SEND_STATE = AT_SEND_IDLE_S;
								  Show_Message_in_Debug ("\r\n#################\r\n", strlen("\r\n#################\r\n"));
								  Show_Message_in_Debug ("START UART\r\n", strlen("START UART\r\n"));
								  at_command_transmit_receive (&at_command);
								  start_AT_SEND_timer ();
				break;
			}
			break;

		case AT_WAIT_ANSWER_S:
			switch (FSM_AT_SEND_EVENT)
			{
			case AT_SEND_IDLE_E:      if (GetFSM_Timer(AT_COMMAND_TIMER) >= (at_command.timeout / 10))  // timeout в мс / 10 мс (период работы аппаратного таймера)
	  	  							  {
										// вышел тайм - аут, ничего не принято
							   	   	   	   	fsm_go_idle_state ();
							   	   	   	   	SendFSM_Broadcast_Messages (AT_COMMANT_NO_ANSWER);
							   	   	   	   	Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
				   	   	   	   	  			Show_Message_in_Debug ("TIMEOUT NO ANSWER\r\n", strlen("TIMEOUT NO ANSWER\r\n"));

				   	   	   	   	  			Show_Message_in_Debug ("\r\n!!!!!!!!!!!!!!!!!!\r\n", strlen("\r\n!!!!!!!!!!!!!!!!!!\r\n"));
				   	   	   	   				Show_Message_in_Debug ("STOP UART\r\n", strlen("START UART\r\n"));
	  	  							  }
				break;
			case AT_RECEIVE_IDLE_E:    answer_p = NULL;  // СООБЩЕНИЕ ПОЛУЧЕНО
									   if (at_compare_answer(modem_rx_buf,at_command.at_response, &answer_p) == AT_OK) // Совпало, выключить прерывания, завершить работу
									   {
										fsm_go_idle_state ();
										SendFSM_Broadcast_Messages (AT_COMMAND_OK);
										Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
										Show_Message_in_Debug ("IDLE RESPONSE OK\r\n", strlen("IDLE RESPONSE OK\r\n"));
										Show_Message_in_Debug (answer_p, strlen(answer_p));

										Show_Message_in_Debug ("\r\n!!!!!!!!!!!!!!!!!!\r\n", strlen("\r\n!!!!!!!!!!!!!!!!!!\r\n"));
										Show_Message_in_Debug ("STOP UART\r\n", strlen("START UART\r\n"));
									   }
									   else if (at_compare_answer(modem_rx_buf,"ERROR", &answer_p) == AT_OK)  // Если не совпало, проверить на ошибку
									   {
										 fsm_go_idle_state ();
										 SendFSM_Broadcast_Messages (AT_COMMAND_ERROR);
										 Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
										 Show_Message_in_Debug ("IDLE RESPONSE ERROR\r\n", strlen("IDLE RESPONSE ERROR\r\n"));
										 Show_Message_in_Debug (answer_p, strlen(answer_p));

										 Show_Message_in_Debug ("\r\n!!!!!!!!!!!!!!!!!!\r\n", strlen("\r\n!!!!!!!!!!!!!!!!!!\r\n"));
										 Show_Message_in_Debug ("STOP UART\r\n", strlen("START UART\r\n"));
									   }
									   else // Не ошибка, пришло что-то другое ждать дальше
									   {
										Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
										Show_Message_in_Debug ("IDLE NOT RESPONSE, WAIT\r\n", strlen("IDLE NOT RESPONSE, WAIT\r\n"));
										Show_Message_in_Debug (modem_rx_buf, strlen(modem_rx_buf));
									   }
				break;
			case AT_RECEIVE_TIMEOUT_E: answer_p = NULL;	// СООБЩЕНИЕ ПОЛУЧЕНО
			   	   	   	   	   	   	   if (at_compare_answer(modem_rx_buf,at_command.at_response, &answer_p) == AT_OK) // Совпало, выключить прерывания, завершить работу
			   	   	   	   	   	   	   {
			   	   	   	   	   	   		  fsm_go_idle_state ();
			   	   	   	   	   	   		  SendFSM_Broadcast_Messages (AT_COMMAND_OK);
			   	   	   	   	   	   		  Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
			   	   	   	   	   	   		  Show_Message_in_Debug ("TIMEOUT RESPONSE OK\r\n", strlen("\r\nTIMEOUT RESPONSE OK\r\n"));
			   	   	   	   	  			  Show_Message_in_Debug (answer_p, strlen(answer_p));

			   	   	   	   	  			  Show_Message_in_Debug ("\r\n!!!!!!!!!!!!!!!!!!\r\n", strlen("\r\n!!!!!!!!!!!!!!!!!!\r\n"));
			   	   	   	   	  			  Show_Message_in_Debug ("STOP UART\r\n", strlen("START UART\r\n"));
			   	   	   	   	   	   	   }
			   	   	   	   	   	   	   else if (at_compare_answer(modem_rx_buf,"ERROR", &answer_p) == AT_OK) // Если не совпало, проверить на ошибку
			   	   	   	   	   	   	   {
			   	   	   	   	   			  fsm_go_idle_state ();
			   	   	   	   	   			  SendFSM_Broadcast_Messages (AT_COMMAND_ERROR);

			   	   	   	   	   			  Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
			   	   	   	   	   			  Show_Message_in_Debug ("TIMEOUT RESPONSE ERROR\r\n", strlen("\r\nTIMEOUT RESPONSE ERROR\r\n"));
			   	   	   	   	   			  Show_Message_in_Debug (answer_p, strlen(answer_p));

			   	   	   	   	   			  Show_Message_in_Debug ("\r\n!!!!!!!!!!!!!!!!!!\r\n", strlen("\r\n!!!!!!!!!!!!!!!!!!\r\n"));
			   	   	   	   	 			  Show_Message_in_Debug ("STOP UART\r\n", strlen("START UART\r\n"));
			   	   	   	   	   	   	   }
			   	   	   	   	   	   	   else // Не ошибка, пришло что-то другое ждать дальше
			   	   	   	   	   	   	   {
			   	   	   	   	   	   		   //at_restart_dma_rx();
			   	   	   	   	   	   		   Show_Message_in_Debug ("\r\n------------------\r\n", strlen("\r\n------------------\r\n"));
			   	   	   	   	  			   Show_Message_in_Debug ("TIMEOUT NOT RESPONSE\r\n", strlen("\r\nTIMEOUT NOT RESPONSE\r\n"));
			   	   	   	   	  			   Show_Message_in_Debug (modem_rx_buf, strlen(modem_rx_buf));
			   	   	   	   	   	   	   }
			   	break;
			}
			break;
		}
}


static FSM_AT_SEND_EVENTS_e at_send_get_event (void)
{
	FSM_AT_SEND_EVENTS_e result = AT_SEND_IDLE_E;

	//(at_command_s = (at_command_data *)GetFSM_Param_Messages(SEND_AT_COMMAND))
	at_command_p = (at_command_data *)GetFSM_Param_Messages(SEND_AT_COMMAND);
	if (at_command_p != NULL)
	{
	 at_command.at_command = at_command_p->at_command;
	 at_command.at_response = at_command_p->at_response;
	 at_command.timeout = at_command_p->timeout;
	 at_command.usart_need_it = at_command_p->usart_need_it;
	 at_command.usart_need_rx_tx = at_command_p->usart_need_rx_tx;
	 free (at_command_p);

	 result = AT_SEND_BEGIN_E;
	}
	else if (GetFSM_Messages(SIM800_UART_IDLE_IT))
	{
	 result = AT_RECEIVE_IDLE_E;
	}
	else if (GetFSM_Messages(SIM800_UART_TIMEOUT_IT))
	{
	 result = AT_RECEIVE_TIMEOUT_E;
	}
	/*else result = BUZZER_IDLE;*/

	return result;
}

static void start_AT_SEND_timer (void)
{
	if (FSM_AT_SEND_STATE != _FSM_AT_SEND_STATE)
 	{
 		ResetFSM_Timer (AT_COMMAND_TIMER);
 		_FSM_AT_SEND_STATE = FSM_AT_SEND_STATE;
 	}
}

static void fsm_go_idle_state (void)
{
	at_uart_work_finish ();
	FSM_AT_SEND_STATE = AT_SEND_IDLE_S;
	_FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
}





