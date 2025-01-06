# Emergency Stop Button with HomeKit Integration

## Overview

This project uses an **ESP32 microcontroller** to create an emergency stop button that can trigger **HomeKit scenes through MQTT**.
\
The ESP32 connects to an MQTT broker and publishes the state of the emergency stop button (pressed or released). This state is then processed by Homebridge using the Mqttthing plugin, which converts the MQTT message into a virtual switch in HomeKit. This allows you to use the emergency stop button to control HomeKit scenes or other automations.

The repository contains the code for the ESP32 microcontroller, which reads the state of the emergency stop button and sends MQTT messages to the broker. It also includes instructions on setting up Homebridge with the **Mqttthing** plugin to handle the MQTT messages and create a virtual switch in HomeKit.

<img src="/assets/components.jpg" width="60%">

## But Why?

A few years ago I randomly bought a emergency stop button from a local store. I was thinking about how to use it in a smart home environment, since it was just sitting in a drawer. I came up with the idea of using it as a panic button to trigger a scene in HomeKit. This project is the result of that idea. Of course, it might have been easier to just buy a smart button or use ESPHome and Home Assistant, but where's the fun in that?

## Components Used

- **ESP32-C3 DevKitC V2** (e.g., an ESP32 microcontroller)
- **Emergency Stop Button**
- **Wi-Fi Network** (for the ESP32 to connect to your MQTT broker)
- **MQTT Broker** (e.g., Mosquitto or any other MQTT service)
- **Homebridge** with **Mqttthing** plugin installed
- **HomeKit App** (for defining scenes or automations)

## Wiring

For the wiring setup, follow these steps:

1. **Connect the Button to the ESP32**:
   - **Pin 0 (GPIO 0)**: Connect this pin to one terminal of the emergency stop button.
   - **GND**: Connect the other terminal of the emergency stop button to the GND pin of the ESP32.
2. **Button Type**:

   - Use a **Normally Closed (NC)** push button. When the button is pressed, it will break the circuit, signaling a "pressed" state to the ESP32. When the button is released, the circuit is completed, signaling a "released" state.

3. **Wiring Summary**:
   - **Pin 0 (GPIO 0)** → One terminal of the push button
   - **GND** → Other terminal of the push button

The circuit should be simple, with the button closing or opening the circuit between **Pin 0** and **GND**.

<img src="/assets/wiring.jpg" width="60%">

## How It Works

