# 1 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
// GasperMIDI, custom MIDI controller by SmartCatLoaf

# 4 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 5 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2

USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");

// Configuration constants




// Pin assignments for the faders
const uint8_t FADER_PINS[3 /* Number of faders connected to the controller*/] = {A1, A2, A3};
// Corresponding Control Change (CC) numbers for each fader
const uint8_t CC_NUMBERS[3 /* Number of faders connected to the controller*/] = {0, 1, 2};
uint16_t lastValues[3 /* Number of faders connected to the controller*/] = {0};

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
    for (i = 0; i < 3 /* Number of faders connected to the controller*/; i++)
    {
        // Read the current value of the fader
        value = analogRead(FADER_PINS[i]);

        // Check if the change in fader value exceeds the threshold
        if (((value - lastValues[i])>0?(value - lastValues[i]):-(value - lastValues[i])) >= 5 /* Sensitivity threshold for detecting changes in fader position*/)
        {

            // If high resolution mode is enabled (14-bit MIDI)

            lsb = value & 0x7F;
            msb = (value >> 7) & 0x7F;
            MidiUSB.sendMIDI({0x0B, 0xB0, CC_NUMBERS[i], lsb});
            MidiUSB.sendMIDI({0x0B, 0xB0, static_cast<uint8_t>(CC_NUMBERS[i] + 32), msb});







            MidiUSB.flush();
            lastValues[i] = value;
        }
    }
}
