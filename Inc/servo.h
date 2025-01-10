/*
 * servo.h
 *
 *  Created on: Jun 8, 2023
 *      Author: msri and chrosp
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"

void servo_init(void);
void servo_rotate(float angle);

#endif /* INC_SERVO_H_ */
