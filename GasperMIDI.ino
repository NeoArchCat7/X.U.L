// GasperMIDI, custom MIDI controller by SmartCatLoaf

#include <MIDIUSB.h>
#include <ArduinoJson.h>
#include "usb_rename.h"

USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");

// Configuration constants
#define NUM_FADERS 3
#define FADER_THRESHOLD 5

int res = 0; // 0 - low res; 1 - mid res; 2 - high res
uint8_t FADER_PINS[NUM_FADERS] = {A1, A2, A3};
uint8_t CC_NUMBERS[NUM_FADERS] = {1, 2, 3};
uint16_t lastValues[NUM_FADERS] = {0};

uint8_t mapToMIDI(uint16_t value)
{
    return map(value, 0, 1023, 0, 127);
}

void setup()
{
    Serial.begin(9600); // Uncomment for debugging
}

void loop()
{

    if (Serial.available())
    {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, Serial);

        if (error)
        {
            Serial.println("Failed to parse JSON");
            return;
        }

        int resolution = doc["resolution"];
        int cc1 = doc["ccValues"][0];
        int cc2 = doc["ccValues"][1];
        int cc3 = doc["ccValues"][2];

        res = resolution;
        // iterate through the faders
        for (int i = 0; i < NUM_FADERS; i++)
        {
            CC_NUMBERS[i] = doc["ccValues"][i];
        }
    }

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
#if RESOLUTION
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