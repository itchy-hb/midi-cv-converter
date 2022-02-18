#include <Arduino.h>
#include <MIDI.h>
#include <USBHost_t36.h>
#include <dac.h>
#include <gate.h>

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

#define LED_PIN4 16
#define LED_PIN3 17
#define LED_PIN2 18
#define LED_PIN1 19

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

int potiVal = 0;

void readSwitches();
void readPoti();

void OnNoteOn(byte, byte, byte);
void OnNoteOff(byte, byte, byte);
void OnClockTick();
void analogMidiReceive();

void startClockTick();
void stopClockTick();
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

  note1.initialize(DAC2_CS_PIN, 0);
  note2.initialize(DAC2_CS_PIN, 1);
  note3.initialize(DAC1_CS_PIN, 0);
  note4.initialize(DAC1_CS_PIN, 1);
  velocity1.initialize(DAC3_CS_PIN, 1);
  velocity2.initialize(DAC3_CS_PIN, 0);
  velocity3.initialize(DAC4_CS_PIN, 1);
  velocity4.initialize(DAC4_CS_PIN, 0);
  gate1.initialize(GATE1_PIN,LED_PIN1);
  gate2.initialize(GATE2_PIN,LED_PIN2);
  gate3.initialize(GATE3_PIN,LED_PIN3);
  gate4.initialize(GATE4_PIN,LED_PIN4);
}

void loop() {
  analogMidiReceive();
  readSwitches();
  readPoti();
  stopClockTick();

  myusb.Task();
	keyboard.read();
}

int smoothSize = 33;
int potivalSmooth[33] = {9999};
int potivalIterator = 0;
int potireadSchedule = 0;
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
        setMidiChannel();
      }
      

      potivalIterator++;
      if (potivalIterator >= smoothSize) {
        potivalIterator = 0;
      }
    }
  }

}

int programMode = 0;
int midiMode = 0;
int switchSchedule = 0;
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
      digitalWrite(LED_PIN1, LOW);
      digitalWrite(LED_PIN2, LOW);
      digitalWrite(LED_PIN3, LOW);
      digitalWrite(LED_PIN4, LOW);
      Serial.print("MIDI MODE:");
      Serial.println(midiMode);
    }
    switch2_state = digitalRead(SWITCH2_PIN);
    if (switch2_state != switch2_last_state) {
      switch2_last_state = switch2_state;

      if (switch2_state == HIGH) {

        programMode++;
        if (programMode >= 3) {
          programMode = 0;
        }

      } else {

      }
      digitalWrite(LED_PIN1, LOW);
      digitalWrite(LED_PIN2, LOW);
      digitalWrite(LED_PIN3, LOW);
      digitalWrite(LED_PIN4, LOW);
      Serial.print("PRG MODE:");
      Serial.println(programMode);
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

int midiChannels[4] = {1,2,3,4};
int setupChannel = 0;
void setMidiChannel() {
  if (programMode == 1 && midiMode == 0) {
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
  if (programMode == 2 && midiMode == 0) {
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

int noteChannels[4] = {0,0,0,0};
void OnNoteOn(byte channel, byte note, byte velocity) {
  int n = (int)(note * 42.666);
  int v = velocity * 32;
  Serial.println(channel);

  if (programMode == 0 && midiMode == 0) {
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
  if (programMode == 0 && midiMode == 1) {
  //if (channel == midiChannel) {
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
      Serial.print("FREESLOT: ");
      Serial.print(freeSlot);
      Serial.print(" NOTE: ");
      Serial.print(n);
      Serial.print(" NOTE PLAY: ");
      Serial.println(noteSlotPlaying[freeSlot]);
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
  setArpNotes();
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  if (programMode == 0 && midiMode == 0) {
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
  if (programMode == 0 && midiMode == 1) {
  //if (channel == midiChannel) {
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

int arpNotesASC[4] = {0}
int arpNotesDESC[4] = {0}
int arpNotesRandom[4] = {0}
int arpRandoms[4] = {9999}

void setArpNotes() {
  
  for (int i = 0; i < 4; i++) {
    int random = rand() % 1000;
    arpRandoms[i] = random;
  }

  int a = 3;
  Serial.print("RANDOM: ");
  for (int i = 0; i < 4; i++) {
    arpNotesASC[i] = noteChannels[i];
    arpNotesDESC[a] = noteChannels[i];
    a--;

    int smallest = 9999;
    int iteratorSmallest = 0;
    for (int x = 0; x < 4; x++) {
      if (arpRandoms[x] <= smallest && arpRandoms[x] != 9999) {
        smallest = arpRandoms[x];
        iteratorSmallest = x;
      }
    }
    arpNotesRandom[i] = smallest;
    arpRandoms[iteratorSmallest] = 9999;
    Serial.print(i);
    Serial.print("-");
    Serial.print(arpNotesRandom[i]);
    Serial.print(" ");
  }
  Serial.print(" ARPNOTES: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(i);
    Serial.print("-");
    Serial.print(arpNotesASC[i]);
    Serial.print(" ");
  }
  Serial.print(" ARPNOTESREVERS: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(i);
    Serial.print("-");
    Serial.print(arpNotesDESC[i]);
    Serial.print(" ");
  }
  Serial.println();
}