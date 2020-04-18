/*
 * FSM_Messages.c
 *
 *  Created on: Feb 25, 2020
 *      Author: Konstantin
 */

#include "FSM_Messages.h"

uint8_t fsm_messages[MAX_MESSAGES]; 						// Массив сообщений
uint8_t fsm_broadcast_messages[MAX_BROADCAST_MESSAGES]; 	// Массив широковещательных сообщений
FSM_message_data fsm_param_messages [MAX_PARAM_MESSAGES];	// Массив собщений с параметрами


/* Инициализация сообщений, обнуление
 *
 */
void InitFSM_Messages (void)
{
	 uint8_t i = 0;

	 for (i = 0; i < MAX_MESSAGES; i++)
	 {
		 fsm_messages[i] = MESSAGE_NOT_ACTIVE;
	 }

	 for (i = 0; i < MAX_BROADCAST_MESSAGES; i++)
	 {
		 fsm_broadcast_messages [i] = MESSAGE_NOT_ACTIVE;
	 }

	 for (i = 0; i < MAX_PARAM_MESSAGES;  i++)
	 {
		 fsm_param_messages[i].messages_state = MESSAGE_NOT_ACTIVE;
		 fsm_param_messages[i].ParamPtr = NULL;
	 }
}


/* Отправить простое сообщение в виде флага (установлен -> сообщение отправлено)
 * Сообщение является установленным, но не активным.
 * Активация сообщения происходит при вызове функции ProcessFSM_Messages.
 * Активное сообщение обрабатывается на следующем шаге всей программы.
 */
void SendFSM_Messages (FSM_messages_id_e MessageID)
{
	if (fsm_messages[MessageID] == MESSAGE_NOT_ACTIVE)
	{
	fsm_messages[MessageID] = MESSAGE_STATE;
	}
}



/* Отправить широковещательное сообщение в виде флага (установлен -> сообщение отправлено)

 */
void SendFSM_Broadcast_Messages (FSM_messages_id_e MessageID)
{
	if (fsm_broadcast_messages[MessageID] == MESSAGE_NOT_ACTIVE)
	{
	fsm_broadcast_messages[MessageID] = MESSAGE_STATE;
	}
}

/* Отправить сообщение с параметром
 *  MessageID : ID сообщения
 *  ParamPtr: Указатель на параметр
 */
void SendFSM_Param_Messages (FSM_messages_id_e MessageID, void *ParamPtr)
{
	if (fsm_param_messages[MessageID].messages_state == MESSAGE_NOT_ACTIVE)
	{
	fsm_param_messages[MessageID].messages_state = MESSAGE_STATE;
	fsm_param_messages[MessageID].ParamPtr = ParamPtr;
	}
}

/* Получить простое сообщение в виде флага (установлен -> сообщение было отправлено).
 * Если сообщение в активном состоянии, то сбросить его и вернуть 1.
 * Если сообщение не в активном состоянии вернуть 0.
 */
uint8_t GetFSM_Messages (FSM_messages_id_e MessageID)
{
	if (fsm_messages[MessageID] == MESSAGE_ACTIVE)
	{
		fsm_messages[MessageID] = MESSAGE_NOT_ACTIVE;  // Сброс сообщения
		return 1;
	}

	return 0;
}

/* Получить широковещательное сообщение в виде флага (установлен -> сообщение было отправлено).
 * Сообщение сбрасывается только в функции ProcessFSM_Messages.
 */
uint8_t GetFSM_Broadcast_Messages (FSM_messages_id_e MessageID)
{
	if (fsm_broadcast_messages[MessageID] == MESSAGE_ACTIVE)
	{
		return 1;
	}

	return 0;
}

/* Получить  сообщение с параметром в виде указателя на параметр
 * MessageID : ID сообщения
 * Функция возвращает указатель на параметр.
 * Еслди указатель равен NULL, значит сообщение не было отправлено
 */
void* GetFSM_Param_Messages (FSM_messages_id_e MessageID)
{
	void *LocalParamPtr = NULL;
	if (fsm_param_messages[MessageID].messages_state == MESSAGE_ACTIVE)
	{
		fsm_param_messages[MessageID].messages_state = MESSAGE_NOT_ACTIVE;  // Сброс сообщения
		LocalParamPtr = fsm_param_messages[MessageID].ParamPtr;				// Сохранить параметр
		fsm_param_messages[MessageID].ParamPtr = NULL; 						// Сброс параметра
		return LocalParamPtr;
	}

	return NULL;
}

/*  Обработка сообщений. Если сообщение находится в состоянии установлено (MESSAGE_STATE), то
 *  оно переводится в состояние активно. Это сдела для того, что сообщения могли быть приняты
 *  всеми автоматами,включая те который расположены выше атомата, передающего сообщение, а не только последующим.
 *  Если сообщение находится в состоянии активно и ни один автомат его не обработал,
 *  оно сбрасывает в неактивное состояние.
 */
void ProcessFSM_Messages (void)
{
	uint8_t i = 0;

	for (i = 0; i < MAX_MESSAGES; i++)
	{
		if (fsm_messages[i] == MESSAGE_ACTIVE) fsm_messages[i] = MESSAGE_NOT_ACTIVE;
		if (fsm_messages[i] == MESSAGE_STATE) fsm_messages[i] = MESSAGE_ACTIVE;
	}

	for (i = 0; i < MAX_BROADCAST_MESSAGES; i++)
	{
		if (fsm_broadcast_messages[i] == MESSAGE_ACTIVE) fsm_broadcast_messages[i] = MESSAGE_NOT_ACTIVE;
		if (fsm_broadcast_messages[i] == MESSAGE_STATE) fsm_broadcast_messages[i] = MESSAGE_ACTIVE;
	}

	for (i = 0; i < MAX_PARAM_MESSAGES;  i++)
	{
		if (fsm_param_messages[i].messages_state == MESSAGE_ACTIVE)
		{
			fsm_param_messages[i].messages_state = MESSAGE_NOT_ACTIVE;
			fsm_param_messages[i].ParamPtr = NULL;
		}
		if (fsm_param_messages[i].messages_state == MESSAGE_STATE)
		{
			fsm_param_messages[i].messages_state = MESSAGE_ACTIVE;
		}
	 }
}
