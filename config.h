
#define SerialMon               Serial
#define MONITOR_BAUDRATE        115200

// Connected to module PWRPIN
#define PWRPIN 10

// ESP32-C3 Define the RX and TX pins for Serial 1
#define SIMSERPORT 1
// #define RXD2 6
// #define TXD2 7
#define RXD2 20
#define TXD2 21

// ESP32-dev module Define the RX and TX pins for Serial 2
// #define SIMSERPORT 2
// #define RXD2 16
// #define TXD2 17

// BME280
//#define I2C_SDA 8
//#define I2C_SCL 9
#define I2C_SDA 6
#define I2C_SCL 7

#define S2_BAUD 115200

#define BATTPIN 0
#define MAX_AD 4096
// Voltage divider ration, 2 results in MAX_AD = 4095 for 6.6V
#define ANALOG_RATIO 2

// If error
#define RESTART_DELAY 60
#define RECONNECT_RETRIES 5

/* set GSM PIN */
#define GSM_PIN             ""

// Moved to secrets.h
// For nb-iot
//#define APN                 "xxxx.xxxx.xxxx"
//#define BAND                3

// MQTT Setting
//#define MQTT_BROKER         "mqtt.brooker"
//#define MQTT_PORT           YYYY
//#define MQTT_USERNAME       ""
//#define MQTT_PASSWORD       ""

#define MAX_MESSAGE_LENGTH 256
#define MQTT_TOPIC          "nbiotout"
#define MQTT_CALLBACK       "nbiotin"
#define MQTT_CLIENT         "nbiotclient1"
// ToDo: MQTT_CLIENT should/could be module serial number

// Wait this long for incoming MQTT messages
#define AWAKETIME           5
#define TIME_TO_SLEEP       1800          /* Time ESP32 will go to sleep (in seconds) */