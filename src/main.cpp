#include <Arduino.h>
#include <MIDI.h>
#include <USBHost_t36.h>
#include <dac.h>

#define SWITCH1_PIN 31
#define SWITCH2_PIN 30
#define SWITCH3_PIN 38
#define SWITCH4_PIN 39
#define SWITCH5_PIN 40
#define SWITCH6_PIN 41

#define POT_PIN 14

#define SPI_CLK 13
#define SPI_MOSI 11
#define DAC1_CS_PIN 37
#define DAC2_CS_PIN 36
#define DAC3_CS_PIN 33
#define DAC4_CS_PIN 34

#define LED_PIN1 16
#define LED_PIN2 17
#define LED_PIN3 18
#define LED_PIN4 19

#define CLOCK_PIN 32

#define GATE1_PIN 27
#define GATE2_PIN 26
#define GATE3_PIN 25
#define GATE4_PIN 24

unsigned long clkOutputStartTime = 0;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

unsigned long t=0;
int switch1_state = LOW;
int switch1_last_state = LOW;
int switch2_state = LOW;
int switch2_last_state = LOW;
int switch3_state = LOW;
int switch3_last_state = LOW;
int switch4_state = LOW;
int switch4_last_state = LOW;
int switch5_state = LOW;
int switch5_last_state = LOW;
int switch6_state = LOW;
int switch6_last_state = LOW;

int midiChannel = 1;
bool on3 = false;
bool on4 = false;
bool on5 = false;
bool on6 = false;

USBHost myusb;
MIDIDevice keyboard(myusb);

byte noteSlotPlaying[4] = {0, 0, 0, 0};

dac dac1;
dac dac2;
dac dac3;
dac dac4;
dac dac5;
dac dac6;
dac dac7;
dac dac8;

int potiVal = 0;

void miditest();
void readSwitches();
void testDAC();
void readPoti();

void OnNoteOn(byte, byte, byte);
void OnNoteOff(byte, byte, byte);
void myClock();
void analogMidiReceive();

void startClockTick();
void stopClockTick();

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.begin(57600);

  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH3_PIN, INPUT_PULLUP);
  pinMode(SWITCH4_PIN, INPUT_PULLUP);
  pinMode(SWITCH5_PIN, INPUT_PULLUP);
  pinMode(SWITCH6_PIN, INPUT_PULLUP);

  // DAC SETUP
  pinMode(DAC1_CS_PIN, OUTPUT);
  pinMode(DAC2_CS_PIN, OUTPUT);
  pinMode(DAC3_CS_PIN, OUTPUT);
  pinMode(DAC4_CS_PIN, OUTPUT);

  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);

  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
  digitalWrite(LED_PIN3, LOW);
  digitalWrite(LED_PIN4, LOW);

  myusb.begin();
	keyboard.setHandleNoteOff(OnNoteOff);
	keyboard.setHandleNoteOn(OnNoteOn);
  keyboard.setHandleClock(myClock);

  usbMIDI.setHandleNoteOff(OnNoteOn);
  usbMIDI.setHandleNoteOn(OnNoteOff);
  usbMIDI.setHandleClock(myClock);

  dac1.initialize(DAC1_CS_PIN, 0);
  dac2.initialize(DAC1_CS_PIN, 1);
  dac3.initialize(DAC2_CS_PIN, 0);
  dac4.initialize(DAC2_CS_PIN, 1);
  dac5.initialize(DAC3_CS_PIN, 0);
  dac6.initialize(DAC3_CS_PIN, 1);
  dac7.initialize(DAC4_CS_PIN, 0);
  dac8.initialize(DAC4_CS_PIN, 1);
}

void loop() {
  analogMidiReceive();
  readSwitches();
  readPoti();
  testDAC();
  stopClockTick();

  myusb.Task();
	keyboard.read();
}

void readPoti() {
  int val = analogRead(POT_PIN);
  if (potiVal != val && (potiVal + 40 < val || potiVal - 40 > val)) {
    potiVal = val;
    Serial.print("POTI1: ");
    Serial.println(val);
  }
}


void setMidiChannel() {
  int ch = 0;
  if (on3) {
    ch += 1;
  }
  if (on4) {
    ch += 2;
  }
  if (on5) {
    ch += 4;
  }
  if (on6) {
    ch += 8;
  }
  midiChannel = ch;
}

