#include "blink.h"

void blink::initialize(int l1, int l2, int l3, int l4) {
    led1 = l1;
    led2 = l2;
    led3 = l3;
    led4 = l4;
    onTime[4] = {0};
}

void blink::process() {
    blinkIt(0, blink1, led1);
    blinkIt(1, blink2, led2);
    blinkIt(2, blink3, led3);
    blinkIt(3, blink4, led4);
}

void blink::blinkIt(int num, bool blink, int led) {
    if (blink) {
        if (onTime[num] == 0) {
            onTime[num] = millis();
            digitalWrite(led, HIGH);
        } else if (millis() - onTime[num] > 200 && millis() - onTime[num] < 400) {
            digitalWrite(led, LOW);
        } else if (millis() - onTime[num] >= 400) {
            digitalWrite(led, LOW);
            onTime[num] = 0;
        }
    }
}

void blink::setOn(int led) {
    onTime[led] = 0;
    switch (led)
    {
    case 1:
        blink1 = true;
        blink2 = false;
        blink3 = false;
        blink4 = false;
        break;
    case 2:
        blink1 = false;
        blink2 = true;
        blink3 = false;
        blink4 = false;
        break;
    case 3:
        blink1 = false;
        blink2 = false;
        blink3 = true;
        blink4 = false;
        break;
    case 4:
        blink1 = false;
        blink2 = false;
        blink3 = false;
        blink4 = true;
        break;
    
    default:
        blink1 = false;
        blink2 = false;
        blink3 = false;
        blink4 = false;
        break;
    }
}

void blink::setOff(int led) {
    switch (led)
    {
    case 1:
        blink1 = false;
        break;
    case 2:
        blink2 = false;
        break;
    case 3:
        blink3 = false;
        break;
    case 4:
        blink4 = false;
        break;
    
    default:
        blink1 = false;
        blink2 = false;
        blink3 = false;
        blink4 = false;
        break;
    }
}