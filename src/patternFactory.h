#include <Arduino.h>
using namespace std;



class patternFactory {

  int notes[4] = {0};
  int notesASC[4] = {0};
  int numberOfNotes = 0;
  int patterMode = 0;
  int pattern[4][8] = {{0},{0},{0},{0}};

  public:
    void initialize();
    void reset();
    void setNotes(int *notesArray);
    void sortASC(int *a, int n);
    void reverse(int *a, int n);
    void getPattern(int *patterns, int channel);
    void generatePatternOneNote();
    void generatePatternTwoNotes();
    void generatePatternThreeNotes();
    void generatePatternVourNotes();
    void generatePattern();
    void clearPatterns();
    void setPatternMode(int mode);
};