#include <dac.h>
#include <gate.h>
using namespace std;

#define ARPSIZE 8

class arp {
    dac this_note;
    dac this_velocity;
    gate this_gate;
    int pattern1[ARPSIZE];
    int pattern2[ARPSIZE];
    int lastIndex;
    int arpCount1;
    int arpCount2;

  public:
    void initialize(dac,dac,gate);
    boolean hasValue(int *a, int n);
    int countValue(int *a, int n);
    void fillArpArray1(int *a, int n);
    void fillArpArray2(int *a, int n);
    void playStep(int *pattern, int arpCount);
    void playStep1();
    void playStepUpDown();
    void setArp(int *arpArray1);
    void playStepOff();
    void reset();
};