#ifndef PWM_LED_H
#define PWM_LED_H
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
 
#define PWM_LED_PERIOD   100U

#define PWM_LED_MIN      0U
#define PWM_LED_MAX      100U

void PWM_LED_Init(void);
 
void PWM_LED_SetDuty(uint8_t duty);
 
uint8_t PWM_LED_ADCToDuty(uint16_t adc_val);
 
void PWM_LED_Stop(void);
 
#endif