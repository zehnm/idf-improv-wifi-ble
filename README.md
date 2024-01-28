
# ESP32 Improv WiFi BLE

This project demonstrates the use of [Improv WiFi](https://www.improv-wifi.com/) via Bluetooth Low Energy communication on an ESP32 board.

## Motivation

The main motivation for this project was to learn BLE communication and to evaluate [Improv WiFi BLE](https://www.improv-wifi.com/ble/) for a project I'm working on. After using PlatformIO with the Arduino framework in the past for private and commercial projects, it was also time to finally embrace the IDF SDK.

Since I couldn't find any IDF implementations, I started this project and hope it will be useful to others. Please note, this is not an IDF component you can simply include in a project to get improv-wifi BLE support! It's mainly a playground to explore NimBLE and other features of the IDF SDK.

## Status

Work in progress :-)

- [x] BLE advertisement works and looks identical to the [ESP Web Tools](https://esphome.github.io/esp-web-tools/) firmware.
- [x] Connection to the device with https://www.improv-wifi.com/ works:
  - [x] Authorization request (push the button) is shown if authorization is enabled.
  - [x] WiFi provisioning dialog is shown if authorization is disabled.
- [x] Capablilites, current state, and error state can be read.
- [x] Identify command works (except with Chrome on Mac, see below).
- [x] WiFi credentials are received (except with Chrome on Mac, see below).
- [x] WiFi connection with received credentials.
- [x] Redirect link is sent back after successful connection
  - [ ] However, the _Next_ button is not shown with https://www.improv-wifi.com/. But this also happens with the [ESP Web Tools](https://esphome.github.io/esp-web-tools/) firmware...

Open issues and missing features:
- WiFi credentials are not persisted. They are only held in RAM and are not written to NVS.
- Not all LED drivers are implemented yet for the identification feature:
  - [x] GPIO LED
  - [ ] PWN LED
  - [ ] PWN RGB LED
  - [x] WS2812 LED strip (no colors yet, just white blinking patterns for now)

## Development

Developed with Espressif IDF 5 and NimBLE.

Tested with:
- ESP32 boards: C6
- Development environment:
  - Espressif IDF 5.1.2
  - Visual Studio Code with ESP-IDF extension
- Browser based WiFi provisioning with https://www.improv-wifi.com/
  - ✅ Chrome 121 on an Android 9 tablet
  - ✅ Chrome 121 on Linux
  - ⛔️ Chrome 121 on macOS 13.6 M1: write call never reaches the peripheral: [improv-wifi/sdk-ble-js#213](https://github.com/improv-wifi/sdk-ble-js/issues/213)

### Configuration

Use the ESP-IDF SDK configuration editor (menuconfig) to configure features and options of improv-wifi:

- Optional authorization through a GPIO push button.
- Optional LED identification feature. Uses IDF [LED Indicator](https://docs.espressif.com/projects/esp-iot-solution/en/latest/display/led_indicator.html) component to support various LED drivers.
- BLE device information advertisement data
