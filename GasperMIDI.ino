// GasperMIDI, custom MIDI controller by SmartCatLoaf

#include <MIDIUSB.h>
#include "usb_rename.h"

USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");

// Configuration constants
#define HIGH_RES 1        // 0 for 7-bit MIDI (0-127), 1 for 14-bit MIDI (0-16383)
#define NUM_FADERS 3      // Number of faders connected to the controller
#define FADER_THRESHOLD 5 // Sensitivity threshold for detecting changes in fader position

// Pin assignments for the faders
const uint8_t FADER_PINS[NUM_FADERS] = {A1, A2, A3};
// Corresponding Control Change (CC) numbers for each fader
const uint8_t CC_NUMBERS[NUM_FADERS] = {0, 1, 2};
uint16_t lastValues[NUM_FADERS] = {0};

uint8_t mapToMIDI(uint16_t value)
{
    return map(value, 0, 1023, 0, 127);
}

void setup()
{
    // Serial.begin(9600); // Uncomment for debugging
}

void loop()
{
    static uint8_t i;
    static uint16_t value;
    static uint8_t lsb, msb;

    // Iterate through each fader
    for (i = 0; i < NUM_FADERS; i++)
    {
        // Read the current value of the fader
        value = analogRead(FADER_PINS[i]);

        // Check if the change in fader value exceeds the threshold
        if (abs(value - lastValues[i]) >= FADER_THRESHOLD)
        {
#if HIGH_RES
            // If high resolution mode is enabled (14-bit MIDI)

            lsb = value & 0x7F;
            msb = (value >> 7) & 0x7F;
            MidiUSB.sendMIDI({0x0B, 0xB0, CC_NUMBERS[i], lsb});
            MidiUSB.sendMIDI({0x0B, 0xB0, static_cast<uint8_t>(CC_NUMBERS[i] + 32), msb});
#else
            // If high resolution mode is disabled (7-bit MIDI)

            // Serial.println(mapToMIDI(value)); // Uncomment for debugging
            MidiUSB.sendMIDI({0x0B, 0xB0, CC_NUMBERS[i], mapToMIDI(value)});
#endif

            MidiUSB.flush();
            lastValues[i] = value;
        }
    }
}