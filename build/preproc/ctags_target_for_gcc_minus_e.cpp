# 1 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
# 2 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 3 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 4 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2

// Rename USB device
USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");




const uint8_t FADER_PINS[3] = {A1, A2, A3};
const uint8_t CC_NUMBERS[3] = {20, 21, 22};
uint8_t lastValues[3] = {0};

void setup() {}

void loop()
{
    for (uint8_t i = 0; i < 3; i++)
    {
        uint8_t value = map(analogRead(FADER_PINS[i]), 0, 1023, 0, 127);

        if (((value - lastValues[i])>0?(value - lastValues[i]):-(value - lastValues[i])) >= 2)
        {
            midiEventPacket_t event = {0x0B, 0xB0, CC_NUMBERS[i], value};
            MidiUSB.sendMIDI(event);
            MidiUSB.flush();

            lastValues[i] = value;
        }
    }
}
