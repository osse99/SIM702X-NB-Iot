# SIM702X-NB-Iot

###
- Send information from BME280 sensor and battery voltage (using AD) to a MQTT broker using [ESP32-C3](https://www.espboards.dev/esp32/esp32-c3-super-mini/) and [SIM7020g](https://www.alibaba.com/product-detail/Wgzx-Simcom-7020g-Lpwa-Nb-Iot_1601629328588.html)
- Do deep sleep of ESP32-C3 and CPOWD of SIM7020g to enable battery operation
-----------------
###
- Aligns with Arduino architecture emulating ethernetClient so "any" client should work, MQTT, COAP, HTTP,...
- Using SSL library MQTTS and HTTPS should work to
-----------------
## Hardware

###
- ESP32-C3 or ESP32-S3 mini (no external, power hungry, USB to serial circuit)
- SIM7020g breakout board
- 39 Kohm resistor to pull PWRPIN high and keep SIM7020 in POWD state
- External 3.3V LDO Voltage regulator XR6206, 3 in parallell to handle up to 600 mA
-- (ESP32-C3 powered from XR6206 through 3.3V pin)
- Solar cell 6V
- CN3791 MPPT charger
- 18650 Liion cell
- Voltage divider 2*27Kohm resistors connected battery -> 27Kohm -> ESP32 AD pin 0 -> 27Kohm -> GND
- ESP32-C3 pin assignments in config.h
-----------------
## Software

###
- Set local NB-Iot and MQTT parameters in secrets.h
- Set number of seconds between connection attempts in config.h
-----------------

Fragments of the code is similar to/copied from TinyGSM

# This is work in progress
