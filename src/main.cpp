#include <Arduino.h>
#include <MIDI.h>
#include <USBHost_t36.h>
#include <arpeggiator.h>

#define SWITCH1_PIN 31
#define SWITCH2_PIN 30

#define PRG_BTN_PIN 38
#define PRG_LED_PIN 39

#define LED_PIN1 19
#define LED_PIN2 18
#define LED_PIN3 17
#define LED_PIN4 16

#define POT_PIN 14

#define SPI_CLK 13
#define SPI_MOSI 11

#define DAC1_CS_PIN 36
#define DAC2_CS_PIN 37
#define DAC3_CS_PIN 33
#define DAC4_CS_PIN 34

#define GATE1_PIN 27
#define GATE2_PIN 26
#define GATE3_PIN 25
#define GATE4_PIN 24

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

USBHost myusb;
MIDIDevice keyboard(myusb);

dac note1;
dac note2;
dac note3;
dac note4;
dac velocity1;
dac velocity2;
dac velocity3;
dac velocity4;

gate gate1;
gate gate2;
gate gate3;
gate gate4;

arp arp1;
arp arp2;
arp arp3;
arp arp4;

arpeggiator arpeggiator1;

int potiVal = 0;

int smoothSize = 33;
int potivalSmooth[33] = {9999};
int potivalIterator = 0;
int potireadSchedule = 0;

int programMode = 0;
int midiMode = 0;
int arpMode = 0;
int switchSchedule = 0;

int midiChannels[4] = {1,2,3,4};
int setupChannel = 0;

int noteSlotPlaying[4] = {0, 0, 0, 0};
int noteChannels[4] = {0,0,0,0};

unsigned long t=0;
int switch1_state = LOW;
int switch1_last_state = LOW;
int switch2_state = LOW;
int switch2_last_state = LOW;
int switch3_state = LOW;
int switch3_last_state = LOW;

int midiChannel = 1;

void readSwitches();
void readPoti();

void OnNoteOn(byte, byte, byte);
void OnNoteOff(byte, byte, byte);
void OnClockTick();
void analogMidiReceive();

void setMidiChannel();

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.begin(57600);

  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);

  pinMode(CLOCK_PIN, OUTPUT);

  myusb.begin();
	keyboard.setHandleNoteOff(OnNoteOff);
	keyboard.setHandleNoteOn(OnNoteOn);
  keyboard.setHandleClock(OnClockTick);

  usbMIDI.setHandleNoteOff(OnNoteOn);
  usbMIDI.setHandleNoteOn(OnNoteOff);
  usbMIDI.setHandleClock(OnClockTick);

  note1.initialize(DAC1_CS_PIN, 0);
  note2.initialize(DAC1_CS_PIN, 1);
  note3.initialize(DAC2_CS_PIN, 0);
  note4.initialize(DAC2_CS_PIN, 1);

  velocity1.initialize(DAC3_CS_PIN, 1);
  velocity2.initialize(DAC3_CS_PIN, 0);
  velocity3.initialize(DAC4_CS_PIN, 1);
  velocity4.initialize(DAC4_CS_PIN, 0);

  gate1.initialize(GATE1_PIN,LED_PIN1);
  gate2.initialize(GATE2_PIN,LED_PIN2);
  gate3.initialize(GATE3_PIN,LED_PIN3);
  gate4.initialize(GATE4_PIN,LED_PIN4);

  arp1.initialize(note1, velocity1, gate1);
  arp2.initialize(note2, velocity2, gate2);
  arp3.initialize(note3, velocity3, gate3);
  arp4.initialize(note4, velocity4, gate4);

  arpeggiator1.initialize(arp1, arp2, arp3, arp4);
}

void loop() {

  analogMidiReceive();
  readSwitches();
  readPoti();
  arpeggiator1.run();

  myusb.Task();
	//keyboard.read();

int incomingType = 0;
  if (keyboard.read()) {   // Is there a MIDI message incoming ?
     incomingType = keyboard.getType();
     Serial.println(incomingType);
  }

  delayMicroseconds(1);
}

