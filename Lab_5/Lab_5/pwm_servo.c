#include "pwm_servo.h"

static inline uint16_t clamp16(uint16_t val, uint16_t lo, uint16_t hi)
{
	if (val < lo) return lo;
	if (val > hi) return hi;
	return val;
}


void PWM_Init(void)
{

	DDRB |= (1 << PB1);
	
	TCCR1A = (1 << COM1A1) |
	(0 << COM1A0) |
	(1 << WGM11)  |
	(0 << WGM10);
	TCCR1B = (1 << WGM13)  |
	(1 << WGM12)  |
	(0 << CS12)   |
	(1 << CS11)   |
	(0 << CS10);
	
	ICR1 = PWM_TOP;

	PWM_SetAngle(90U);
}


void PWM_SetAngle(uint8_t grados)
{
	uint16_t ticks;

	if (grados > SERVO_MAX_DEG)
	{
		grados = SERVO_MAX_DEG;
	}

	ticks = (uint16_t)(PWM_MIN_TICKS +
	((uint32_t)grados * PWM_RANGE_TICKS) / SERVO_MAX_DEG);
	
	OCR1A = ticks;
}

/* --------------------------------------------------------------------------- */

void PWM_SetPulseUS(uint16_t us)
{
	uint16_t ticks;
	us = clamp16(us, 1000U, 2000U);
	ticks = us * 2U;
	
	OCR1A = ticks;
}

/* --------------------------------------------------------------------------- */

void PWM_Stop(void)
{
	TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));

	PORTB &= ~(1 << PB1);
}
