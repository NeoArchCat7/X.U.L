#include <Arduino.h>
#line 1 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
#include <MIDIUSB.h>
#include <ResponsiveAnalogRead.h>
#include "usb_rename.h"

// Rename USB device
USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");

#define NUM_FADERS 3
#define FADER_THRESHOLD 2

const uint8_t FADER_PINS[NUM_FADERS] = {A1, A2, A3};
const uint8_t CC_NUMBERS[NUM_FADERS] = {20, 21, 22};
uint8_t lastValues[NUM_FADERS] = {0};

#line 15 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
void setup();
#line 17 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
void loop();
#line 15 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
void setup() {}

void loop()
{
    for (uint8_t i = 0; i < NUM_FADERS; i++)
    {
        uint8_t value = map(analogRead(FADER_PINS[i]), 0, 1023, 0, 127);

        if (abs(value - lastValues[i]) >= FADER_THRESHOLD)
        {
            midiEventPacket_t event = {0x0B, 0xB0, CC_NUMBERS[i], value};
            MidiUSB.sendMIDI(event);
            MidiUSB.flush();

            lastValues[i] = value;
        }
    }
}
