/*
 * fsm_wait_network_registration.h
 *
 *  Created on: 20 апр. 2020 г.
 *      Author: Konstantin
 */

#ifndef SIM800_FSM_SIM800_INC_FSM_WAIT_NETWORK_REGISTRATION_H_
#define SIM800_FSM_SIM800_INC_FSM_WAIT_NETWORK_REGISTRATION_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "FSM_Timers.h"
#include "FSM_Messages.h"
#include "at_command.h"

#define REGULAR_TIMEOUT				250
#define CREG_TIMEOUT				250 //мс
#define DELAY_NET_REG_TIMEOUT		5*SEC

#define MAX_CREG_ATTEMPTS			8

typedef enum states
{
	IDLE_S					= 0x00,
	WAIT_CREG_S,
	DELAY_S
} FSM_NET_REG_STATE_e;

typedef enum evets
{
	NO_EVENT_E 				= 0x00,
 	START_REG_E,
	REG_OK_E,
	REG_NOT_E,
	REG_ERROR_E,
	DELAY_TIMEOUT
} FSM_NET_REG_EVENTS_e;

typedef void (*transition_callback) (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);

struct transition
{
	FSM_NET_REG_STATE_e		NEW_FSM_STATE;
	transition_callback		work_function;
};









#endif /* SIM800_FSM_SIM800_INC_FSM_WAIT_NETWORK_REGISTRATION_H_ */
