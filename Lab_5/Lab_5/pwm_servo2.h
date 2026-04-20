#ifndef PWM_SERVO2_H
#define PWM_SERVO2_H

#include <avr/io.h>
#include <stdint.h>

#define PWM2_TOP         255U

#define PWM2_MIN_TICKS   7U

#define PWM2_MAX_TICKS   36U

#define PWM2_RANGE_TICKS (PWM2_MAX_TICKS - PWM2_MIN_TICKS)

#define SERVO2_MAX_DEG   180U

void PWM2_Init(void);

void PWM2_SetAngle(uint8_t grados);

void PWM2_Stop(void);

#endif
