#include "dac.h"

void dac::initialize(int chipSelect, int dacOutput) {
    cs = chipSelect;
    dac = dacOutput;
    gain = 0;
    SPI.begin();  
    SPI.setClockDivider(SPI_CLOCK_DIV2);
}

void dac::write(int val) {
  digitalWrite(cs, LOW);
  dac = dac & 1;
  val = val & 4095;
  SPI.transfer(dac << 7 | gain << 5 | 1 << 4 | val >> 8);
  SPI.transfer(val & 255);
  digitalWrite(cs, HIGH);
}