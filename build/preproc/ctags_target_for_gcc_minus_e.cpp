# 1 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino"
# 2 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 3 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2
# 4 "C:\\Users\\Gregor\\Desktop\\Nace\\Programiranje\\VS Code - git\\GasperMIDI\\GasperMIDI.ino" 2

USBRename dummy = USBRename("GasperMIDI", "SmartCatLoaf", "0001");

// Configuration constants




int res = 0; // 0 - low res (7-bit); 1 - mid res (10-bit); 2 - high res (14-bit)
uint8_t FADER_PINS[3] = {A1, A2, A3};
uint8_t CC_NUMBERS[3] = {1, 2, 3};
uint16_t lastValues[3] = {0};

// Function prototypes
void sendMIDI7bit(uint8_t channel, uint8_t control, uint8_t value);
void sendMIDI10bit(uint8_t channel, uint8_t control, uint16_t value);
void sendMIDI14bit(uint8_t channel, uint8_t control, uint16_t value);
uint16_t applyDeadZone(uint16_t value, uint16_t maxValue);
void debugPrint(const char *message);
void debugPrintln(const char *message);

void setup()
{
    if (false /* Set to false to disable debug prints*/)
    {
        Serial.begin(9600);
        while (!Serial)
        {
            ; // Wait for serial port to connect. Needed for native USB
        }
        debugPrintln("GasperMIDI initialized");
    }
}

void loop()
{
    if (Serial.available())
    {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, Serial);

        if (error)
        {
            debugPrint("JSON parsing failed: ");
            debugPrintln(error.c_str());
            return;
        }

        if (doc.containsKey("resolution") && doc.containsKey("ccValues"))
        {
            res = doc["resolution"];
            if (res < 0 || res > 2)
            {
                debugPrintln("Invalid resolution value");
                return;
            }
            for (int i = 0; i < 3; i++)
            {
                if (doc["ccValues"][i].is<int>())
                {
                    CC_NUMBERS[i] = doc["ccValues"][i];
                }
                else
                {
                    debugPrintln("Invalid CC number");
                    return;
                }
            }
            debugPrintln("Configuration updated");
        }
        else
        {
            debugPrintln("Invalid JSON structure");
        }
    }

    for (int i = 0; i < 3; i++)
    {
        uint16_t value = analogRead(FADER_PINS[i]);

        if ((((int)value - (int)lastValues[i])>0?((int)value - (int)lastValues[i]):-((int)value - (int)lastValues[i])) >= 5)
        {
            value = applyDeadZone(value, 1023);

            if (false /* Set to false to disable debug prints*/)
            {
                Serial.print("Fader ");
                Serial.print(i + 1);
                Serial.print(" | Resolution: ");
                Serial.print(res);
                Serial.print(" | Raw value: ");
                Serial.print(value);
                Serial.print(" | ");
            }

            switch (res)
            {
            case 0:
            { // 7-bit resolution
                uint8_t mappedValue = map(value, 0, 1023, 0, 127);
                sendMIDI7bit(0, CC_NUMBERS[i], mappedValue);
                if (false /* Set to false to disable debug prints*/)
                {
                    Serial.print("CC: ");
                    Serial.print(CC_NUMBERS[i]);
                    Serial.print(", Value: ");
                    Serial.println(mappedValue);
                }
                break;
            }
            case 1:
            { // 10-bit resolution
                sendMIDI10bit(0, CC_NUMBERS[i], value);
                if (false /* Set to false to disable debug prints*/)
                {
                    Serial.print("CC1: ");
                    Serial.print(CC_NUMBERS[i]);
                    Serial.print(", CC2: ");
                    Serial.print(CC_NUMBERS[i] + 32);
                    Serial.print(", Value: ");
                    Serial.println(value);
                }
                break;
            }
            case 2:
            { // 14-bit resolution
                uint16_t scaledValue = map(value, 0, 1023, 0, 16368);
                sendMIDI14bit(0, CC_NUMBERS[i], value);
                if (false /* Set to false to disable debug prints*/)
                {
                    Serial.print("CC1: ");
                    Serial.print(CC_NUMBERS[i]);
                    Serial.print(", CC2: ");
                    Serial.print(CC_NUMBERS[i] + 32);
                    Serial.print(", Value: ");
                    Serial.println(scaledValue);
                }
                break;
            }
            }
            lastValues[i] = value;
        }
    }
}

void sendMIDI7bit(uint8_t channel, uint8_t control, uint8_t value)
{
    midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
    MidiUSB.sendMIDI(event);
    MidiUSB.flush();
}

void sendMIDI10bit(uint8_t channel, uint8_t control, uint16_t value)
{
    uint8_t lsb = value & 0x7F;
    uint8_t msb = (value >> 7) & 0x07;
    sendMIDI7bit(channel, control, lsb);
    sendMIDI7bit(channel, control + 32, msb);
}

void sendMIDI14bit(uint8_t channel, uint8_t control, uint16_t value)
{
    uint16_t scaledValue = map(value, 0, 1023, 0, 16368);
    uint8_t lsb = scaledValue & 0x7F;
    uint8_t msb = (scaledValue >> 7) & 0x7F;
    sendMIDI7bit(channel, control, lsb);
    sendMIDI7bit(channel, control + 32, msb);
}

uint16_t applyDeadZone(uint16_t value, uint16_t maxValue)
{
    const uint16_t deadZone = maxValue / 100; // 1% dead zone at each end
    if (value < deadZone)
        return 0;
    if (value > maxValue - deadZone)
        return maxValue;
    return map(value, deadZone, maxValue - deadZone, 0, maxValue);
}

void debugPrint(const char *message)
{
    if (false /* Set to false to disable debug prints*/)
    {
        Serial.print(message);
    }
}

void debugPrintln(const char *message)
{
    if (false /* Set to false to disable debug prints*/)
    {
        Serial.println(message);
    }
}
