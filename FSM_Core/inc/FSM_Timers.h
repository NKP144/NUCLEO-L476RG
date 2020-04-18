/*
 * FSM_Timers.h
 *
 *  Управление таймерами для конечного автомата
 *
 *  Created on: Feb 25, 2020
 *      Author: Konstantin
 */

#ifndef INC_FSM_TIMERS_H_
#define INC_FSM_TIMERS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "tim.h"

#define MAX_TIMERS	16

#define MSECX10	1		// 10мс
#define SEC 	100   	//Период аппаратного таймера 10мс -> 1сек = 100 * 10 мс
#define MIN 	60*SEC
#define HOUR	60*MIN
#define DAY		24*HOUR

typedef enum {
 	SIM800_TIMER			= 0x00,
	AT_COMMAND_TIMER
 } FSM_timer_id_e;

/*  Инициализация таймеров, инициализация аппаратного таймера
 *
 */
void InitFSM_timers (void);

/* Функция возвращает текущее значение таймера
 * TimerID - номер таймера
 *
 */
uint32_t GetFSM_Timer (FSM_timer_id_e TimerID);

/* Функция обнуляет программный таймер
 *
 */
void ResetFSM_Timer (FSM_timer_id_e TimerID);

#ifdef __cplusplus
}
#endif
#endif /* INC_FSM_TIMERS_H_ */