void readPoti() {
  potireadSchedule++;
  if (potireadSchedule > 5000) {
    potireadSchedule = 0;
    int val = analogRead(POT_PIN);
    int smoothval = 0;
    if (potiVal != val) {
      potivalSmooth[potivalIterator] = val;
      int divider = 0;
      for (int i = 0; i < smoothSize; i++) {
        if (potivalSmooth[i] != 9999) {
          divider++;
          smoothval += potivalSmooth[i];
        }
      }

      if (divider > 0) {
        smoothval = smoothval / divider;
      } else {
        smoothval = val;
      }

      if (potiVal != smoothval && (potiVal + 5 < smoothval || potiVal - 5 > smoothval)) {
        Serial.print("POTI1: ");
        Serial.println(potiVal);
        potiVal = smoothval;

        if (arpMode == 0) {
          setMidiChannel();
        }
        if (arpMode == 1) {
         arpeggiator1.setArpSpeed(potiVal);
        }
      }

      potivalIterator++;
      if (potivalIterator >= smoothSize) {
        potivalIterator = 0;
      }
    }
  }

}

void readSwitches() {
  switchSchedule++;
  if (switchSchedule > 5000) {
    switchSchedule = 0;
    
    switch1_state = digitalRead(SWITCH1_PIN);
    if (switch1_state != switch1_last_state) {
      switch1_last_state = switch1_state;
      if (switch1_state == HIGH) {
        midiMode = 1;
      } else {
        midiMode = 0;
      }
    }

    switch2_state = digitalRead(SWITCH2_PIN);
    if (switch2_state != switch2_last_state) {
      switch2_last_state = switch2_state;
      if (switch2_state == HIGH) {
        arpMode = 1;
        arpeggiator1.start();
      } else {
        arpMode = 0;
        arpeggiator1.stop();
      }
      programMode = 0;
      digitalWrite(PRG_LED_PIN, LOW);
      digitalWrite(LED_PIN1, LOW);
      digitalWrite(LED_PIN2, LOW);
      digitalWrite(LED_PIN3, LOW);
      digitalWrite(LED_PIN4, LOW);
      arpeggiator1.resetNotes();
    }

    switch3_state = digitalRead(PRG_BTN_PIN);
    if (switch3_state != switch3_last_state) {
      switch3_last_state = switch3_state;

      if (switch3_state == HIGH) {
        programMode++;
        if (programMode >= 3) {
          programMode = 0;
        }
        if (arpMode == 1) {
          arpeggiator1.setPMode(programMode);
          int testnotes[4] = {10,100,20,120};
          arpeggiator1.setArpNotes(testnotes);
        }
      }

      if (programMode == 0) {
        digitalWrite(PRG_LED_PIN, LOW);
        digitalWrite(LED_PIN1, LOW);
        digitalWrite(LED_PIN2, LOW);
        digitalWrite(LED_PIN3, LOW);
        digitalWrite(LED_PIN4, LOW);
      } else {
        digitalWrite(PRG_LED_PIN, HIGH);
      }
    }
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

void setMidiChannel() {
  if (programMode == 1 && midiMode == 0) {
    // Select output to set the midi channel
    setupChannel = map(potiVal, 0, 1023, 0, 3);
    digitalWrite(LED_PIN1, LOW);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);
    digitalWrite(LED_PIN4, LOW);
    switch (setupChannel) {
      case 0:
        digitalWrite(LED_PIN1, HIGH);
        break;
      case 1:
        digitalWrite(LED_PIN2, HIGH);
        break;
      case 2:
        digitalWrite(LED_PIN3, HIGH);
        break;
      case 3:
        digitalWrite(LED_PIN4, HIGH);
        break;
    }
  }
  if (programMode == 2 || (programMode == 1 && midiMode == 1)) {
    if (programMode == 1 && midiMode == 1) {
      setupChannel = 0;
    }
    //if (programMode == 2 && midiMode == 0) {
    // Set the midi channel on selected output
    int setChannel = map(potiVal, 0, 1023, 1, 16);
    midiChannels[setupChannel] = setChannel;
    digitalWrite(LED_PIN1, LOW);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);
    digitalWrite(LED_PIN4, LOW);
    switch (setChannel) {
      case 1:
        digitalWrite(LED_PIN1, HIGH);
        break;
      case 2:
        digitalWrite(LED_PIN2, HIGH);
        break;
      case 3:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN2, HIGH);
        break;
      case 4:
        digitalWrite(LED_PIN3, HIGH);
        break;
      case 5:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        break;
      case 6:
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        break;
      case 7:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        break;
      case 8:
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 9:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 10:
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 11:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 12:
        digitalWrite(LED_PIN3, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 13:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 14:
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 15:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
      case 16:
        digitalWrite(LED_PIN1, HIGH);
        digitalWrite(LED_PIN2, HIGH);
        digitalWrite(LED_PIN3, HIGH);
        digitalWrite(LED_PIN4, HIGH);
        break;
    }
  }
  for (int i = 0; i < 4; i++) {
    Serial.print(" CH");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(midiChannels[i]);
  }
  Serial.println();
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  int n = (int)(note * 42.666);
  int v = velocity * 32;

  if (programMode == 0 && midiMode == 0 && arpMode == 0) {
    for (int i = 0; i < 4; i++) {
      if (midiChannels[i] == channel && noteChannels[i] == 0) {
        noteChannels[i] = note;

        switch (i) {
          case 0:
            note1.write(n);
            velocity1.write(v);
            gate1.on();
            break;
          case 1:
            note2.write(n);
            velocity2.write(v);
            gate2.on();
            break;
          case 2:
            note3.write(n);
            velocity3.write(v);
            gate3.on();
            break;
          case 3:
            note4.write(n);
            velocity4.write(v);
            gate4.on();
            break;
          
          default:
            note1.write(n);
            velocity1.write(v);
            gate1.on();
            break;
        }
        
      }
    }
  }
  if ((programMode == 0 && midiMode == 1) || arpMode == 1) {
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
      if (arpMode == 0) {
        switch (freeSlot) {
          case 0:
            note1.write(n);
            velocity1.write(v);
            gate1.on();
            break;
          case 1:
            note2.write(n);
            velocity2.write(v);
            gate2.on();
            break;
          case 2:
            note3.write(n);
            velocity3.write(v);
            gate3.on();
            break;
          case 3:
            note4.write(n);
            velocity4.write(v);
            gate4.on();
            break;
          
          default:
            note1.write(n);
            velocity1.write(v);
            gate1.on();
            break;
        }
      }
    }
  }
  if (arpMode == 1) {
    if (midiMode == 0) {
      for (int i = 0; i < 4; i++) {
        Serial.print(noteSlotPlaying[i]);
        Serial.print(":");
      }
      Serial.println();

      arpeggiator1.setArpNotes(noteSlotPlaying);
    } else {
      arpeggiator1.setArpNote(note);
    }
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  if (programMode == 0 && midiMode == 0 && arpMode == 0) {
    for (int i = 0; i < 4; i++) {
      if (midiChannels[i] == channel && noteChannels[i] == note) {
        noteChannels[i] = 0;
        switch (i) {
          case 0:
            gate1.off();
            break;
          case 1:
            gate2.off();
            break;
          case 2:
            gate3.off();
            break;
          case 3:
            gate4.off();
            break;
          
          default:
            gate1.off();
            break;
        }
      }
    }
  }
  if (programMode == 0 && midiMode == 1 && arpMode == 0) {
    for (int i = 0; i < 4; i++) {
      if (noteSlotPlaying[i] == note) {
        noteSlotPlaying[i] = 0;
        switch (i) {
          case 0:
            gate1.off();
            break;
          case 1:
            gate2.off();
            break;
          case 2:
            gate3.off();
            break;
          case 3:
            gate4.off();
            break;
          
          default:
            gate1.off();
            break;
        }
      }
    }
  }
}

void OnClockTick() {
  if (arpMode == 1) {
    arpeggiator1.clockTick();
  }
  if (arpeggiator1.running()) {
    arpeggiator1.stop();
  }
  arpeggiator1.startClockTick();
}