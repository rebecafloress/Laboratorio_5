#include "pwm_servo2.h"

void PWM2_Init(void)
{
    DDRD |= (1 << PD3);

    TCCR2A = (1 << COM2B1) |    
             (0 << COM2B0) |    
             (1 << WGM21)  |   
             (1 << WGM20);      
 
    TCCR2B = (1 << WGM22)  |   
             (1 << CS22)   |    
             (1 << CS21)   |    
             (1 << CS20);       

    OCR2A = PWM2_TOP;

    PWM2_SetAngle(90U);
}

void PWM2_SetAngle(uint8_t grados)
{
    uint8_t ticks;

    if (grados > SERVO2_MAX_DEG)
    {
        grados = SERVO2_MAX_DEG;
    }

    ticks = (uint8_t)(PWM2_MIN_TICKS +
                      ((uint16_t)grados * PWM2_RANGE_TICKS) / SERVO2_MAX_DEG);

    OCR2B = ticks;
}

/* --------------------------------------------------------------------------- */

void PWM2_Stop(void)
{
    TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));

    PORTD &= ~(1 << PD3);
}