/*
 * modem_bsb.c
 *
 *  Created on: Apr 16, 2020
 *      Author: Konstantin
 */

#include "modem_bsp.h"


void mdm_rst_hi (void)
{
	SIM_RST_HI();
}

void mdm_rst_low (void)
{
	SIM_RST_LOW();
}
