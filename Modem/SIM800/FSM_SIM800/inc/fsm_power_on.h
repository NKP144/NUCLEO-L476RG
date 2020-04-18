/*
 * fsm_power_on.h
 *
 *  Created on: Apr 15, 2020
 *      Author: Konstantin
 */

#ifndef SIM800_FSM_SIM800_INC_FSM_POWER_ON_H_
#define SIM800_FSM_SIM800_INC_FSM_POWER_ON_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "FSM_Timers.h"
#include "FSM_Messages.h"

#define RESET_TIME		15*MSECX10 //150 мс
#define STATUS_HI_TIME 	3*SEC   //



typedef enum {
	SIM800_POWER_ON_INIT				= 0x00,
	SIM800_START_RESET,
	SIM800_WAIT_END_RESET,
	SIM800_WAIT_STATUS_HI,
	SIM800_STOP_RESET

  } FSM_SIM800_Power_on_STATE_e;

typedef enum {
	SIM800_POWER_IDLE  				= 0x00,
 	SIM800_BEGIN_RESET,
   } FSM_SIM800_Power_on_EVENTS_e;


void FSM_SIM800_Power_on_init(void);
void FSM_SIM800_Power_on (void);

#endif /* SIM800_FSM_SIM800_INC_FSM_POWER_ON_H_ */
