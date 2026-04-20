#ifndef PWM_SERVO_H
#define PWM_SERVO_H
 
#include <avr/io.h>
#include <stdint.h>
 
#define F_CPU_HZ        16000000UL
 
#define PWM_PRESCALER   8UL
 
#define PWM_TOP         40000U

#define PWM_MIN_TICKS   1000U
 
#define PWM_MAX_TICKS   5000U
 
#define PWM_RANGE_TICKS (PWM_MAX_TICKS - PWM_MIN_TICKS)   /* 2 000 */
 
#define SERVO_MAX_DEG   180U
 
void PWM_Init(void);

void PWM_SetAngle(uint8_t grados);

void PWM_SetPulseUS(uint16_t us);

void PWM_Stop(void);
 
#endif 
