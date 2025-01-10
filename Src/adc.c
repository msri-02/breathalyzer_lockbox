#include "adc.h"
#include "main.h"
#include <stdint.h>
#define ADCPORT GPIOA
#define ADCWAIT 4000

uint16_t conversion;
uint8_t ADC_flag = 0;

void ADC_init(void)
{
	/*-------------------ADC_INIT-----------------------*/
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN; 			// Turn on ADC clock
	// ADC will run at the same speed as the CPU clk
	ADC123_COMMON->CCR = (1 << ADC_CCR_CKMODE_Pos);
	// Power ADC and Voltage Regulator
	ADC1->CR &= ~(ADC_CR_DEEPPWD);
	ADC1->CR |= (ADC_CR_ADVREGEN);
	for (int i = ADCWAIT; i > 0; i--); 			// Wait at least 20 us
	// Calibrate the ADC
	// Ensure ADC is disabled and single-ended mode
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
	ADC1->CR |= ADC_CR_ADCAL;					// Start calibration
	while (ADC1->CR & ADC_CR_ADCAL); 			// Wait for calibration to finish
	// configure for single-ended mode on channel 5
	// must be set before enabling the ADC
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5); 	// Using ADC123_IN5 Channel
	// Enable the ADC
	ADC1->ISR |= (ADC_ISR_ADRDY); 				// Clear the ready bit
	ADC1->CR |= (ADC_CR_ADEN); 					// Enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY)); 		// Wait for ADRDY to become 1
	ADC1->ISR |= (ADC_ISR_ADRDY); 				// Clear ADRDY bit with a 1


	// Configure ADC
	// set sequence 1 for 1 conversion on channel 5
	ADC1->SQR1 = (5 << ADC_SQR1_SQ1_Pos);
	// 12-bit resolution, software trigger,
	// right align, single-conversion mode
	ADC1->CFGR = 0;
	// Sampling time for channel 5 to 2.5 clocks
	ADC1->SMPR1 = (7 << ADC_SMPR1_SMP5_Pos);
	// Enable EOC interrupt (end of conversion)
	ADC1->IER |= (ADC_IER_EOC);
	ADC1->ISR |= (ADC_ISR_EOC);
	// Enable interrupt in the NVIC
	NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F));
	__enable_irq(); // Enable global interrupts

	// Configure GPIO for channel 5
	RCC->AHB2ENR     |= RCC_AHB2ENR_GPIOAEN;
	ADCPORT->ASCR    |= (GPIO_ASCR_ASC0); 				// Connect analog switch to ADC input
	ADCPORT->MODER   &= ~(GPIO_MODER_MODE0);
	ADCPORT->MODER   |= (3 << GPIO_MODER_MODE0_Pos); 	// Analog mode
	ADCPORT->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED0_Pos);
	// Start a conversion
	ADC1->CR |= (ADC_CR_ADSTART);
}

void ADC1_2_IRQHandler(void)
{
	if (ADC1->ISR & ADC_ISR_EOC)
	{
		conversion = ADC1->DR; 	// Get conversion from the ADC data register
		ADC_flag = 1; 			// Set global flag
	}
}


uint16_t avg(uint16_t array[ARRAY_SIZE])
{
	/* function to get the average of the array*/
	int i;
	uint32_t avg = 0;
	uint16_t real_avg;
	for (i = 0; i < ARRAY_SIZE; i++)
	{
		avg += array[i];
	}
	avg /= ARRAY_SIZE; 			// Get the average
	real_avg = avg;
	return real_avg;
}



