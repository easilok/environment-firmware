# Environment Firmware

Code compatible with the [environment board](https://github.com/easilok/environment_board), using an
[nodemcu](https://www.nodemcu.com/index_en.html) compatible board and a
[BME/BMP280](https://esphome.io/components/sensor/bme280.html) environment sensor.

## Prepare the build

In order for this firmware to work, connection details must be provided for:

- WiFi to connect;
- MQTT server to connect;
- MQTT topics to write the readings and sensor information.

An `ConnectionDetails.sample.h` file is provided that must be copied as `ConnectionDetails.h` and the information filled
accordingly.

It is also required to install the [arduino-cli](https://arduino.github.io/arduino-cli/1.1/installation/) tool which is
used to compile the code and upload it to the target.


## Install library dependencies

This firmware has some third party library dependencies in order to perform some interactions, like with the environment
sensor or with the MQTT server. To install the required libraries simply run:

```bash
make setup
```

This will use the `arduino-cli` tool to install all required dependencies.

> [!WARNING]
> Be aware that if any of the libraries is already installed, the setup will fail. Manual edition of the Makefile will
> be required to install the missing ones.

## Build

After the preparation checking if the code is able to be compiled can be accomplished simply by running:

```bash
make compile
```

This will try to compile the firmware with the `arduino-cli` tool. 

## Upload firmware to target

In order to program the `nodemcu` board with the firmware, connect the board to the computer with a proper USB cable and
run the following command:

```bash
make upload
```

This will try to compile the firmware and try to program the board with the `arduino-cli` tool. 

> [!NOTE]
> The provided makefile assumes that the board connection is identified as `/dev/ttyUSB0`. If this is not the case, the
> upload will fail.

The board will flash its internal LED when trying to authenticate with the WiFi connection and the MQTT server. Whenever
both connections are established, the internal LED will be turned off.

> [!TIP]
> For better debugging install a tool that is capable of reading the serial port `/dev/ttyUSB0`. The board communicates
> status messages with the baudrate of 115200. An example of a software capable of doing such monitor is the official
> [Arduino IDE](https://www.arduino.cc/en/software).
