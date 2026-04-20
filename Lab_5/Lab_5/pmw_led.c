/**
 * @file    pwm_led.c
 * @brief   Implementación PWM manual por software para LED (ATmega328P)
 *
 * Timer0 — Modo Normal (WGM01:00 = 00), desbordamiento cada 256 ticks.
 * La ISR TIMER0_OVF_vect implementa el PWM por software:
 *
 *  contador: 0  1  2  3 ... duty-1  duty  duty+1 ... PERIOD-1  0  1 ...
 *  pin D6:   H  H  H  H ...   H      L      L    ...     L      H  H ...
 *             ??????? pulso alto ?????????????? pulso bajo ?????????
 *
 * Configuración TCCR0B:
 *   CS02=0 CS01=1 CS00=1 ? Prescaler = 64
 *   f_ISR = 16MHz / 64 / 256 = 976.5 Hz
 *   f_PWM = 976.5 / 100      = ~9.76 Hz
 */

#include "pwm_led.h"

/* ??? Variables internas (compartidas con la ISR) ???????????????????????????? */

/**
 * Contador interno de la ISR — avanza de 0 a (PWM_LED_PERIOD - 1).
 * volatile: puede cambiar dentro de la interrupción.
 */
static volatile uint8_t pwm_counter = 0;

/**
 * Duty cycle activo [0, PWM_LED_PERIOD].
 * Se actualiza desde el main mediante PWM_LED_SetDuty().
 * volatile: la ISR la lee, el main la escribe.
 */
static volatile uint8_t pwm_duty = 0;

/* ??? ISR — Corazón del PWM manual ?????????????????????????????????????????? */

/**
 * @brief Interrupción de desbordamiento del Timer0.
 *
 * Se ejecuta ~976 veces por segundo (cada 1.024 ms).
 * Implementa el PWM por software:
 *   - contador == 0        ? pone el pin en ALTO (inicio del pulso)
 *   - contador == pwm_duty ? pone el pin en BAJO  (fin del pulso)
 *
 * Caso especial duty = 0:   el pin nunca sube ? LED completamente apagado.
 * Caso especial duty = 100: el pin nunca baja ? LED completamente encendido.
 */
ISR(TIMER0_OVF_vect)
{
    /* Avanzar el contador cíclico */
    if (pwm_counter >= PWM_LED_PERIOD)
    {
        pwm_counter = 0;
    }

    /* ?? Lógica PWM ?? */
    if (pwm_duty == 0)
    {
        /* Duty 0% ? LED siempre apagado */
        PORTD &= ~(1 << PD6);
    }
    else if (pwm_duty >= PWM_LED_PERIOD)
    {
        /* Duty 100% ? LED siempre encendido */
        PORTD |= (1 << PD6);
    }
    else
    {
        if (pwm_counter == 0)
        {
            /* Inicio del periodo ? pin en ALTO */
            PORTD |= (1 << PD6);
        }
        else if (pwm_counter == pwm_duty)
        {
            /* Se alcanzó el duty ? pin en BAJO */
            PORTD &= ~(1 << PD6);
        }
        /* En cualquier otro tick el pin mantiene su estado */
    }

    pwm_counter++;
}

/* ??? Implementación de la API pública ?????????????????????????????????????? */

void PWM_LED_Init(void)
{
    /*
     * 1. Configurar PD6 (Arduino D6) como salida
     *    PD6 = bit 6 del puerto D
     */
    DDRD |= (1 << PD6);

    /* Asegurar que el pin inicia en LOW */
    PORTD &= ~(1 << PD6);

    /*
     * 2. Configurar Timer0 en modo Normal
     *    WGM01:WGM00 = 0:0 ? modo Normal (contador libre hasta 255, luego desborda)
     *    No se conecta ninguna salida OC0A/OC0B ? COM bits = 0
     */
    TCCR0A = 0x00;    /* Modo Normal, sin salida OC */

    /*
     * 3. Configurar prescaler en TCCR0B
     *    CS02=0 CS01=1 CS00=1 ? Prescaler = 64
     *    f_overflow = 16MHz / 64 / 256 = 976.5 Hz
     */
    TCCR0B = (0 << CS02) |
         (1 << CS01) |
         (0 << CS00);

    /*
     * 4. Habilitar interrupción de desbordamiento de Timer0
     *    TOIE0 = Timer Overflow Interrupt Enable 0
     */
    TIMSK0 |= (1 << TOIE0);

    /* 5. Duty inicial: 0% (LED apagado) */
    pwm_duty    = 0;
    pwm_counter = 0;

    /*
     * NOTA: No se llama sei() aquí intencionalmente.
     * Las interrupciones globales deben habilitarse en main()
     * una vez que todos los módulos estén inicializados.
     */
}

/* --------------------------------------------------------------------------- */

void PWM_LED_SetDuty(uint8_t duty)
{
    /* Saturar al rango [0, PWM_LED_MAX] */
    if (duty > PWM_LED_MAX)
    {
        duty = PWM_LED_MAX;
    }

    /*
     * Escritura atómica:
     * pwm_duty es leída por la ISR. En el ATmega328P, una escritura
     * a uint8_t es atómica (una sola instrucción ST), por lo que no
     * es necesario deshabilitar interrupciones para esta asignación.
     */
    pwm_duty = duty;
}

/* --------------------------------------------------------------------------- */

uint8_t PWM_LED_ADCToDuty(uint16_t adc_val)
{
    /*
     * Mapeo lineal: [0, 1023] ? [0, 100]
     * duty = (adc_val * 100) / 1023
     * Se usa uint32_t para evitar desbordamiento: 1023 * 100 = 102300 > 65535
     */
    return (uint8_t)(((uint32_t)adc_val * PWM_LED_MAX) / 1023U);
}

/* --------------------------------------------------------------------------- */

void PWM_LED_Stop(void)
{
    /* Deshabilitar interrupción de Timer0 */
    TIMSK0 &= ~(1 << TOIE0);

    /* Apagar el LED */
    PORTD &= ~(1 << PD6);

    pwm_duty    = 0;
    pwm_counter = 0;
}