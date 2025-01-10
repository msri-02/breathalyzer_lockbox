/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "adc.h"
#include "servo.h"
#include <stdio.h>
#define ADCPORT GPIOA
#define LEDPORT GPIOA
#define NUMSAMPLES 5				//Air samples collected
#define WAIT_TIME 100000
#define MIN 900						//Threshold for green-> yellow
#define MAX 2250					//Threshold for yellow->red
#define OPEN_ANGLE 3				//Servo open position
#define CLOSED_ANGLE 10				//Servo close position

#define BLUELED GPIO_ODR_OD4
#define GREENLED GPIO_ODR_OD5
#define YELLOWLED GPIO_ODR_OD6
#define REDLED GPIO_ODR_OD7


void SystemClock_Config(void);


void led_init(void)
{
	  /* configure LEDs for blinking
	   * PA4 - blue thinking LED
	   * PA5 - green
	   * PA6 - yellow
	   * PA7 - red*/
	  // start the clock
	  RCC->AHB2ENR     |= RCC_AHB2ENR_GPIOAEN;
	  LEDPORT->MODER   &= ~(GPIO_MODER_MODE4| GPIO_MODER_MODE5 |GPIO_MODER_MODE6| GPIO_MODER_MODE7 |GPIO_MODER_MODE8);
	  // set to output mode
	  LEDPORT->MODER   |= (1 << GPIO_MODER_MODE4_Pos|1 << GPIO_MODER_MODE5_Pos |1 << GPIO_MODER_MODE6_Pos |1 << GPIO_MODER_MODE7_Pos);
	  // no push pull resistors
	  LEDPORT->OTYPER  &= ~(GPIO_OTYPER_OT4|GPIO_OTYPER_OT5|GPIO_OTYPER_OT6|GPIO_OTYPER_OT7| GPIO_OTYPER_OT8);
	  // low speed
	  LEDPORT->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4|GPIO_OSPEEDR_OSPEED5|GPIO_OSPEEDR_OSPEED6|GPIO_OSPEEDR_OSPEED7|GPIO_OSPEEDR_OSPEED8);
	  // no pull up/pull down
	  LEDPORT->PUPDR   &= ~(GPIO_PUPDR_PUPD4|GPIO_PUPDR_PUPD5|GPIO_PUPDR_PUPD6|GPIO_PUPDR_PUPD7|GPIO_PUPDR_PUPD8);
	  //all LEDs on to indicate device is starting
	  LEDPORT->ODR     |= (GPIO_ODR_OD4 | GPIO_ODR_OD5| GPIO_ODR_OD6| GPIO_ODR_OD7);
}


int main(void)
{
	typedef enum
	{
		COLLECT,
		DETECT,
		GREEN,
		YELLOW,
		RED
	}state_var_type;

	/*----------------Init stuff-------------------*/
	HAL_Init();
	SystemClock_Config();
	ADC_init();
	led_init();
	servo_init();


	servo_rotate(CLOSED_ANGLE);				// Start off closed
	uint16_t sampleArray[NUMSAMPLES] = {0}; // Array to average out samples
	uint16_t i = 0;							// Variable to keep track of sample count
	uint16_t avg_value = 0;
	state_var_type state = COLLECT;			//Start state

	while (1)
	{
		/*----------------Switch Case-------------------*/
          switch(state)
          {
          case COLLECT:
        	  /*--Collect enough samples using the ADC--*/
        	  LEDPORT->ODR |= (REDLED);
			  if (ADC_flag == 1 && i < NUMSAMPLES)
			  {

				sampleArray[i] = conversion;	// Add a sample to the array
				ADC_flag = 0; 					// Reset the global flag
				i++;
				ADC1->CR |= (ADC_CR_ADSTART); 	// Start another conversion
				state = COLLECT;
			  }
			  else if (i == NUMSAMPLES)
			  {
				avg_value = (avg(sampleArray));	// Calculate avg value and reset sample count
				i = 0;
				state = DETECT;					//Change state
			  }

        	  /*---------Blue LED blinks---------------*/
			  GPIOA-> ODR &= ~(BLUELED);			//turn off
			  for (int k=0; k < WAIT_TIME; k++); 	//wait
			  GPIOA-> ODR |= (BLUELED);				//turn on
			  for (int k=0; k < WAIT_TIME; k++); 	//wait
			  LEDPORT->ODR &= ~(REDLED);
        	  break;
          case DETECT:
        	  /*----------Detect sobriety level-----------*/
        	  LEDPORT->ODR &= ~(BLUELED);         	 // turn off Blue LED

        	  if(avg_value <= MIN)
        	  {
        		  state = GREEN;
        	  }
        	  else if (avg_value < MAX && avg_value > MIN)
        	  {
        		  state = YELLOW;
        	  }
        	  else if (avg_value >= MAX)
        	  {
        		  state = RED;
        	  }
        	  break;
          case GREEN:
                /*-------------Sober state----------------*/
        	  	LEDPORT-> ODR |= (GREENLED);	        	    // Turn on GREEN LED
        	  	LEDPORT->ODR &= ~(BLUELED|YELLOWLED|REDLED);	// Turn off other LEDs
            	servo_rotate(OPEN_ANGLE);						// Move servo to open

        	  	for(int j = 0; j < WAIT_TIME; j++);
        	  	state= COLLECT;
                break;
          case YELLOW:
                /*---------Drinking but legal limit-------*/
				LEDPORT-> ODR |= (YELLOWLED);					// Turn on YELLOW LED
				LEDPORT->ODR &= ~(BLUELED|GREENLED|REDLED);		// Turn off other LEDS
		    	servo_rotate(OPEN_ANGLE);						// Move servo to open

				for(int j = 0; j < WAIT_TIME; j++);
				state= COLLECT;
				break;
          case RED:
                /*----------Drunk------------------------*/
				LEDPORT-> ODR |= (REDLED);						// Turn on RED LED
				LEDPORT->ODR &= ~(BLUELED|GREENLED|YELLOWLED);	// Turn off other LEDS
				for(int j = 0; j < WAIT_TIME; j++);
		    	servo_rotate(CLOSED_ANGLE);						// Move servo to close
				state= COLLECT;
                break;
		   default:
			   LEDPORT->ODR &= ~(BLUELED);
			   state = COLLECT;
			break;
          }
	}
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
