/*
 * fsm_at_command_send.h
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#ifndef SIM800_FSM_SIM800_INC_FSM_AT_COMMAND_SEND_H_
#define SIM800_FSM_SIM800_INC_FSM_AT_COMMAND_SEND_H_

 /* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "at_command.h"
#include "FSM_Timers.h"
#include "FSM_Messages.h"


typedef enum {
	AT_SEND_IDLE_S				= 0x00,
	AT_SEND_START_S,
	AT_WAIT_ANSWER_S,
	AT_RECEIVE_OK_S,
	AT_RECEIVE_ERROR_S,
	AT_NOT_RECEIVED_S
  } FSM_AT_SEND_STATE_e;

typedef enum {
	AT_SEND_IDLE_E 				= 0x00,
 	AT_SEND_BEGIN_E,
	AT_RECEIVE_IDLE_E,
	AT_RECEIVE_TIMEOUT_E
   } FSM_AT_SEND_EVENTS_e;


void FSM_AT_SEND_init(void);
void FSM_AT_SEND (void);



#endif /* SIM800_FSM_SIM800_INC_FSM_AT_COMMAND_SEND_H_ */
