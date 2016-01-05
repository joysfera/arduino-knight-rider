/**
 * Knight Rider (actually K.I.T.T.) 8xLED scanner bar
 * 
 * Petr Stehlik in December 2015
 *
 * petr@pstehlik.cz
 *
 * licensed under the GPLv3
 */

#include <TimerOne.h>

#define PWM_PINS    8

volatile bool leds[PWM_PINS];
volatile byte pwm_regs[PWM_PINS];
volatile byte kittMode;

void setup()
{
    for(byte i=2; i<10; i++) {
        pinMode(i, OUTPUT);
    }
    LEDoff();
    setKittMode(2);
    Timer1.initialize(64);  // 15625 Hz => 8bit PWM 61 Hz refresh rate
    Timer1.attachInterrupt(myIrq);
}

void loop()
{
    nextKittStep();
    delay(72);  // this is the exact speed of the original Knight Rider intro (on the desert)
}

void LEDoff()
{
    for(byte i=2; i<10; i++) {
        digitalWrite(i, HIGH);
    }
}

void setKittMode(byte mode)
{
    kittMode = -1;
    LEDoff();
    kittMode = mode;
}

void nextKittStep()
{
    static byte index;
    static bool dirRight;

    switch(kittMode) {
        case 0: leds[index] = LOW;
                if (++index >= PWM_PINS) index = 0;
                leds[index] = HIGH;
                break;
        case 1: leds[index] = LOW;
                if (--index >= PWM_PINS) index = PWM_PINS-1;
                leds[index] = HIGH;
                break;
        case 2: leds[index] = LOW;
                if (dirRight && ++index >= PWM_PINS) { index = PWM_PINS-2; dirRight = false; }
                else if (!dirRight && --index >= PWM_PINS) { index = 1; dirRight = true; }
                leds[index] = HIGH;
                break;
    }
}

void myIrq(void)
{
    static byte counter = 0;
    if (!counter++)
        fadeOutEffect();
    softPWM();
}

void softPWM(void)
{
    static byte counter = 0;
    static byte shadows[PWM_PINS];

    if (!counter++) {
        for(byte i = 0; i < PWM_PINS; i++) {
            shadows[i] = pwm_regs[i];
        }
        counter++;
    }

    byte state = 0;   
    for(byte i = 0; i < PWM_PINS; i++) {
        bool b = false;
        if (shadows[i]) {
            shadows[i]--;
            b = true;
        }
        state = state << 1 | b;
    }
    PORTD = (PORTD & 0x03) | ~(state << 2);
    PORTB = (PORTB & ~0x03) | ~(state >> 6);
}

void fadeOutEffect(void)
{
    for(byte i = 0; i < PWM_PINS; i++) {
        if (leds[i] && pwm_regs[i] != 255) {
            unsigned x = pwm_regs[i] + 32;  // quickly light up (in 4 steps)
            if (x > 255) x = 255;
            pwm_regs[i] = x;
        }
        else if (!leds[i] && pwm_regs[i]) {
            pwm_regs[i] = pwm_regs[i] * 15 / 16; // very slowly fade out (in 54 steps)
        }
    }   
}

