/*
 * fsm_sim800_init.c
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#include "fsm_sim800_init.h"
#include "fsm_at_command_send.h"

#include "stdlib.h"
#include "string.h"

FSM_SIM800_INIT_STATE_e FSM_SIM800_INIT_STATE;
FSM_SIM800_INIT_STATE_e _FSM_SIM800_INIT_STATE;

FSM_SIM800_INIT_EVENTS_e FSM_SIM800_INIT_EVENT;

static void start_SIM800_INIT_timer (void);
static FSM_SIM800_INIT_EVENTS_e sim800_init_get_event (void);
static void clear_rx_buf (void);

/*
 *
 */
void FSM_SIM800_Initialize_init(void)
{
	FSM_SIM800_INIT_STATE = SIM800_INIT_IDLE_S;
	_FSM_SIM800_INIT_STATE = FSM_SIM800_INIT_STATE;
	ResetFSM_Timer (SIM800_TIMER);
}

/*
 *
 */
void FSM_SIM800_Initialize (void)
{
	FSM_SIM800_INIT_EVENT = sim800_init_get_event ();

		switch (FSM_SIM800_INIT_STATE) {
		case SIM800_INIT_IDLE_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E:
				break;
			case SIM800_START_INIT_E: FSM_SIM800_INIT_STATE = SIM800_WAIT_RDY_S;
									  _FSM_SIM800_INIT_STATE = SIM800_INIT_IDLE_S;
									  SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("", "RDY", RDY_TIMEOUT, NEED_TIMEOUT_IT , NEED_RX));
				break;
			}
			break;

		case SIM800_WAIT_RDY_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();
							break;
			case SIM800_AT_RECEIVE_OK_E:		FSM_SIM800_INIT_STATE = SIM800_WAIT_CLTS_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_RDY_S;
												clear_rx_buf ();
												SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT+CLTS=1\r\n", "OK", REGULAR_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX | NEED_RX));
							break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:   FSM_SIM800_INIT_STATE = SIM800_WAIT_AT_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_RDY_S;
												SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT\r\n", "OK", REGULAR_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX | NEED_RX));
							break;
			case SIM800_AT_RECEIVE_ERROR_E:		FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_RDY_S;
							break;
			}
			break;


		case SIM800_WAIT_AT_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();
				break;
			case SIM800_AT_RECEIVE_OK_E:		FSM_SIM800_INIT_STATE = SIM800_WAIT_IPR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_AT_S;
												clear_rx_buf();
												SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT+IPR=115200\r\n", "OK", REGULAR_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX | NEED_RX));
				break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:   FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_AT_S;
				break;
			case SIM800_AT_RECEIVE_ERROR_E: 	FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_AT_S;
				break;
			}
			break;

		case SIM800_WAIT_IPR_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();
				break;
			case SIM800_AT_RECEIVE_OK_E:		FSM_SIM800_INIT_STATE = SIM800_WAIT_W_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_IPR_S;
												clear_rx_buf();
												SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT&W\r\n", "OK", ATW_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX | NEED_RX));
				break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:	FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_IPR_S;
				break;
			case SIM800_AT_RECEIVE_ERROR_E:		FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_IPR_S;
				break;
			}

			break;

		case SIM800_WAIT_W_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();
				break;
			case SIM800_AT_RECEIVE_OK_E:		FSM_SIM800_INIT_STATE = SIM800_WAIT_CLTS_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_W_S;
												clear_rx_buf();
												SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT+CLTS=1\r\n", "OK", REGULAR_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX | NEED_RX));
				break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:	FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_W_S;
				break;
			case SIM800_AT_RECEIVE_ERROR_E:		FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_W_S;
				break;
			}
			break;

		case SIM800_WAIT_CLTS_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();
				break;
			case SIM800_AT_RECEIVE_OK_E:		FSM_SIM800_INIT_STATE = SIM800_WAIT_SMS_READY_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_CLTS_S;
												clear_rx_buf();
												SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("", "SMS Ready", SMS_READY_TIMEOUT, NEED_TIMEOUT_IT, NEED_RX));
				break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:	FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_CLTS_S;
				break;
			case SIM800_AT_RECEIVE_ERROR_E:		FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_CLTS_S;
				break;
			}
			break;

		case SIM800_WAIT_SMS_READY_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();
				break;
			case SIM800_AT_RECEIVE_OK_E:		FSM_SIM800_INIT_STATE = SIM800_INIT_FINISH_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_SMS_READY_S;
												clear_rx_buf();
				break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:	FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_SMS_READY_S;
				break;
			case SIM800_AT_RECEIVE_ERROR_E:		FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
												_FSM_SIM800_INIT_STATE = SIM800_WAIT_SMS_READY_S;
				break;
			}
			break;

		case SIM800_INIT_ERROR_S:	SendFSM_Messages (SIM800_INIT_ERROR);
									FSM_SIM800_INIT_STATE = SIM800_INIT_IDLE_S;
									_FSM_SIM800_INIT_STATE = SIM800_INIT_ERROR_S;
			break;

		case SIM800_INIT_FINISH_S:	SendFSM_Messages (SIM800_INIT_FINISH);
									FSM_SIM800_INIT_STATE = SIM800_INIT_IDLE_S;
									_FSM_SIM800_INIT_STATE = SIM800_INIT_FINISH_S;
			break;
		}
}


/*
 *
 */
static FSM_SIM800_INIT_EVENTS_e sim800_init_get_event (void)
{
	FSM_SIM800_INIT_EVENTS_e result = SIM800_IDLE_INIT_E;

	if (GetFSM_Messages(SIM800_INIT_BEGIN))
	{
	 result = SIM800_START_INIT_E;
	}
	else if (GetFSM_Broadcast_Messages(AT_COMMAND_OK))
	{
	 result = SIM800_AT_RECEIVE_OK_E;
	}
	else if (GetFSM_Broadcast_Messages(AT_COMMAND_ERROR))
	{
	result = SIM800_AT_RECEIVE_ERROR_E;
	}
	else if (GetFSM_Broadcast_Messages(AT_COMMANT_NO_ANSWER))
	{
	result = SIM800_AT_RECEIVE_TIMEOUT_E;
	}
	/*else result = BUZZER_IDLE;*/

	return result;
}

/*
 *
 */
static void start_SIM800_INIT_timer (void)
 {
 	if (FSM_SIM800_INIT_STATE != _FSM_SIM800_INIT_STATE)
 	{
 		ResetFSM_Timer (SIM800_TIMER);
 		_FSM_SIM800_INIT_STATE = FSM_SIM800_INIT_STATE;
 	}
 }

/*
 *
 */
static void clear_rx_buf (void)
{
	memset (modem_rx_buf, '\0', uart_rx_counter);
}
