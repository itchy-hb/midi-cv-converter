#include "patternFactory.h"

void patternFactory::initialize() {
    reset();
}

void patternFactory::reset() {
    clearPatterns();

    for (int i = 0; i < 4; i++) {
        notes[i] = {0};
    }
    for (int i = 0; i < 4; i++) {
        notesASC[i] = {0};
    }

    numberOfNotes = 0;
}

void patternFactory::setNotes(int *notesArray) {
    reset();
    for (int i = 0; i < 4; i++) {
        int note = notesArray[i];

        if (note > 0) {
            numberOfNotes++;
        }
        
        notes[i] = note;
        notesASC[i] = note;
    }
    sortASC(notesASC, 4);

    generatePattern();
}

void patternFactory::generatePattern() {
    switch (numberOfNotes) {
        case 1:
          generatePatternOneNote();
          break;
        case 2:
          generatePatternTwoNotes();
          break;
        case 3:
          generatePatternThreeNotes();
          break;
        case 4:
          generatePatternVourNotes();
          break;
        
        default:
          clearPatterns();
          break;
    }
}

int sort_desc_pfactory(const void *cmp1, const void *cmp2) {
  // Need to cast the void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
}

void patternFactory::sortASC(int *a, int n) {
  qsort(a, n, sizeof(a[0]), sort_desc_pfactory);
  reverse(a, n);
}

void patternFactory::reverse(int *a, int n) {
  int s[4];
  for (int i = 0; i < n; i++) {
    s[n-1-i] = a[i];
  }
  for (int i = 0; i < n; i++) {
    a[i] = s[i];
  }
}

void patternFactory::setPatternMode(int mode) {
  patterMode = mode;
}

void patternFactory::getPattern(int *patterns, int channel) {
    for (int j = 0; j < 8; j++) {
        patterns[j] = pattern[channel][j];
    }
}

void patternFactory::clearPatterns() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++) {
      pattern[i][j] = 0;
    }
  }
}

void patternFactory::generatePatternOneNote() {
  int note = 0;
  clearPatterns();
  
  for (int i = 0; i < 4; i++) {
    if (notes[i] != 0) {
      note = notes[i];
      break;
    }
  }

  switch (patterMode) {
    case 0:
      // Fill pattern ARP A - channel 1 : A
      pattern[0][0] = note;

      // Fill pattern ARP A - channel 2 :  A - A+4 (UP 1)
      pattern[1][0] = note;
      pattern[1][1] = note+4;

      // Fill pattern ARP A - channel 3 :  A - A+4 - A+7  (UP 2)
      pattern[2][0] = note;
      pattern[2][1] = note+4;
      pattern[2][2] = note+7;

      // Fill pattern ARP A - channel 4 :  A+7 - A+4 - A   (DOWN)
      pattern[3][0] = note+7;
      pattern[3][1] = note+4;
      pattern[3][2] = note;
      break;
    case 1:
      // Fill pattern ARP B - channel 1 :  A - A+4 - A+7 - A+4   (UP-DOWN)
      pattern[0][0] = note;
      pattern[0][1] = note+4;
      pattern[0][2] = note+7;
      pattern[0][3] = note+4;

      // Fill pattern ARP B - channel 2 :  A+7 - A+4 - A - A+4   (DOWN-UP)
      pattern[1][0] = note+7;
      pattern[1][1] = note+4;
      pattern[1][2] = note;
      pattern[1][3] = note+4;

      // Fill pattern ARP B - channel 3 :  A - A+7 - A+4 - A+7   (MIXED 1)
      pattern[2][0] = note;
      pattern[2][1] = note+7;
      pattern[2][2] = note+4;
      pattern[2][3] = note+7;

      // Fill pattern ARP B - channel 4 :  A+4 - A+7 - A - A+7   (MIXED 2)
      pattern[3][0] = note+4;
      pattern[3][1] = note+7;
      pattern[3][2] = note;
      pattern[3][3] = note+7;
      break;
    case 2:
      // Fill pattern ARP C - channel 1 :  A - A+4 - A+7 - A+7 - A+4 - A   (UP AND DOWN)
      pattern[0][0] = note;
      pattern[0][1] = note+4;
      pattern[0][2] = note+7;
      pattern[0][3] = note+7;
      pattern[0][4] = note+4;
      pattern[0][5] = note;

      // Fill pattern ARP C - channel 2 :  A+7 - A+4 - A - A - A+4 - A+7  (DOWN AND UP)
      pattern[1][0] = note+7;
      pattern[1][1] = note+4;
      pattern[1][2] = note;
      pattern[1][3] = note;
      pattern[1][4] = note+4;
      pattern[1][5] = note+7;

      // Fill pattern ARP C - channel 3 :  A - A+7 - A+4 - A+4 - A+7 - A   (MIXED UP AND DOWN)
      pattern[2][0] = note;
      pattern[2][1] = note+7;
      pattern[2][2] = note+4;
      pattern[2][3] = note+4;
      pattern[2][4] = note+7;
      pattern[2][5] = note;

      // Fill pattern ARP C - channel 4 :  A+4 - A+7 - A - A - A+7 - A+4   (MIXED DOWN AND UP)
      pattern[3][0] = note+4;
      pattern[3][1] = note+7;
      pattern[3][2] = note;
      pattern[3][3] = note;
      pattern[3][4] = note+7;
      pattern[3][5] = note+4;
      break;
    
    default:
      break;
  }
}

