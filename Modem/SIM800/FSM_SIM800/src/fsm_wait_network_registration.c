/*
 * fsm_wait_network_registration.c
 *
 *  Created on: 20 апр. 2020 г.
 *      Author: Konstantin
 */


#include "fsm_wait_network_registration.h"
#include "fsm_at_command_send.h"
#include "string.h"

static void start_creg_wait (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);
static void finish_creg_wait (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);
static void error_creg_wait (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);
static void repeat_creg_wait (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);
static void call_at_send_fsm (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);
static void start_net_reg_timer (FSM_NET_REG_STATE_e, FSM_NET_REG_EVENTS_e);

struct transition FSM_wait_reg_table [3][6] = {

	[IDLE_S][NO_EVENT_E] = {IDLE_S, NULL},
	[IDLE_S][START_REG_E] = {WAIT_CREG_S, start_creg_wait},  //---> отправка AT+CREG? переход в сос-е ожидания ответа
	[IDLE_S][REG_OK_E] = {IDLE_S, NULL},
	[IDLE_S][REG_NOT_E] = {IDLE_S, NULL},
	[IDLE_S][REG_ERROR_E] = {IDLE_S, NULL},
	[IDLE_S][DELAY_TIMEOUT] = {IDLE_S, NULL},

	[WAIT_CREG_S][NO_EVENT_E] = {WAIT_CREG_S, call_at_send_fsm},
	[WAIT_CREG_S][START_REG_E] = {WAIT_CREG_S, NULL},
	[WAIT_CREG_S][REG_OK_E] = {IDLE_S, finish_creg_wait},      // ---> завершение fsm, отправка сообщения OK
	[WAIT_CREG_S][REG_NOT_E] = {DELAY_S, start_net_reg_timer}, // ---> переход в сос-е паузы, запуск таймера
	[WAIT_CREG_S][REG_ERROR_E] = {IDLE_S, error_creg_wait},	   // ---> завершение fsm, отправка error
	[WAIT_CREG_S][DELAY_TIMEOUT] = {WAIT_CREG_S, NULL},

	[DELAY_S][NO_EVENT_E] = {DELAY_S, NULL},
	[DELAY_S][START_REG_E] = {DELAY_S, NULL},
	[DELAY_S][REG_OK_E] = {DELAY_S, NULL},
	[DELAY_S][REG_NOT_E] = {DELAY_S, NULL},
	[DELAY_S][REG_ERROR_E] = {DELAY_S, NULL},
	[DELAY_S][DELAY_TIMEOUT] = {WAIT_CREG_S, repeat_creg_wait} //---> повторная отправка AT+CREG? после паузы, переход к ожиданию ответа

};

FSM_NET_REG_STATE_e FSM_NET_REG_STATE;
FSM_NET_REG_STATE_e _FSM_NET_REG_STATE;

FSM_NET_REG_EVENTS_e FSM_NET_REG_EVENTS;

uint8_t creg_attemts;


static FSM_NET_REG_EVENTS_e net_reg_get_event (void);
static void clear_rx_buf (void);



/*
 *
 */
void FSM_wait_net_reg_init (void)
{
	FSM_NET_REG_STATE = IDLE_S;
	_FSM_NET_REG_STATE = FSM_NET_REG_STATE;
	ResetFSM_Timer (SIM800_TIMER);
}


/*
 *
 */
void FSM_wait_net_reg (void)
{

	FSM_NET_REG_EVENTS = net_reg_get_event ();

	_FSM_NET_REG_STATE = FSM_NET_REG_STATE;
	FSM_NET_REG_STATE = FSM_wait_reg_table [FSM_NET_REG_STATE][FSM_NET_REG_EVENTS].NEW_FSM_STATE;
	transition_callback fsm_state_function = FSM_wait_reg_table [_FSM_NET_REG_STATE][FSM_NET_REG_EVENTS].work_function;

	if (fsm_state_function != NULL)
	{
		fsm_state_function (FSM_NET_REG_STATE, FSM_NET_REG_EVENTS);
		printf("FSM_wait_net_reg state = %d, signal = %d, new = %d\n", _FSM_NET_REG_STATE, FSM_NET_REG_EVENTS, FSM_NET_REG_STATE);
	}



}



