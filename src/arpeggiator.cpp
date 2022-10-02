#include "arpeggiator.h"

void arpeggiator::initialize(arp a1, arp a2, arp a3, arp a4) {
  arp1 = a1;
  arp2 = a2;
  arp3 = a3;
  arp4 = a4;
  note_counter = 0;
  patternFactory1.initialize();
  blinker.initialize(LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4);
  resetNotes();
}

void arpeggiator::resetNotes() {
  for (int i = 0; i < 4; i++) {
    notes[i] = 0;
  }
  setArpArray(notes);
}

void arpeggiator::setArpNote(int note) {
  Serial.println("setArpNote");
  
  if (note_counter >= 4) {
    note_counter = 0;
    resetNotes();
  }

  notes[note_counter] = note;

  blinker.setOff(5);
  blinker.setOn(note_counter + 1);

  setArpArray(notes);

  note_counter++;
}

void arpeggiator::setArpNotes(int *noteSlotPlaying) {
  Serial.println("setArpNotes");
  setArpArray(noteSlotPlaying);
}

void arpeggiator::setArpArray(int *a) {
  for (int i = 0; i < 4; i++) {
    arpNotes[i] = a[i];
  }

  patternFactory1.setPatternMode(pMode);
  patternFactory1.setNotes(arpNotes);
  patternFactory1.getPattern(pattern0, 0);
  patternFactory1.getPattern(pattern1, 1);
  patternFactory1.getPattern(pattern2, 2);
  patternFactory1.getPattern(pattern3, 3);

  Serial.print("PATTERN 1");
  arp1.setArp(pattern0);
  Serial.print("PATTERN 2");
  arp2.setArp(pattern1);
  Serial.print("PATTERN 3");
  arp3.setArp(pattern2);
  Serial.print("PATTERN 4");
  arp4.setArp(pattern3);
  Serial.println();

  Serial.print(" ARPNOTES: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(i);
    Serial.print("-");
    Serial.print(arpNotes[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void arpeggiator::setArpSpeed(int potiVal) {
  arpSpeedDiv = map(potiVal, 0, 1024, 0, 5);
}

void arpeggiator::setPMode(int mode) {
  pMode = mode;
  patternFactory1.setPatternMode(pMode);
  patternFactory1.setNotes(arpNotes);
  patternFactory1.getPattern(pattern0, 0);
  patternFactory1.getPattern(pattern1, 1);
  patternFactory1.getPattern(pattern2, 2);
  patternFactory1.getPattern(pattern3, 3);

  arp1.setArp(pattern0);
  arp2.setArp(pattern1);
  arp3.setArp(pattern2);
  arp4.setArp(pattern3);
}

void arpeggiator::clockTick() {
    if (beat_counter == 0) {
        Serial.println("STEP");
        arp1.playStep1();
        arp2.playStep1();
        arp3.playStep1();
        arp4.playStep1();
    }
    beat_counter++;
    if (beat_counter >= divRatio[arpSpeedDiv]) {
      beat_counter = 0;
    }

    if (beat_counter >= divRatio[arpSpeedDiv]/2+1) {
      arp1.playStepOff();
      arp2.playStepOff();
      arp3.playStepOff();
      arp4.playStepOff();
    }
}

int sort_desc(const void *cmp1, const void *cmp2) {
  // Need to cast the void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
}

void arpeggiator::sortASC(int *a, int n) {
  qsort(a, n, sizeof(a[0]), sort_desc);
  reverse(a, n);
}

void arpeggiator::reverse(int *a, int n) {
  int s[4];
  for (int i = 0; i < n; i++) {
    s[n-1-i] = a[i];
  }
  for (int i = 0; i < n; i++) {
    a[i] = s[i];
  }
}

void arpeggiator::start() {
  isRunning = true;
  laststep = 0;
  blinker.setOff(5);
}

void arpeggiator::stop() {
  isRunning = false;
  blinker.setOff(5);
}

bool arpeggiator::running() {
  return isRunning;
}

void arpeggiator::run() {
  uint32_t maxmillis = 200;
  if (millis() - laststep > maxmillis) {
    clockTick();
    startClockTick();
    laststep = millis();
  }
  //implement timer
  //clockTick(); + startClockTick();

  stopClockTick();
  blinker.process();
}

void arpeggiator::startClockTick() {
  if (clkOutputStartTime == 0) {
    digitalWrite(CLOCK_PIN, HIGH);
    clkOutputStartTime = millis();
  }
}

void arpeggiator::stopClockTick() {
  if (clkOutputStartTime - millis() >= 10) {
    digitalWrite(CLOCK_PIN, LOW);
    clkOutputStartTime = 0;
  }
}