void patternFactory::generatePatternTwoNotes() {
  int note1 = 0;
  int note2 = 0;
  int counter = 0;
  clearPatterns();
  
  for (int i = 0; i < 4; i++) {
    if (notes[i] != 0) {
      if (counter == 0) {
        note1 = notes[i];
      } else {
        note2 = notes[i];
        break;
      }
      counter++;
    }
  }

switch (patterMode) {
    case 0:
      // Fill pattern ARP A - channel 1 : A - B
      pattern[0][0] = note1;
      pattern[0][1] = note2;

      // Fill pattern ARP A - channel 2 :  A - A - B - B
      pattern[1][0] = note1;
      pattern[1][1] = note1;
      pattern[1][2] = note2;
      pattern[1][3] = note2;

      // Fill pattern ARP A - channel 3 :  A - A+4 - B - B+4
      pattern[2][0] = note1;
      pattern[2][1] = note1+4;
      pattern[2][2] = note2;
      pattern[2][3] = note2+4;

      // Fill pattern ARP A - channel 4 :  B+4 - B - A+4 - A
      pattern[3][0] = note2+4;
      pattern[3][1] = note2;
      pattern[3][2] = note1+4;
      pattern[3][3] = note1;
      break;
    case 1:
      // Fill pattern ARP B - channel 1 :  A - A+4 - B - B+4 - B - A+4   (UP-DOWN 1)
      pattern[0][0] = note1;
      pattern[0][1] = note1+4;
      pattern[0][2] = note2;
      pattern[0][3] = note2+4;
      pattern[0][4] = note2;
      pattern[0][5] = note1+4;

      // Fill pattern ARP B - channel 2 :  B+4 - B - A+4 - A - A+4 - B   (DOWN-UP 1)
      pattern[1][0] = note2+4;
      pattern[1][1] = note2;
      pattern[1][2] = note1+4;
      pattern[1][3] = note1;
      pattern[1][4] = note1+4;
      pattern[1][5] = note2;

      // Fill pattern ARP B - channel 3 :  A - B - A+4 - B+4 - A+4 - B   (UP-DOWN 2)
      pattern[2][0] = note1;
      pattern[2][1] = note2;
      pattern[2][2] = note1+4;
      pattern[2][3] = note2+4;
      pattern[2][4] = note1+4;
      pattern[2][5] = note2;

      // Fill pattern ARP B - channel 4 :  B+4 - A+4 - B - A - B - A+4   (DOWN-UP 2)
      pattern[3][0] = note2+4;
      pattern[3][1] = note1+4;
      pattern[3][2] = note2;
      pattern[3][3] = note1;
      pattern[3][4] = note2;
      pattern[3][5] = note1+4;
      break;
    case 2:
      // Fill pattern ARP C - channel 1 :     (UP AND DOWN)
      pattern[0][0] = note1;
      pattern[0][1] = note1+4;
      pattern[0][2] = note2;
      pattern[0][3] = note2+4;
      pattern[0][4] = note2+4;
      pattern[0][5] = note2;
      pattern[0][6] = note1+4;
      pattern[0][7] = note1;

      // Fill pattern ARP C - channel 2 :    (DOWN AND UP)
      pattern[1][0] = note2+4;
      pattern[1][1] = note2;
      pattern[1][2] = note1+4;
      pattern[1][3] = note1;
      pattern[1][4] = note1;
      pattern[1][5] = note1+4;
      pattern[1][6] = note2;
      pattern[1][7] = note2+4;

      // Fill pattern ARP C - channel 3 :     (MIXED UP AND DOWN)
      pattern[2][0] = note1;
      pattern[2][1] = note2;
      pattern[2][2] = note1+4;
      pattern[2][3] = note2+4;
      pattern[2][4] = note2+4;
      pattern[2][5] = note1+4;
      pattern[2][6] = note2;
      pattern[2][7] = note1;

      // Fill pattern ARP C - channel 4 :     (MIXED DOWN AND UP)
      pattern[3][0] = note2+4;
      pattern[3][1] = note1+4;
      pattern[3][2] = note2;
      pattern[3][3] = note1;
      pattern[3][4] = note1;
      pattern[3][5] = note2;
      pattern[3][6] = note1+4;
      pattern[3][7] = note2+4;
      break;
    
    default:
      break;
  }
}

