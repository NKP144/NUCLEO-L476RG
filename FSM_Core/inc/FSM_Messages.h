/*
 * FSM_Messages.h
 *
 *  Created on: Feb 25, 2020
 *      Author: Konstantin
 */

#ifndef INC_FSM_MESSAGES_H_
#define INC_FSM_MESSAGES_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

#define MAX_MESSAGES 			64
#define MAX_BROADCAST_MESSAGES	16
#define MAX_PARAM_MESSAGES 		8

typedef struct
{
	uint8_t messages_state;
	void *ParamPtr;
} FSM_message_data;

typedef enum {
	 MESSAGE_NOT_ACTIVE = 0,
	 MESSAGE_STATE		= 1,
	 MESSAGE_ACTIVE		= 2
 } FSM_message_state_e;

typedef enum {
	SIM800_RESET_BEGIN				= 0x00,
	SIM800_RESET_FINISH,
	SIM800_INIT_BEGIN,
	SIM800_INIT_FINISH,
	SEND_AT_COMMAND,
	AT_COMMAND_OK,
	AT_COMMAND_ERROR,
	AT_COMMANT_NO_ANSWER,
	SIM800_UART_IDLE_IT,
	SIM800_UART_TIMEOUT_IT,

 } FSM_messages_id_e;

 /* Параметры для передачи в сообщении для FSM_LED
  *
  */
 /*typedef enum {
   	LED_RED			= 0x00
 } FSM_LED_param_e;*/

 /*
  *
  */
 void InitFSM_Messages (void);

 /*
  *
  */
 void SendFSM_Messages (FSM_messages_id_e MessageID);

 /*
  *
  */
 void SendFSM_Broadcast_Messages (FSM_messages_id_e MessageID);

 /*
  *
  */
 void SendFSM_Param_Messages (FSM_messages_id_e MessageID, void *ParamPtr);

 /*
  *
  */
 uint8_t GetFSM_Messages (FSM_messages_id_e MessageID);

 /*
  *
  */
 uint8_t GetFSM_Broadcast_Messages (FSM_messages_id_e MessageID);

 /*
  *
  */
 void* GetFSM_Param_Messages (FSM_messages_id_e MessageID);

 /*
  *
  */
 void ProcessFSM_Messages (void);

#ifdef __cplusplus
}
#endif
#endif /* INC_FSM_MESSAGES_H_ */
