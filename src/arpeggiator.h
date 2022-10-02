#include <arp.h>
#include <blink.h>
#include <patternFactory.h>
using namespace std;

#define LED_PIN1 19
#define LED_PIN2 18
#define LED_PIN3 17
#define LED_PIN4 16
#define CLOCK_PIN 32

class arpeggiator {
    arp arp1;
    arp arp2;
    arp arp3;
    arp arp4;
    blink blinker;
    patternFactory patternFactory1;
    int pMode = 0;
    int beat_counter = 0;
    int note_counter = 0;
    int divRatio[6] = {96,48,24,12,6,3};
    int arpSpeedDiv = 2;
    int arpNotes[4] = {0};
    int arpNotesASC[4] = {0};
    int arpNotesDESC[4] = {0};

    int pattern0[8] = {0};
    int pattern1[8] = {0};
    int pattern2[8] = {0};
    int pattern3[8] = {0};

    int notes[4] = {0};

    bool isRunning = false;
    unsigned long clkOutputStartTime = 0;

    uint32_t laststep = 0;

  public:
    void initialize(arp,arp,arp,arp);
    void sortASC(int *a, int n);
    void reverse(int *a, int n);
    void setArpArray(int *a);
    void setArpNotes(int *noteSlotPlaying);
    void setArpNote(int);
    void setArpSpeed(int);
    void setPMode(int);
    void clockTick();
    void resetNotes();
    void start();
    void stop();
    void run();
    bool running();
    void startClockTick();
    void stopClockTick();
};