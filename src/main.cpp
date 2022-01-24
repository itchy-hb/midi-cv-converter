#include <Arduino.h>
#include <MIDI.h>
#include <SPI.h>

#define SWITCH1_PIN 31
#define SWITCH2_PIN 30

#define SPI_CLK 13
#define SPI_MOSI 11
#define DAC1_CS_PIN 37
#define DAC2_CS_PIN 36
#define DAC3_CS_PIN 33
#define DAC4_CS_PIN 34
int gain = 0;


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

unsigned long t=0;
int switch1_state = LOW;
int switch1_last_state = LOW;
int switch2_state = LOW;
int switch2_last_state = LOW;

void testDAC();
void writeDAC(int cs, int dac, int val);
void miditest();
void switchtest();
void setOutput(unsigned int val);
void setOutput(byte channel, byte gain, byte shutdown, unsigned int val);

void setup() {
  //MIDI INPUT SETUP
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.begin(57600);

  // DAC SETUP
  pinMode(DAC1_CS_PIN, OUTPUT);
  pinMode(DAC2_CS_PIN, OUTPUT);
  pinMode(DAC3_CS_PIN, OUTPUT);
  pinMode(DAC4_CS_PIN, OUTPUT);
  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2);
}


void loop() {
  miditest();
  switchtest();
  testDAC();
  
  delay(1);
}

int dac_loop_counter = 1;
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

void writeDAC(int cs, int dac, int val) {
  digitalWrite(cs, LOW);
  dac = dac & 1;
  val = val & 4095;
  SPI.transfer(dac << 7 | gain << 5 | 1 << 4 | val >> 8);
  SPI.transfer(val & 255);
  digitalWrite(cs, HIGH);
}

void switchtest() {
  switch1_state = digitalRead(SWITCH1_PIN);
  switch2_state = digitalRead(SWITCH2_PIN);
  if (switch1_state != switch1_last_state) {
    switch1_last_state = switch1_state;

    Serial.print("SWITCH 1 ");

    if (switch1_state == HIGH) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  }
  if (switch2_state != switch2_last_state) {
    switch2_last_state = switch2_state;

    Serial.print("SWITCH 2 ");

    if (switch2_state == HIGH) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  }
}

void miditest() {
  int type, note, velocity, channel, d1, d2;
  if (MIDI.read()) {                    // Is there a MIDI message incoming ?
    byte type = MIDI.getType();
    switch (type) {
      case midi::NoteOn:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        channel = MIDI.getChannel();
        if (velocity > 0) {
          Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
        } else {
          Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
        }
        break;
      case midi::NoteOff:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        channel = MIDI.getChannel();
        Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
        break;
      default:
        d1 = MIDI.getData1();
        d2 = MIDI.getData2();
        Serial.println(String("Message, type=") + type + ", data = " + d1 + " " + d2);
    }
    t = millis();
  }
  if (millis() - t > 10000) {
    t += 10000;
    Serial.println("(inactivity)");
  }
}

void setOutput(unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x10;
   
  PORTB &= 0xfb;
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  PORTB |= 0x4;
}
 
void setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;
   
  PORTB &= 0xfb;
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  PORTB |= 0x4;
}