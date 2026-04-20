#include "pwm_led.h"

static volatile uint8_t pwm_counter = 0;

static volatile uint8_t pwm_duty = 0;

ISR(TIMER0_OVF_vect)
{
    if (pwm_counter >= PWM_LED_PERIOD)
    {
        pwm_counter = 0;
    }

    if (pwm_duty == 0)
    {
        PORTD &= ~(1 << PD6);
    }
    else if (pwm_duty >= PWM_LED_PERIOD)
    {
        PORTD |= (1 << PD6);
    }
    else
    {
        if (pwm_counter == 0)
        {
            PORTD |= (1 << PD6);
        }
        else if (pwm_counter == pwm_duty)
        {
            PORTD &= ~(1 << PD6);
        }
    }

    pwm_counter++;
}

void PWM_LED_Init(void)
{

    DDRD |= (1 << PD6);
    PORTD &= ~(1 << PD6);
    TCCR0A = 0x00; 

    TCCR0B = (0 << CS02) |
         (1 << CS01) |
         (0 << CS00);

    TIMSK0 |= (1 << TOIE0);

    pwm_duty    = 0;
    pwm_counter = 0;
}

/* --------------------------------------------------------------------------- */

void PWM_LED_SetDuty(uint8_t duty)
{
    if (duty > PWM_LED_MAX)
    {
        duty = PWM_LED_MAX;
    }
	
    pwm_duty = duty;
}

/* --------------------------------------------------------------------------- */

uint8_t PWM_LED_ADCToDuty(uint16_t adc_val)
{
    return (uint8_t)(((uint32_t)adc_val * PWM_LED_MAX) / 1023U);
}

/* --------------------------------------------------------------------------- */

void PWM_LED_Stop(void)
{
    TIMSK0 &= ~(1 << TOIE0);

    PORTD &= ~(1 << PD6);

    pwm_duty    = 0;
    pwm_counter = 0;
}