void patternFactory::generatePatternThreeNotes() {
  int note1 = 0;
  int note2 = 0;
  int note3 = 0;

  int noteSorted1 = 0;
  int noteSorted2 = 0;
  int noteSorted3 = 0;

  int counter = 0;
  clearPatterns();
  
  for (int i = 0; i < 4; i++) {
    if (notes[i] != 0) {
      if (counter == 0) {
        note1 = notes[i];
      } else if (counter == 1) {
        note2 = notes[i];
      } else {
        note3 = notes[i];
        break;
      }
      counter++;
    }
  }

  counter = 0;
  for (int i = 0; i < 4; i++) {
    if (notesASC[i] != 0) {
      if (counter == 0) {
        noteSorted1 = notesASC[i];
      } else if (counter == 1) {
        noteSorted2 = notesASC[i];
      } else {
        noteSorted3 = notesASC[i];
        break;
      }
      counter++;
    }
  }
switch (patterMode) {
    case 0:
      // Fill pattern ARP A - channel 1 : A - B - C  (UP SORTED)
      pattern[0][0] = noteSorted1;
      pattern[0][1] = noteSorted2;
      pattern[0][2] = noteSorted3;

      // Fill pattern ARP A - channel 2 :  C - B - A (DOWN SORTED)
      pattern[1][0] = noteSorted3;
      pattern[1][1] = noteSorted2;
      pattern[1][2] = noteSorted1;

      // Fill pattern ARP A - channel 3 :  A - B - C (UP PLAYED)
      pattern[2][0] = note1;
      pattern[2][1] = note2;
      pattern[2][2] = note3;

      // Fill pattern ARP A - channel 4 :  C - B - A (DOWN PLAYED)
      pattern[3][0] = note3;
      pattern[3][1] = note2;
      pattern[3][2] = note1;
      break;
    case 1:
      // Fill pattern ARP B - channel 1 :  A - B - C - B   (UP-DOWN SORTED)
      pattern[0][0] = noteSorted1;
      pattern[0][1] = noteSorted2;
      pattern[0][2] = noteSorted3;
      pattern[0][3] = noteSorted2;

      // Fill pattern ARP B - channel 2 :  C - B - A - B   (DOWN-UP SORTED)
      pattern[1][0] = noteSorted3;
      pattern[1][1] = noteSorted2;
      pattern[1][2] = noteSorted1;
      pattern[1][3] = noteSorted2;

      // Fill pattern ARP B - channel 3 :  A - B - C - B   (UP-DOWN PLAYED)
      pattern[2][0] = note1;
      pattern[2][1] = note2;
      pattern[2][2] = note3;
      pattern[2][3] = note2;

      // Fill pattern ARP B - channel 4 :   C - B - A - B   (UP-DOWN PLAYED)
      pattern[3][0] = note3;
      pattern[3][1] = note2;
      pattern[3][2] = note1;
      pattern[3][3] = note2;
      break;
    case 2:
      // Fill pattern ARP C - channel 1 :     (UP AND DOWN)
      pattern[0][0] = noteSorted1;
      pattern[0][1] = noteSorted2;
      pattern[0][2] = noteSorted3;
      pattern[0][3] = noteSorted3;
      pattern[0][4] = noteSorted2;
      pattern[0][5] = noteSorted1;

      // Fill pattern ARP C - channel 2 :    (DOWN AND UP)
      pattern[1][0] = noteSorted3;
      pattern[1][1] = noteSorted2;
      pattern[1][2] = noteSorted1;
      pattern[1][3] = noteSorted1;
      pattern[1][4] = noteSorted2;
      pattern[1][5] = noteSorted3;

      // Fill pattern ARP C - channel 3 :     (MIXED UP AND DOWN)
      pattern[2][0] = note1;
      pattern[2][1] = note2;
      pattern[2][2] = note3;
      pattern[2][3] = note3;
      pattern[2][4] = note2;
      pattern[2][5] = note1;

      // Fill pattern ARP C - channel 4 :     (MIXED DOWN AND UP)
      pattern[3][0] = note3;
      pattern[3][1] = note2;
      pattern[3][2] = note1;
      pattern[3][3] = note1;
      pattern[3][4] = note2;
      pattern[3][5] = note3;
      break;
    
    default:
      break;
  }

}

