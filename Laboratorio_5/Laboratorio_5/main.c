// Universidad del Valle de Guatemala
// Ie2023: Programación de Microcontroladores
// Pre Laboratorio 5
// Author: Rebeca Flores
// Hardware: ATMega328P
// Creado 06/04/2026
// Descripción: 

// Libraries
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "pwm_servo.h"

#define ADC_MAX         1023U
#define ADC_CHANNEL     6U
#define ADC_SETTLE_US   10U

// Function Prototypes
static void     ADC_Init(void);
static uint16_t ADC_Read(uint8_t channel);
static uint8_t  Map_ADC_To_Angle(uint16_t adc_val);

// Main Function
int main(void)
{
    uint16_t adc_val;
    uint8_t  angulo;
	
    ADC_Init();
    PWM_Init();

    while (1)
    {
        adc_val = ADC_Read(ADC_CHANNEL);
 
        angulo = Map_ADC_To_Angle(adc_val);
 
        PWM_SetAngle(angulo);

        _delay_ms(20);
    }
 
    return 0;
}

static void ADC_Init(void)
{
    ADMUX  = (0 << REFS1) |
             (1 << REFS0) |
             (0 << ADLAR); }
 
    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
 
     ADMUX = (ADMUX & 0xF0) | (ADC_CHANNEL & 0x07);
    _delay_us(ADC_SETTLE_US);
 
    ADCSRA |= (1 << ADSC);                 
    while (ADCSRA & (1 << ADSC));          
    (void)ADC;                              
}
 
 // NON-Interrupt Subroutines
static uint16_t ADC_Read(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    _delay_us(ADC_SETTLE_US);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    return ADC;
}
 
static uint8_t Map_ADC_To_Angle(uint16_t adc_val)
{
    return (uint8_t)(((uint32_t)adc_val * SERVO_MAX_DEG) / ADC_MAX);
}