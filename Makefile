CMD=arduino-cli
BOARD=esp8266:esp8266:nodemcuv2
PORT=/dev/ttyUSB0

compile: 
	$(CMD) compile -b $(BOARD)

upload: compile code_upload

code_upload: 
	$(CMD) upload -b $(BOARD) -p $(PORT)

setup:
	$(CMD) config init
	$(CMD) config set board_manager.additional_urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
	$(CMD) core update-index
	$(CMD) core install esp8266:esp8266
	$(CMD) lib install PubSubClient
	$(CMD) lib install "Adafruit BME280 Library"
	$(CMD) lib install "Adafruit Unified Sensor"

.PHONY: clean

clean:
	rm -rf ./build

