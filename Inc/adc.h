#define ARRAY_SIZE 5
#include <stdint.h>
#ifndef INC_ADC_H_
#define INC_ADC_H_

extern uint16_t conversion;
extern uint8_t ADC_flag;

void ADC_init(void);
uint16_t avg(uint16_t array[ARRAY_SIZE]);
void ADC1_2_IRQHandler(void);

#endif /* INC_ADC_H_ */
