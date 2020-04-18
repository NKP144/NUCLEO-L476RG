/*
 * FSM_Timers.c
 *
 * Управление таймерами для конечного автомата
 *
 *  Created on: Feb 25, 2020
 *      Author: Konstantin
 */

#include "FSM_Timers.h"


uint32_t fsm_timers[MAX_TIMERS]; // Массив программных таймеров

/* Инициализация аппаратного таймера для FSM
 * Прерывания через 10мс
 *
 */
static void FSM_timers_hardware_init (void)
{
	//tim6_init();
	tim6_start();
}

static void FSM_timers_hardware_deinit (void)
{
	tim6_deinit();
}

/************************************************************************/
/*
 *  Обработчик прерываний аппаратного таймера
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
		uint8_t i = 0;

		for (i = 0; i < MAX_TIMERS; i++)
		{
			fsm_timers[i]++;
		}
	}
}

/************************************************************************/
/* Обнуление программных таймеров и инициализация аппаратного таймера
 *
 */
void InitFSM_timers (void)
{
	uint8_t i = 0;

	for (i = 0; i < MAX_TIMERS; i++)
	{
		fsm_timers[i] = 0;
	}

	FSM_timers_hardware_init ();
}

/************************************************************************/
/* Деинициализация аппаратного таймера и обнуление программного
 *
 */
void DeinitFSM_timers (void)
{
	uint8_t i = 0;

	for (i = 0; i < MAX_TIMERS; i++)
	{
		fsm_timers[i] = 0;
	}

	FSM_timers_hardware_deinit ();
}

/* Функция возвращает текущее значение таймера
 * TimerNumber - номер таймера
 *
 */
uint32_t GetFSM_Timer (FSM_timer_id_e TimerID)
{
	return fsm_timers[TimerID];
}

/* Функция обнуляет программный таймер
 *
 */
void ResetFSM_Timer (FSM_timer_id_e TimerID)
{
	fsm_timers[TimerID] = 0;
}



