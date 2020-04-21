/*
 * fsm_power_on.c
 *
 *  Created on: Apr 15, 2020
 *      Author: Konstantin
 */


#include "fsm_power_on.h"
#include "modem_bsp.h"

FSM_SIM800_Power_on_STATE_e FSM_SIM800_Power_on_STATE;
FSM_SIM800_Power_on_STATE_e _FSM_SIM800_Power_on_STATE;

FSM_SIM800_Power_on_EVENTS_e FSM_SIM800_Power_on_EVENT;


static void start_SIM800_Power_on_timer (void);
static FSM_SIM800_Power_on_EVENTS_e sim800_power_get_event (void);

/*
 *
 */

void FSM_SIM800_Power_on_init(void)
{
	FSM_SIM800_Power_on_STATE = SIM800_POWER_ON_INIT;
	_FSM_SIM800_Power_on_STATE = FSM_SIM800_Power_on_STATE;
	ResetFSM_Timer (SIM800_TIMER);
}

/*  Конечный автомат управления питанием и перезагрузки модема
 *
 */
void FSM_SIM800_Power_on (void)
{
	FSM_SIM800_Power_on_EVENT = sim800_power_get_event ();

	switch (FSM_SIM800_Power_on_STATE) {
	case SIM800_POWER_ON_INIT:
		switch (FSM_SIM800_Power_on_EVENT)
		{
		case SIM800_POWER_IDLE:  mdm_rst_hi ();
			break;
		case SIM800_BEGIN_RESET: FSM_SIM800_Power_on_STATE = SIM800_START_RESET;
							     _FSM_SIM800_Power_on_STATE = SIM800_POWER_ON_INIT;
			break;
		}

		break;
	case SIM800_START_RESET:	mdm_rst_low ();
								start_SIM800_Power_on_timer();
								FSM_SIM800_Power_on_STATE = SIM800_WAIT_END_RESET;
								_FSM_SIM800_Power_on_STATE = SIM800_START_RESET;
		break;
	case SIM800_WAIT_END_RESET:	if (GetFSM_Timer(SIM800_TIMER) >= RESET_TIME)  //
	 	  	  	  	  			{
									mdm_rst_hi ();
									FSM_SIM800_Power_on_STATE = SIM800_WAIT_STATUS_HI;
									_FSM_SIM800_Power_on_STATE = SIM800_WAIT_END_RESET;
									start_SIM800_Power_on_timer();
	 	  	  	  	  			}
		break;
	case SIM800_WAIT_STATUS_HI: if (GetFSM_Timer(SIM800_TIMER) >= STATUS_HI_TIME)
								{
									FSM_SIM800_Power_on_STATE = SIM800_STOP_RESET;
									_FSM_SIM800_Power_on_STATE = SIM800_WAIT_STATUS_HI;
								}
		break;
	case SIM800_STOP_RESET:		SendFSM_Messages (SIM800_RESET_FINISH);
								SendFSM_Messages (SIM800_INIT_BEGIN);
								FSM_SIM800_Power_on_STATE = SIM800_POWER_ON_INIT;
								_FSM_SIM800_Power_on_STATE = SIM800_STOP_RESET;
		break;
	}
}


/*
 *
 */

static FSM_SIM800_Power_on_EVENTS_e sim800_power_get_event (void)
{
	FSM_SIM800_Power_on_EVENTS_e result = SIM800_POWER_IDLE;

	 if (GetFSM_Messages(SIM800_RESET_BEGIN))
	 {
		 result = SIM800_BEGIN_RESET;
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
 */static void start_SIM800_Power_on_timer (void)
 {
 	if (FSM_SIM800_Power_on_STATE != _FSM_SIM800_Power_on_STATE)
 	{
 		ResetFSM_Timer (SIM800_TIMER);
 		_FSM_SIM800_Power_on_STATE = FSM_SIM800_Power_on_STATE;
 	}
 }

