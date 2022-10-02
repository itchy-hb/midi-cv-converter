#include "arp.h"

void arp::initialize(dac note, dac velocity, gate gate) {
  this_note = note;
  this_gate = gate;
  this_velocity = velocity;
  reset();
}

void arp::setArp(int *arpArray1) {
  reset();
  fillArpArray1(arpArray1, ARPSIZE);
  arpCount1 = countValue(arpArray1, ARPSIZE);
  for (int i = 0; i < 8; ++i) {
    Serial.print(arpArray1[i]);
    Serial.print(" | ");
  }
  Serial.println();
}

void arp::playStep1() {
  playStep(pattern1, arpCount1);
}

void arp::playStep(int *pattern, int arpCount) {
  if (hasValue(pattern, ARPSIZE)) {

    int index1 = lastIndex + 1;
    if (index1 >= arpCount) {
      index1 = 0;
    }
    lastIndex = index1;

    Serial.print("STEP: ");
    Serial.println(index1);
    Serial.print("PLAY NOTE ARP: ");
    Serial.println(pattern[index1]);


    int n = (int)(pattern[index1] * 42.666);
    int v = 87 * 32;

    this_note.write(n);
    this_velocity.write(v);
    this_gate.on();
  } 
}

void arp::playStepOff() {
  this_gate.off();
}

void arp::reset() {
  lastIndex = -1;
  arpCount1 = 0;
}

boolean arp::hasValue(int *a, int n) {
  boolean hasVal = false;
  for (int i = 0; i < n; ++i) {
    if (a[i] > 0) {
      hasVal = true;
    }
  }
  return hasVal;
}

int arp::countValue(int *a, int n) {
  int countVal = 0;
  for (int i = 0; i < n; ++i) {
    if (a[i] > 0) {
      countVal++;
    }
  }
  return countVal;
}

void arp::fillArpArray1(int *a, int n) {
  int counter = 0;
  for (int i = 0; i < n; ++i) {
    if (a[i] > 0) {
      pattern1[counter] = a[i];
      counter++;
    }
  }
}