/*
 * fsm_sim800_init.c
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#include "fsm_sim800_init.h"
#include "fsm_at_command_send.h"

#include "stdlib.h"

FSM_SIM800_INIT_STATE_e FSM_SIM800_INIT_STATE;
FSM_SIM800_INIT_STATE_e _FSM_SIM800_INIT_STATE;

FSM_SIM800_INIT_EVENTS_e FSM_SIM800_INIT_EVENT;

static void start_SIM800_INIT_timer (void);
static FSM_SIM800_INIT_EVENTS_e sim800_init_get_event (void);


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
			case SIM800_START_INIT_E: FSM_SIM800_INIT_STATE = SIM800_AT_SEND_S;
									  _FSM_SIM800_INIT_STATE = SIM800_INIT_IDLE_S;
				break;
			}
			break;

		case SIM800_AT_SEND_S: ;    //at_command_data *const at_at_command = fill_at_command_data ("AT\r\n", "OK\r\n", 100);
									/*at_at_command = (at_command_data *) calloc (1, sizeof(at_command_data));
									at_at_command->at_command = "AT\r\n";
									at_at_command->at_response = "OK\r\n";
									at_at_command->timeout = 100;*/

									SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT\r\n", "\r\nOK\r\n", 50, NEED_TIMEOUT_IT));

									FSM_SIM800_INIT_STATE = SIM800_WAIT_AT_ANSWER_S;
									_FSM_SIM800_INIT_STATE = SIM800_AT_SEND_S;
			break;

		case SIM800_WAIT_AT_ANSWER_S:
			switch (FSM_SIM800_INIT_EVENT)
			{
			case SIM800_IDLE_INIT_E: 			FSM_AT_SEND ();/*TODO Использовать вложенный автомат отправки/приема at-команды */
				break;
			case SIM800_AT_RECEIVE_OK_E:
				break;
			case SIM800_AT_RECEIVE_TIMEOUT_E:
				break;
			case SIM800_AT_RECEIVE_ERROR_E:
				break;
			}
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
	/* else if (GetFSM_Messages(BUZZER_START_5_SEC))
	{
	 result = BUZZER_START_5SEC;
	}
	else if (GetFSM_Messages(BUZZER_STOP_MES))
	{
	result = BUZZER_STOP;
	}
	else result = BUZZER_IDLE;*/

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