void patternFactory::generatePatternVourNotes() {
  clearPatterns();
  
  switch (patterMode) {
    case 0:
      // Fill pattern ARP A - channel 1 : A - B - C - D (UP SORTED)
      pattern[0][0] = notesASC[0];
      pattern[0][1] = notesASC[1];
      pattern[0][2] = notesASC[2];
      pattern[0][3] = notesASC[3];

      // Fill pattern ARP A - channel 2 :  D - C - B - A (DOWN SORTED)
      pattern[1][0] = notesASC[3];
      pattern[1][1] = notesASC[2];
      pattern[1][2] = notesASC[1];
      pattern[1][3] = notesASC[0];

      // Fill pattern ARP A - channel 3 :  A - B - C - D (UP PLAYED)
      pattern[2][0] = notes[0];
      pattern[2][1] = notes[1];
      pattern[2][2] = notes[2];
      pattern[2][3] = notes[3];

      // Fill pattern ARP A - channel 4 :  D - C - B - A (DOWN PLAYED)
      pattern[3][0] = notes[3];
      pattern[3][1] = notes[2];
      pattern[3][2] = notes[1];
      pattern[3][3] = notes[0];
      break;
    case 1:
      // Fill pattern ARP B - channel 1 :  A - B - C - D - C - B   (UP-DOWN SORTED)
      pattern[0][0] = notesASC[0];
      pattern[0][1] = notesASC[1];
      pattern[0][2] = notesASC[2];
      pattern[0][3] = notesASC[3];
      pattern[0][4] = notesASC[2];
      pattern[0][5] = notesASC[1];

      // Fill pattern ARP B - channel 2 :  D - C - B - A - B - C   (DOWN-UP SORTED)
      pattern[1][0] = notesASC[3];
      pattern[1][1] = notesASC[2];
      pattern[1][2] = notesASC[1];
      pattern[1][3] = notesASC[0];
      pattern[1][4] = notesASC[1];
      pattern[1][5] = notesASC[2];

      // Fill pattern ARP B - channel 3 :  A - B - C - D - C - B   (UP-DOWN PLAYED)
      pattern[2][0] = notes[0];
      pattern[2][1] = notes[1];
      pattern[2][2] = notes[2];
      pattern[2][3] = notes[3];
      pattern[2][4] = notes[2];
      pattern[2][5] = notes[1];

      // Fill pattern ARP B - channel 4 :  D - C - B - A - B - C   (UP-DOWN PLAYED)
      pattern[3][0] = notes[3];
      pattern[3][1] = notes[2];
      pattern[3][2] = notes[1];
      pattern[3][3] = notes[0];
      pattern[3][4] = notes[1];
      pattern[3][5] = notes[2];
      break;
    case 2:
      // Fill pattern ARP C - channel 1 :     (UP AND DOWN)
      pattern[0][0] = notesASC[0];
      pattern[0][1] = notesASC[1];
      pattern[0][2] = notesASC[2];
      pattern[0][3] = notesASC[3];
      pattern[0][4] = notesASC[3];
      pattern[0][5] = notesASC[2];
      pattern[0][6] = notesASC[1];
      pattern[0][7] = notesASC[0];

      // Fill pattern ARP C - channel 2 :    (DOWN AND UP)
      pattern[1][0] = notesASC[3];
      pattern[1][1] = notesASC[2];
      pattern[1][2] = notesASC[1];
      pattern[1][3] = notesASC[0];
      pattern[1][4] = notesASC[0];
      pattern[1][5] = notesASC[1];
      pattern[1][6] = notesASC[2];
      pattern[1][7] = notesASC[3];

      // Fill pattern ARP C - channel 3 :     (MIXED UP AND DOWN)
      pattern[2][0] = notes[0];
      pattern[2][1] = notes[1];
      pattern[2][2] = notes[2];
      pattern[2][3] = notes[3];
      pattern[2][4] = notes[3];
      pattern[2][5] = notes[2];
      pattern[2][6] = notes[1];
      pattern[2][7] = notes[0];

      // Fill pattern ARP C - channel 4 :     (MIXED DOWN AND UP)
      pattern[3][0] = notes[3];
      pattern[3][1] = notes[2];
      pattern[3][2] = notes[1];
      pattern[3][3] = notes[0];
      pattern[3][4] = notes[0];
      pattern[3][5] = notes[1];
      pattern[3][6] = notes[2];
      pattern[3][7] = notes[3];
      break;
    
    default:
      break;
  }
  
}
