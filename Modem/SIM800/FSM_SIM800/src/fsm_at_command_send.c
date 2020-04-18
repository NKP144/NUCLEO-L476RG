/*
 * fsm_at_command_send.c
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#include "fsm_at_command_send.h"
#include "stdlib.h"
#include "stdio.h"

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
			case AT_SEND_BEGIN_E: FSM_AT_SEND_STATE = AT_SEND_START_S;
								  _FSM_AT_SEND_STATE = AT_SEND_IDLE_S;
				break;
			}
			break;

		case AT_SEND_START_S: at_command_transmit_receive (&at_command);
							  FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
							  _FSM_AT_SEND_STATE = AT_SEND_START_S;
			break;

		case AT_WAIT_ANSWER_S:
			switch (FSM_AT_SEND_EVENT)
			{
			case AT_SEND_IDLE_E:
				break;
			case AT_RECEIVE_IDLE_E:    answer_p = NULL;
									   if (at_compare_answer(modem_rx_buf,at_command.at_response, &answer_p) == AT_OK) // Совпало, выключить прерывания, завершить работу
									   {
										FSM_AT_SEND_STATE = AT_RECEIVE_OK_S;
										_FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
										printf ("IDLE RESPONSE OK \r\n");
									   }
									   else if (at_compare_answer(modem_rx_buf,"ERROR", &answer_p) == AT_OK)  // Если не совпало, проверить на ошибку
									   {
										   FSM_AT_SEND_STATE = AT_RECEIVE_ERROR_S;
										   _FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
										printf ("IDLE RESPONSE ERROR \r\n");
									   }
									   else // Не ошибка, пришло что-то другое ждать дальше пока не сработает receive timeout
									   {
										printf ("IDLE NOT RESPONSE, WAIT \r\n");
									   }
				break;
			case AT_RECEIVE_TIMEOUT_E: answer_p = NULL;
			   	   	   	   	   	   	   if (at_compare_answer(modem_rx_buf,at_command.at_response, &answer_p) == AT_OK) // Совпало, выключить прерывания, завершить работу
			   	   	   	   	   	   	   {
			   	   	   	   	   	   		  FSM_AT_SEND_STATE = AT_RECEIVE_OK_S;
			   	   	   	   	   	   		  _FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
			   	   	   	   	   	   		  printf ("TIMEOUT RESPONSE OK \r\n");
			   	   	   	   	   	   	   }
			   	   	   	   	   	   	   else if (at_compare_answer(modem_rx_buf,"ERROR", &answer_p) == AT_OK) // Если не совпало, проверить на ошибку
			   	   	   	   	   	   	   {
			   	   	   	   	   	   		   FSM_AT_SEND_STATE = AT_RECEIVE_ERROR_S;
			   	   	   	   	   			   _FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
			   	   	   	   	   			  printf ("TIMEOUT RESPONSE ERROR \r\n");
			   	   	   	   	   	   	   }
			   	   	   	   	   	   	   else // Не ошибка, ответ не пришёл вовремя
			   	   	   	   	   	   	   {
			   	   	   	   	   	   		   FSM_AT_SEND_STATE = AT_NOT_RECEIVED_S;
			   	   	   	   	  			   _FSM_AT_SEND_STATE = AT_WAIT_ANSWER_S;
			   	   	   	   	  			   printf ("TIMEOUT NO ANSWER \r\n");
			   	   	   	   	   	   	   }
			   	break;
			}
			break;

		case AT_RECEIVE_OK_S:		fsm_go_idle_state ();
									SendFSM_Messages (AT_COMMAND_OK);
			break;

		case AT_RECEIVE_ERROR_S:	fsm_go_idle_state ();
									SendFSM_Messages (AT_COMMAND_ERROR);
			break;

		case AT_NOT_RECEIVED_S:		fsm_go_idle_state ();
									SendFSM_Messages (AT_COMMANT_NO_ANSWER);
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
	_FSM_AT_SEND_STATE = AT_RECEIVE_OK_S;
}





