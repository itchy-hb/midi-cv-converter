#include <Arduino.h>
using namespace std;

class blink {
    int led1;
    int led2;
    int led3;
    int led4;
    bool blink1;
    bool blink2;
    bool blink3;
    bool blink4;
    int onTime[4];

  public:
    void initialize(int,int,int,int);
    void process();
    void blinkIt(int, bool, int);
    void setOn(int);
    void setOff(int);
};