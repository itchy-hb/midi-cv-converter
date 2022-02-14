#include <Arduino.h>
using namespace std;

class gate {
    int gatePin;
    int ledPin;

  public:
    void initialize(int,int);
    void on();
    void off();
};