/*  Запуск таймера
 *
 */
static void start_net_reg_timer (FSM_NET_REG_STATE_e state, FSM_NET_REG_EVENTS_e event)
{
	if (FSM_NET_REG_STATE != _FSM_NET_REG_STATE)
	 	{
	 		ResetFSM_Timer (SIM800_TIMER);
	 		_FSM_NET_REG_STATE = FSM_NET_REG_STATE;
	 	}
}

/*  Очистка буффера
 *
 */
void clear_rx_buf (void)
{
	memset (modem_rx_buf, '\0', uart_rx_counter);
}

/*  Получение событий
 *
 */
static FSM_NET_REG_EVENTS_e net_reg_get_event (void)
{
	FSM_NET_REG_EVENTS_e result = NO_EVENT_E;

	if (GetFSM_Messages(SIM800_INIT_FINISH))
	{
	 result = START_REG_E;
	}
	else if (GetFSM_Broadcast_Messages(AT_COMMAND_OK))
	{
		if (at_compare_answer ((char*)modem_rx_buf,"CREG: 0,1", NULL) == AT_OK) result = REG_OK_E;
		else if (at_compare_answer ((char*)modem_rx_buf,"CREG: 0,5", NULL) == AT_OK) result = REG_OK_E;
		else if (creg_attemts < MAX_CREG_ATTEMPTS) result = REG_NOT_E;
		else result = REG_ERROR_E;
	}
	else if (GetFSM_Broadcast_Messages(AT_COMMAND_ERROR))
	{
		result = REG_ERROR_E;
	}
	else if (GetFSM_Broadcast_Messages(AT_COMMANT_NO_ANSWER))
	{
		result = REG_ERROR_E;
	}
	else if ((FSM_NET_REG_STATE == DELAY_S) && (GetFSM_Timer(SIM800_TIMER) >= DELAY_NET_REG_TIMEOUT))
	{
		result = DELAY_TIMEOUT;
	}
	/*else result = */

	return result;
}

/*	 Функция вызывается при переходе из состояния IDLE_S в WAIT_CREG_S
 *   Отправляется AT-команда AT+CREG?. Ожидаемый ответ CREG.
 *   Обнуляется счетчик попыток регистрации в сети
 *
 */
void start_creg_wait (FSM_NET_REG_STATE_e state, FSM_NET_REG_EVENTS_e event)
{
	creg_attemts = 0;
	SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT+CREG?\r\n", "CREG", CREG_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX|NEED_RX));
}

/* Повторная отправка и ожидание CREG
 *
 */
void repeat_creg_wait (FSM_NET_REG_STATE_e state, FSM_NET_REG_EVENTS_e event)
{
	creg_attemts++;
	SendFSM_Param_Messages(SEND_AT_COMMAND, fill_at_command_data ("AT+CREG?\r\n", "CREG", CREG_TIMEOUT, NEED_TIMEOUT_IT, NEED_TX|NEED_RX));
}

/*
 *
 *
 */
void finish_creg_wait (FSM_NET_REG_STATE_e state, FSM_NET_REG_EVENTS_e event)
{
	SendFSM_Messages (NET_REGISRATION_OK);
}

/*
 *
 *
 */
void error_creg_wait (FSM_NET_REG_STATE_e state, FSM_NET_REG_EVENTS_e event)
{
	SendFSM_Messages (NET_REGISRATION_ERROR);
}


/*
 *
 *
 *
 */
void call_at_send_fsm (FSM_NET_REG_STATE_e state, FSM_NET_REG_EVENTS_e event)
{
	FSM_AT_SEND ();
}




