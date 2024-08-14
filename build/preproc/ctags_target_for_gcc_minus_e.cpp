# 1 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
// GasperMIDI, custom MIDI controller by SmartCatLoaf

# 4 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 5 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 6 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2

USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");

// Configuration constants



int res = 0;
uint8_t FADER_PINS[3] = {A1, A2, A3};
uint8_t CC_NUMBERS[3] = {1, 2, 3};
uint16_t lastValues[3] = {0};

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
        for (int i = 0; i < 3; i++)
        {
            CC_NUMBERS[i] = doc["ccValues"][i];
        }
    }

    Serial.print("Configuration (res-cc1-cc2-cc3) -->   ");
    Serial.print(res);
    Serial.print(" : ");
    Serial.print(CC_NUMBERS[0]);
    Serial.print(" : ");
    Serial.print(CC_NUMBERS[1]);
    Serial.print(" : ");
    Serial.println(CC_NUMBERS[2]);

    delay(3000);

    static uint8_t i;
    static uint16_t value;
    static uint8_t lsb, msb;

    // Iterate through each fader
    for (i = 0; i < 3; i++)
    {
        // Read the current value of the fader
        value = analogRead(FADER_PINS[i]);

        // Check if the change in fader value exceeds the threshold
        if (((value - lastValues[i])>0?(value - lastValues[i]):-(value - lastValues[i])) >= 5)
        {
# 87 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
            // If high resolution mode is disabled (7-bit MIDI)

            // Serial.println(mapToMIDI(value)); // Uncomment for debugging
            MidiUSB.sendMIDI({0x0B, 0xB0, CC_NUMBERS[i], mapToMIDI(value)});


            MidiUSB.flush();
            lastValues[i] = value;
        }
    }
}
