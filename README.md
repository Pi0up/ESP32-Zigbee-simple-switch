# ESP32-Zigbee-Simple-Switch

## Overview
This project provides a simple Zigbee-enabled switch implementation for ESP32 devices. It utilizes the ESP32 Espressif 3.1.2 library to support contact switches.

## Features
- Zigbee Color Dimmable Light control
- Two Contact Switch endpoints
- Factory reset via button press
- RGB LED state indication
- Tested on ESP32-C6 Mini

## Requirements
- ESP32 board (tested on ESP32-C6 Mini)
- Zigbee module
- ESP32 Espressif 3.1.2 library
- Arduino IDE or PlatformIO

## Setup
1. Install the ESP32 Espressif 3.1.2 library.
2. Connect the GPIO pins as specified in the code.
3. Compile and upload the firmware to your ESP32 device.
4. Open the serial monitor at 115200 baud rate to debug the connection.

## Testing Matrix
| Device          | Status |
|----------------|--------|
| ESP32-C6 Mini | ✅ OK  |

## Usage
- The device will attempt to connect to a Zigbee network on startup.
- Contact switches update their states based on GPIO inputs.
- The factory reset button restores Zigbee settings after a long press.

## License
This project is open-source and available under the MIT License.
