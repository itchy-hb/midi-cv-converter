# midi-cv-converter
Midi to CV converter 4 Voices

Midi to CV:
    Midi Modes:
        Mode 0:
            Each output has his own midi channel (Default channels:out1->midi ch1, out2->midi ch2, out3->midi ch3, out4->midi ch4)
                - 4 device outputs at the same time
                - output monophon 
                - only one note from device
        Mode 1:
            Input from only one midi channel (Default: midi ch1  -  takes midi channel from out1)
                - 1 device output
                - output polyphon 
                - 4 notes at the same time possible 
                - output on all 4 outputs


Arpeggiator:

    - 1 Device Input
    - Outputs monophonic

    Arp Modes:
        Mode 0:
            - Arpeggiator Off - see "Midi to CV" chapter
            - clear pattern
        Mode 1:
            - Arpeggiator On
                - Input from device 1
            - clear pattern

    Midi Modes in Arp Mode 1:
        Mode 0:
            - push all notes hold at the same time to arpeggiator 
                - in "push order" and "sorted by notes"
                - new pattern when push hold notes again
        Mode 1:
            - push note by note to arpeggiator
                - in "push order" and "sorted by notes"
                - clear pattern by switch arp off and on

    PRG Modes in Arp Mode 1:
        - switch arpeggiator patterns
        - 3 Modes, each with 4 different Patterns on the 4 outputs at the same time
        PrgMode 0:
            Pattern - see pattern sheet ARP A
        PrgMode 1:
            Pattern - see pattern sheet ARP B
        PrgMode 2:
            Pattern - see pattern sheet ARP C


Set Midi Channels:
(Not Available in Arpeggiator Mode - Arp Mode 1)

    - Switch Arp Off - Arp Mode 0
    - Select MidiMode 0 to set midi channel of the 4 outputs
    - or Select MidiMode 1 to just set out1 midi channel

    MidiMode 0:
        - press prg button
        - select output 
            - led shows the output selected
        - press prg button
        - select midi channel
            - leds show the selected number as binary number
        - press prg button to leave midi setup

    MidiMode 1:
        - press prg button
        - select midi channel
            - leds show the selected number as binary number
        - press prg button to leave midi setup
