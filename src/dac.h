#include <SPI.h>
using namespace std;

class dac {
    int cs;
    int dac;
    int gain;

  public:
    void initialize(int,int);
    void write(int);
};