1. **ESP32 Setup**: The ESP32 is programmed to monitor the state of a physical button. When the button is pressed or released, the ESP32 sends the corresponding state (`1` for pressed, `L` for released) to an MQTT broker.
2. **MQTT Communication**: The ESP32 connects to the MQTT broker using provided credentials (SSID, password, and MQTT broker details).
3. **Homebridge Mqttthing**: The Homebridge instance uses the [Mqttthing](https://github.com/arachnetech/homebridge-mqttthing#readme) plugin to subscribe to the MQTT topic (`home/emergency_button/state`). When a state change is received, it triggers the corresponding action in HomeKit.
4. **Virtual Switch in HomeKit**: The MQTT message is converted into a virtual switch in HomeKit, which can then be used to trigger scenes or automations.

The following sequence diagram illustrates the flow of data between the components:
![Sequence Diagram showing the flow of data between the components](/assets/sequence_diagram.png)

## Project Setup

### 1. PlatformIO Setup

This project is built using PlatformIO. Follow these steps to set up the project:

- Clone or download the repository to your local machine.
- Open the project folder in VS Code with PlatformIO installed.
- Adjust the `platformio.ini` file to match your ESP32 model and settings.

### 2. Create `secrets.h`

In order to provide your Wi-Fi and MQTT credentials to the ESP32, you need to create a `secrets.h` file. The repository includes a `secrets.h.example` file, which serves as a template.

1. **Copy `secrets.h.example` to `secrets.h`**:

   - Rename the `secrets.h.example` file to `secrets.h` in the `src` directory.

2. **Edit the `secrets.h` file**:

   - Open the `secrets.h` file and fill in your Wi-Fi network credentials and MQTT broker information:

     ```cpp
     // secrets.h
     #define WIFI_SSID "your_wifi_ssid"          // Your Wi-Fi network name
     #define WIFI_PASSWORD "your_wifi_password"  // Your Wi-Fi network password

     #define MQTT_SERVER "your_mqtt_broker_ip"   // IP address or hostname of your MQTT broker
     #define MQTT_PORT 1883                       // MQTT broker port (default is 1883)
     #define MQTT_USER "your_mqtt_username"       // MQTT username (if required)
     #define MQTT_PASSWORD "your_mqtt_password"   // MQTT password (if required)
     ```

3. **Important**: Do not commit the actual `secrets.h` file to the repository, as it contains sensitive information. Only the `secrets.h.example` file should be included in the repository.

### 3. Upload to ESP32

- Build and upload the code to your ESP32 using PlatformIO.
- Ensure the ESP32 is connected to your network and can communicate with the MQTT broker.

> [!TIP]
> The ESP32 code logs the connection status and button state to the serial monitor. Use the serial monitor to debug any connection issues or button state changes.

### 4. Homebridge Setup

- Install Homebridge on a Raspberry Pi or another compatible device.
- Install the **Mqttthing** plugin in Homebridge.
- Create a virtual `StatelessProgrammableSwitch` accessory in Homebridge using the plugin. This kind of switch is read-only and can be used to trigger multiple scenes or automations.
- Configure the plugin with the MQTT connection details and credentials.
- Set the `Get Switch` event to the MQTT topic (`home/emergency_button/state`) and `Get Online` event to the MQTT topic (`home/emergency_button/status`).

### 5. Using the Emergency Stop Button

Once everything is set up:

- Run the function test to check if the button is working correctly. Press and release the button until the orange LED turns off.
- Pressing the button will send an MQTT message to the broker.
- Homebridge will convert this message into a virtual switch in HomeKit.
- You can link the virtual switch to HomeKit scenes or automations. The created `StatelessProgrammableSwitch` has three read-only states: `Single Press`, `Double Press`, and `Long Press`.
  The emergency stop button will trigger the `Single Press` state when pressed and the `Long Press` state when released. `Double Press`cannot be triggered by the button.

## LED Indicator

The onboard LED of the ESP32 provides visual feedback for the connection status and button state:

- **Off**: The device is operating normally, assuming it is powered on and the small red power LED is illuminated.
- **🔴 Red**: The button has been pressed, and the message was successfully sent to the MQTT broker. The LED turns off after one minute.
- **🟢 Green**: The button has been released, and the message was successfully sent to the MQTT broker. The LED turns off after five seconds.
- **🔵 Blue**: The device is in test mode. Press and release the button to complete the test. Upon pressing, the LED will turn light blue. Once the test is successful, the blue light will turn off, and the device will return to normal operation.
- **🟡 Yellow**: The device is unable to establish a Wi-Fi connection.
- **🟣 Purple**: The device is connected to the Wi-Fi network but cannot connect to the MQTT broker.


If you are using a different ESP32 model, you may need to modify the pin number assigned to the onboard LED.
## Example MQTT Message

- **Button Pressed**: Sends `1` to the MQTT topic `home/emergency_button/state`.
- **Button Released**: Sends `L` to the MQTT topic `home/emergency_button/state`.
- **Controller gets online**: Sends `true` to the MQTT topic `home/emergency_button/status`.

## Notes

- Make sure your MQTT broker is running and accessible to both the ESP32 and Homebridge.
- You can modify the script to adjust debounce times, button behavior, and MQTT topics as needed.
On other ESP32 models, you may need to adjust the pin number for the onboard LED.
- The ESP32 does not send the initial state of the button when it connects to the MQTT broker to avoid triggering the `released` state on startup.
- Using `StatelessProgrammableSwitch` in favor of a regular switch allows for more flexibility in HomeKit scenes and automations, like turning off the triggered scene after a given time.

## Conclusion

This project provides an easy way to trigger HomeKit scenes or automations with a physical emergency stop button. The combination of an ESP32, MQTT, and Homebridge makes it simple to integrate the button into your Homebridge ecosystem. But most importantly, it's a fun way to repurpose a simple device into a useful tool for your home and learn about different technologies in the process.
