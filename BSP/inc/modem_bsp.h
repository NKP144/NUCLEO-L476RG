/*
 * modem_bsp.h
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#ifndef INC_MODEM_BSP_H_
#define INC_MODEM_BSP_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define SIM_RST_LOW()	 LL_GPIO_ResetOutputPin(SIM_RST_GPIO_Port, SIM_RST_Pin)
#define SIM_RST_HI()	 LL_GPIO_SetOutputPin(SIM_RST_GPIO_Port, SIM_RST_Pin)

void mdm_rst_hi (void);
void mdm_rst_low (void);


#endif /* INC_MODEM_BSP_H_ */
