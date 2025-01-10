/*
 * servo.c
 *
 *  Created on: Jun 8, 2023
 *      Author: msri and chrosp
 */

#include "servo.h"
// Function to rotate servo to a specified angle
void servo_rotate(float angle)
{
    // Update duty cycle
    TIM2->CCR2 = (uint32_t)((angle));
}


void servo_init(void)
{
	/* ------------------GPIO CONFIG-------------------------*/
	// enable the GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// configure GPIO pin as AF mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE1);
	GPIOA->MODER |= (2 << GPIO_MODER_MODE1_Pos);

	//set to push pull
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD1);

	// select AF1 for PA1 (TIM2_CH2)
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL1);
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL1_Pos);

	/* ------------------TIMER CONFIG-------------------------*/
	// enable TIM2 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

	// set prescaler and ARR values
	TIM2->PSC = 480 - 1;			// 24000000/50 = 480000
	TIM2->ARR = 1000 - 1;			// 480000/480 = 1000
									// Set ARR to 1000 so that CCR2 can be porportional to 1000% of degree

	// configure the CCR2 register for PWM mode
	TIM2->CCR2 = 0;  				// Initial position

	// configure the PWM mode for TIM2 channel 2 (output compare mode)
	TIM2->CCMR1 &= ~(TIM_CCMR1_OC2M);
	TIM2->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2);  // PWM mode 110

	//set event generation
	TIM2->EGR |= (TIM_EGR_UG);

	// enable the output for TIM2 channel 2
	TIM2->CCER |= TIM_CCER_CC2E;

	// enable TIM2 counter (start the counter)
	TIM2->CR1 |= TIM_CR1_CEN;
}
