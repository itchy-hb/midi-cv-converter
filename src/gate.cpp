#include "gate.h"

void gate::initialize(int gate_pin, int led_pin) {
    gatePin = gate_pin;
    ledPin = led_pin;
    pinMode(gatePin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(gatePin, LOW);
    digitalWrite(ledPin, LOW);
}

void gate::on() {
    digitalWrite(gatePin, HIGH);
    digitalWrite(ledPin, HIGH);
}

void gate::off() {
    digitalWrite(gatePin, LOW);
    digitalWrite(ledPin, LOW);
}