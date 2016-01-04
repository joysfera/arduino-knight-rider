/**
 * Knight Rider (actually K.I.T.T.) 8xLED scanner 
 * 
 * Petr Stehlik 2015
 * licensed under the GPLv3
 */

#include <TimerOne.h>

#define PWM_PINS    8

volatile bool leds[PWM_PINS];
volatile byte pwm_regs[PWM_PINS];

void setup()
{
    for(byte i=2; i<10; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH); // LED OFF
    }
    Timer1.initialize(64);  // 15625 Hz => 8bit PWM 61 Hz refresh rate
    Timer1.attachInterrupt(myIrq);
}

void loop()
{
    static byte x;
    static bool dir = false;

    for(byte i = 0; i < PWM_PINS; i++) {
        leds[i] = (i == x);
    }
    if (dir) {
        if (++x == PWM_PINS) {
            dir = !dir;
            x = PWM_PINS-2;
        }
    }
    else {
        if (!x--) {
            dir = !dir;
            x = 1;
        }
    }
    delay(200);
}

void myIrq(void)
{
    static byte counter = 0;
    if (!counter++)
        myDosvit();
    myPwm();
}

void myPwm(void)
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

void myDosvit(void)
{
    for(byte i = 0; i < PWM_PINS; i++) {
        if (leds[i]) {
            pwm_regs[i] = 255;
        }
        else {
            if (pwm_regs[i]) {
                int x = pwm_regs[i] * 7 / 8;
                if (x < 0)
                    x = 0;
                pwm_regs[i] = x;
            }
        }
    }   
}

