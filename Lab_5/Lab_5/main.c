// Universidad del Valle de Guatemala
// Ie2023: Programación de Microcontroladores
// Laboratorio 5
// Author: Rebeca Flores
// Hardware: ATMega328P
// Creado 06/04/2026
// Descripción: Dos potenciometros a través de puertos ACD 
// muestran un movimiento de 180° en dos servomotones a través de puertos PWM

// Libraries

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "pwm_servo.h"
#include "pwm_servo2.h"
#include "pwm_led.h"

#define ADC_MAX        1023U
#define ADC_CH_SERVO1  6U   
#define ADC_CH_SERVO2  7U    
#define ADC_CH_LED     5U   

// Function Prototypes

static void     ADC_Init(void);
static uint16_t ADC_Read(uint8_t channel);
static uint8_t  Map_ADC_To_Angle(uint16_t adc_val);

// Main Function

int main(void)
{
	uint16_t adc1, adc2, adc3;
	uint8_t  angulo1, angulo2, duty;

	ADC_Init();
	PWM_Init();    
	PWM2_Init();   
	PWM_LED_Init();  

	sei();          
	
	while (1)
	{
		//Servo 1
		adc1    = ADC_Read(ADC_CH_SERVO1);
		angulo1 = Map_ADC_To_Angle(adc1);
		PWM_SetAngle(angulo1);

		//Servo 2
		adc2    = ADC_Read(ADC_CH_SERVO2);
		angulo2 = Map_ADC_To_Angle(adc2);
		PWM2_SetAngle(angulo2);

		// LED
		adc3 = ADC_Read(ADC_CH_LED);
		duty = PWM_LED_ADCToDuty(adc3);
		PWM_LED_SetDuty(duty);

		_delay_ms(20);
	}

	return 0;
}

// NON-Interrupt Subroutines

static void ADC_Init(void)
{
	ADMUX  = (0 << REFS1) |
	(1 << REFS0) |
	(0 << ADLAR);

	ADCSRA = (1 << ADEN)  |
	(1 << ADPS2) |
	(1 << ADPS1) |
	(1 << ADPS0);

	//Segunda conversión
	ADMUX = (ADMUX & 0xF0) | (ADC_CH_SERVO1 & 0x07);
	_delay_us(10);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	(void)ADC;
}

static uint16_t ADC_Read(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	_delay_us(10);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

static uint8_t Map_ADC_To_Angle(uint16_t adc_val)
{
	return (uint8_t)(((uint32_t)adc_val * SERVO_MAX_DEG) / ADC_MAX);
}