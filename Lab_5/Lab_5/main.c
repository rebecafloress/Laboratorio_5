// Universidad del Valle de Guatemala
// Ie2023: Programación de Microcontroladores
// Laboratorio 5
// Author: Rebeca Flores
// Hardware: ATMega328P
// Creado 06/04/2026
// Descripción: Dos potenciometros a través de puertos ACD 
// muestran un movimiento de 180° en dos servomotones a través de puertos PWM

/**
 * @file    main.c
 * @brief   Control de dos servos + LED PWM manual — ATmega328P (Arduino Nano)
 *
 *  Hardware:
 *   - Potenciómetro 1 ? A6 (ADC6) ? Servo 1  ? D9  (OC1A  / Timer1)
 *   - Potenciómetro 2 ? A7 (ADC7) ? Servo 2  ? D3  (OC2B  / Timer2)
 *   - Potenciómetro 3 ? A5 (ADC5) ? LED PWM  ? D6  (PD6   / Timer0 ISR)
 *
 *  Librerías propias:
 *   - pwm_servo.h/.c  ? Timer1 hardware  — Servo 1
 *   - pwm_servo2.h/.c ? Timer2 hardware  — Servo 2
 *   - pwm_led.h/.c    ? Timer0 software  — LED (PWM manual por interrupción)
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "pwm_servo.h"
#include "pwm_servo2.h"
#include "pwm_led.h"

/* ??? Constantes ADC ????????????????????????????????????????????????????????? */

#define ADC_MAX        1023U
#define ADC_CH_SERVO1  6U     /* A6 ? Servo 1 */
#define ADC_CH_SERVO2  7U     /* A7 ? Servo 2 */
#define ADC_CH_LED     5U     /* A5 ? LED     */

/* ??? Prototipos ????????????????????????????????????????????????????????????? */

static void     ADC_Init(void);
static uint16_t ADC_Read(uint8_t channel);
static uint8_t  Map_ADC_To_Angle(uint16_t adc_val);

/* ??????????????????????????????????????????????????????????????????????????? */

int main(void)
{
    uint16_t adc1, adc2, adc3;
    uint8_t  angulo1, angulo2, duty;

    /* ?? Inicializaciones ?? */
    ADC_Init();
    PWM_Init();       /* Servo 1 — Timer1 — D9  */
    PWM2_Init();      /* Servo 2 — Timer2 — D3  */
    PWM_LED_Init();   /* LED PWM — Timer0 — D6  */

    sei();            /* Habilitar interrupciones globales (necesario para LED) */

    while (1)
    {
        /* ?? Servo 1: A6 ? D9 ?? */
        adc1    = ADC_Read(ADC_CH_SERVO1);
        angulo1 = Map_ADC_To_Angle(adc1);
        PWM_SetAngle(angulo1);

        /* ?? Servo 2: A7 ? D3 ?? */
        adc2    = ADC_Read(ADC_CH_SERVO2);
        angulo2 = Map_ADC_To_Angle(adc2);
        PWM2_SetAngle(angulo2);

        /* ?? LED PWM: A5 ? D6 ?? */
        adc3 = ADC_Read(ADC_CH_LED);
        duty = PWM_LED_ADCToDuty(adc3);
        PWM_LED_SetDuty(duty);

        _delay_ms(20);
    }

    return 0;
}

/* ??????????????????????????????????????????????????????????????????????????? */

static void ADC_Init(void)
{
    ADMUX  = (0 << REFS1) |
             (1 << REFS0) |
             (0 << ADLAR);

    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);

    /* Conversión de descarte */
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
