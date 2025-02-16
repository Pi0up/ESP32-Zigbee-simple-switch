#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/********************* GPIO Configuration *********************/
#define ZIGBEE_LIGHT_ENDPOINT 10
#define ZIGBEE_SWITCH_ENDPOINT_1 11
#define ZIGBEE_SWITCH_ENDPOINT_2 12

#define PIN_INPUT1 0
#define PIN_INPUT2 1
#define LASTSTATE 0
#define NEWSTATE 1

bool statesArray[2][2];

uint8_t led = RGB_BUILTIN;  // GPIO for the LED
uint8_t button = BOOT_PIN;  // Boot button
bool ledState = false;

/********************* Zigbee Objects *********************/
ZigbeeColorDimmableLight zbColorLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeContactSwitch zbSwitch1(ZIGBEE_SWITCH_ENDPOINT_1);
ZigbeeContactSwitch zbSwitch2(ZIGBEE_SWITCH_ENDPOINT_2);

/********************* LED Functions *********************/
void setLED(bool value) {
    digitalWrite(led, value ? HIGH : LOW);
}

/********************* Setup Function *********************/
void setup() {
    Serial.begin(115200);

    // LED Initialization
    pinMode(led, OUTPUT);
    rgbLedWrite(led, 0, 0, 0);

    // Set Zigbee callbacks
    zbColorLight.onLightChange(setRGBLight);
    zbColorLight.onIdentify(identify);

    // Configure input pins
    pinMode(PIN_INPUT1, INPUT_PULLUP);
    pinMode(PIN_INPUT2, INPUT_PULLUP);
    pinMode(button, INPUT_PULLUP);

    // Set device manufacturer and model
    zbColorLight.setManufacturerAndModel("Pioup", "Interrupteur");
    zbSwitch1.setManufacturerAndModel("Pioup", "zbSwitch");
    zbSwitch2.setManufacturerAndModel("Pioup", "ZBSwitch2");

    // Add endpoints to the Zigbee core
    Serial.println("Adding Zigbee endpoints...");
    Zigbee.addEndpoint(&zbColorLight);
    Zigbee.addEndpoint(&zbSwitch1);
    Zigbee.addEndpoint(&zbSwitch2);

    // Open network for 180 seconds after boot
    Zigbee.setRebootOpenNetwork(180);

    // Start Zigbee
    if (!Zigbee.begin()) {
        Serial.println("Error: Zigbee could not start!");
        Serial.println("Restarting...");
        ESP.restart();
    }

    Serial.println("Connecting to Zigbee network...");
    while (!Zigbee.connected()) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
}

/********************* State Management *********************/
bool changeState(int pin) {
    if (!statesArray[pin][NEWSTATE]) {
        initState(pin);
    }
    statesArray[pin][NEWSTATE] = digitalRead(pin);
    if (statesArray[pin][NEWSTATE] != statesArray[pin][LASTSTATE]) {
        statesArray[pin][LASTSTATE] = statesArray[pin][NEWSTATE];
        return true;
    }
    return false;
}

void initState(int pin) {
    statesArray[pin][LASTSTATE] = false;
    statesArray[pin][NEWSTATE] = false;
}

/********************* RGB LED Functions *********************/
void setRGBLight(bool state, uint8_t red, uint8_t green, uint8_t blue, uint8_t level) {
    if (!state) {
        rgbLedWrite(led, 0, 0, 0);
        return;
    }
    float brightness = (float)level / 255;
    rgbLedWrite(led, red * brightness, green * brightness, blue * brightness);
}

/********************* Identify Function *********************/
void identify(uint16_t time) {
    static uint8_t blink = 1;
    log_d("Identify called for %d seconds", time);
    if (time == 0) {
        zbColorLight.restoreLight();
        return;
    }
    rgbLedWrite(led, 255 * blink, 255 * blink, 255 * blink);
    blink = !blink;
}

/********************* Loop Function *********************/
void loop() {
    static bool contact1 = false;
    static bool contact2 = false;

    // Check contact sensor 1
    if (digitalRead(PIN_INPUT1) == HIGH && !contact1) {
        zbSwitch1.setOpen();
        contact1 = true;
    } else if (digitalRead(PIN_INPUT1) == LOW && contact1) {
        zbSwitch1.setClosed();
        contact1 = false;
    }

    // Check contact sensor 2
    if (digitalRead(PIN_INPUT2) == HIGH && !contact2) {
        zbSwitch2.setOpen();
        contact2 = true;
    } else if (digitalRead(PIN_INPUT2) == LOW && contact2) {
        zbSwitch2.setClosed();
        contact2 = false;
    }

    // Check button for factory reset
    if (digitalRead(button) == LOW) {
        delay(100);
        int startTime = millis();
        while (digitalRead(button) == LOW) {
            delay(50);
            if ((millis() - startTime) > 3000) {
                Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
                delay(1000);
                Zigbee.factoryReset();
            }
        }
        zbColorLight.setLightLevel(zbColorLight.getLightLevel() / 3);
    }
    delay(100);
}
