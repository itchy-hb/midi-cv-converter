#include <Arduino.h>
#include <MIDI.h>
#include <SPI.h>
#include <USBHost_t36.h>

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
int gain = 0;

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

int dac_loop_counter = 1;

int potiVal = 0;

void miditest();
void readSwitches();
void testDAC();
void writeDAC(int, int, int);
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

  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  myusb.begin();
	keyboard.setHandleNoteOff(OnNoteOff);
	keyboard.setHandleNoteOn(OnNoteOn);
  keyboard.setHandleClock(myClock);

  usbMIDI.setHandleNoteOff(OnNoteOn);
  usbMIDI.setHandleNoteOn(OnNoteOff);
  usbMIDI.setHandleClock(myClock);
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

void testDAC() {
  dac_loop_counter++;
  if (dac_loop_counter >= 4095) {
    dac_loop_counter = 1;
  }
  
  writeDAC(DAC1_CS_PIN, 0, dac_loop_counter);
  writeDAC(DAC1_CS_PIN, 1, dac_loop_counter);

  writeDAC(DAC2_CS_PIN, 0, dac_loop_counter);
  writeDAC(DAC2_CS_PIN, 1, dac_loop_counter);

  writeDAC(DAC3_CS_PIN, 0, dac_loop_counter);
  writeDAC(DAC3_CS_PIN, 1, dac_loop_counter);

  writeDAC(DAC4_CS_PIN, 0, dac_loop_counter);
  writeDAC(DAC4_CS_PIN, 1, dac_loop_counter);
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
      switch (freeSlot) {
        case 1:
          writeDAC(DAC1_CS_PIN, 0, (int)(note * 42.666));
          writeDAC(DAC3_CS_PIN, 0, velocity * 32);
          digitalWrite(GATE1_PIN, HIGH);
          digitalWrite(LED_PIN1, HIGH);
          break;
        case 2:
          writeDAC(DAC1_CS_PIN, 1, (int)(note * 42.666));
          writeDAC(DAC3_CS_PIN, 1, velocity * 32);
          digitalWrite(GATE2_PIN, HIGH);
          digitalWrite(LED_PIN2, HIGH);
          break;
        case 3:
          writeDAC(DAC2_CS_PIN, 0, (int)(note * 42.666));
          writeDAC(DAC4_CS_PIN, 0, velocity * 32);
          digitalWrite(GATE3_PIN, HIGH);
          digitalWrite(LED_PIN3, HIGH);
          break;
        case 4:
          writeDAC(DAC2_CS_PIN, 1, (int)(note * 42.666));
          writeDAC(DAC4_CS_PIN, 1, velocity * 32);
          digitalWrite(GATE4_PIN, HIGH);
          digitalWrite(LED_PIN4, HIGH);
          break;
        
        default:
          writeDAC(DAC1_CS_PIN, 0, (int)(note * 42.666));
          writeDAC(DAC3_CS_PIN, 0, velocity * 32);
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

void writeDAC(int cs, int dac, int val) {
  digitalWrite(cs, LOW);
  dac = dac & 1;
  val = val & 4095;
  SPI.transfer(dac << 7 | gain << 5 | 1 << 4 | val >> 8);
  SPI.transfer(val & 255);
  digitalWrite(cs, HIGH);
}