void readSwitches() {
  switch1_state = digitalRead(SWITCH1_PIN);
  if (switch1_state != switch1_last_state) {
    switch1_last_state = switch1_state;

    if (switch1_state == HIGH) {
      Serial.println("SWITCH 1 ON");
    } else {
      Serial.println("SWITCH 1 OFF");
    }
  }
  switch2_state = digitalRead(SWITCH2_PIN);
  if (switch2_state != switch2_last_state) {
    switch2_last_state = switch2_state;

    if (switch2_state == HIGH) {
      Serial.println("SWITCH 2 ON");
    } else {
      Serial.println("SWITCH 2 OFF");
    }
  }

  switch3_state = digitalRead(SWITCH3_PIN);
  if (switch3_state != switch3_last_state) {
    switch3_last_state = switch3_state;

    if (switch3_state == HIGH) {
      on3 = true;
    } else {
      on3 = false;
    }
    setMidiChannel();
  }
  switch4_state = digitalRead(SWITCH4_PIN);
  if (switch4_state != switch4_last_state) {
    switch4_last_state = switch4_state;
    if (switch4_state == HIGH) {
      on4 = true;
    } else {
      on4 = false;
    }
    setMidiChannel();
  }
  switch5_state = digitalRead(SWITCH5_PIN);
  if (switch5_state != switch5_last_state) {
    switch5_last_state = switch5_state;
    if (switch5_state == HIGH) {
      on5 = true;
    } else {
      on5 = false;
    }
    setMidiChannel();
  }
  switch6_state = digitalRead(SWITCH6_PIN);
  if (switch6_state != switch6_last_state) {
    switch6_last_state = switch6_state;
    if (switch6_state == HIGH) {
      on6 = true;
    } else {
      on6 = false;
    }
    setMidiChannel();
  }
}

void analogMidiReceive() {
  int note, velocity, channel, d1, d2;
  if (MIDI.read()) {
    byte type = MIDI.getType();
    switch (type) {
      case midi::NoteOn:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        channel = MIDI.getChannel();
        OnNoteOn(channel, note, velocity);
        break;
      case midi::NoteOff:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        channel = MIDI.getChannel();
        OnNoteOff(channel, note, velocity);
        break;
      default:
        //GET MIDI CLOCK
        d1 = MIDI.getData1();
        d2 = MIDI.getData2();
        Serial.println(String("Message, type=") + type + ", data = " + d1 + " " + d2);
    }
    t = millis();
  }
  if (millis() - t > 10000) {
    t += 10000;
  }
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  if (channel == midiChannel) {
    int freeSlot = 0;
    bool hasFreeSlot = false;
    for (int i = 0; i < 4; i++) {
      if (noteSlotPlaying[i] == 0) {
        freeSlot = i;
        hasFreeSlot = true;
        break;
      }
    }
    if (hasFreeSlot) {
      noteSlotPlaying[freeSlot] = note;
      int n = (int)(note * 42.666);
      int v = velocity * 32;
      switch (freeSlot) {
        case 1:
          dac1.write(n);
          dac5.write(v);
          digitalWrite(GATE1_PIN, HIGH);
          digitalWrite(LED_PIN1, HIGH);
          break;
        case 2:
          dac2.write(n);
          dac6.write(v);
          digitalWrite(GATE2_PIN, HIGH);
          digitalWrite(LED_PIN2, HIGH);
          break;
        case 3:
          dac3.write(n);
          dac7.write(v);
          digitalWrite(GATE3_PIN, HIGH);
          digitalWrite(LED_PIN3, HIGH);
          break;
        case 4:
          dac4.write(n);
          dac8.write(v);
          digitalWrite(GATE4_PIN, HIGH);
          digitalWrite(LED_PIN4, HIGH);
          break;
        
        default:
          dac1.write(n);
          dac5.write(v);
          digitalWrite(GATE1_PIN, HIGH);
          digitalWrite(LED_PIN1, HIGH);
          break;
      }
    }
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  if (channel == midiChannel) {
    for (int i = 0; i < 4; i++) {
      if (noteSlotPlaying[i] == note) {
        noteSlotPlaying[i] = 0;
        switch (i) {
          case 1:
            digitalWrite(GATE1_PIN, LOW);
            digitalWrite(LED_PIN1, LOW);
            break;
          case 2:
            digitalWrite(GATE2_PIN, LOW);
            digitalWrite(LED_PIN2, LOW);
            break;
          case 3:
            digitalWrite(GATE3_PIN, LOW);
            digitalWrite(LED_PIN3, LOW);
            break;
          case 4:
            digitalWrite(GATE4_PIN, LOW);
            digitalWrite(LED_PIN4, LOW);
            break;
          
          default:
            digitalWrite(GATE1_PIN, LOW);
            digitalWrite(LED_PIN1, LOW);
            break;
        }
      }
    }
  }
}

void myClock() {
  startClockTick();
}

void startClockTick() {
  if (clkOutputStartTime == 0) {
    digitalWrite(CLOCK_PIN, HIGH);
    clkOutputStartTime = millis();
  }
}

void stopClockTick() {
  if (clkOutputStartTime - millis() >= 10) {
    digitalWrite(CLOCK_PIN, LOW);
    clkOutputStartTime = 0;
  }
}

// remove
int dac_loop_counter = 1;
void testDAC() {
  dac_loop_counter++;
  if (dac_loop_counter >= 4095) {
    dac_loop_counter = 1;
  }

  dac1.write(dac_loop_counter);
  dac2.write(dac_loop_counter);
  dac3.write(dac_loop_counter);
  dac4.write(dac_loop_counter);
  dac5.write(dac_loop_counter);
  dac6.write(dac_loop_counter);
  dac7.write(dac_loop_counter);
  dac8.write(dac_loop_counter);
}
