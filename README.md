# SIM702X-NB-Iot

Send information to a MQTT broker using ESP32 and SIM7020g

Do deep sleep of ESP32 and CPOWD of SIM7020g to save battery power

Aligns with Arduino architecture emulating ethernetClient so "any" client should work, MQTT, COAP, HTTP,...
Using SSL library MQTTS and HTTPS should work to

Hardware

ESP32-C3 or ESP32-S3 mini (no external, power hungry, USB to serial circuit)
SIM7020g breakout board
39 Kohm resistor to pull PWRPIN high and keep SIM7020 in POWD state

