/*
 * fsm_sim800_init.h
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#ifndef SIM800_FSM_SIM800_INC_FSM_SIM800_INIT_H_
#define SIM800_FSM_SIM800_INC_FSM_SIM800_INIT_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "FSM_Timers.h"
#include "FSM_Messages.h"
#include "at_command.h"

#define SMS_READY_TIME	5*SEC //150 мс


typedef enum {
	SIM800_INIT_IDLE_S				= 0x00,
	SIM800_AT_SEND_S,
	SIM800_WAIT_AT_ANSWER_S,
	SIM800_INIT_FINISH_S
  } FSM_SIM800_INIT_STATE_e;

typedef enum {
	SIM800_IDLE_INIT_E 				= 0x00,
 	SIM800_START_INIT_E,
	SIM800_AT_RECEIVE_OK_E,
	SIM800_AT_RECEIVE_TIMEOUT_E,
	SIM800_AT_RECEIVE_ERROR_E,
   } FSM_SIM800_INIT_EVENTS_e;


void FSM_SIM800_Initialize_init(void);
void FSM_SIM800_Initialize (void);

#endif /* SIM800_FSM_SIM800_INC_FSM_SIM800_INIT_H_ */
