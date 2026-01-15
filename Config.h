
#define UPLOAD_INTERVAL         60000

#define SerialMon               Serial
#define MONITOR_BAUDRATE        115200

// Connected to module PWRPIN
#define PWRPIN 4

// Define the RX and TX pins for Serial 2
#define RXD2 16
#define TXD2 17
#define S2_BAUD 115200

/* set GSM PIN */
#define GSM_PIN             ""

// For Telia nb-iot
#define APN                 "xxxx.xxxx.xx"
#define BAND                3

// MQTT Setting
#define MQTT_BROKER         "xxxx.xxxx.xx"
#define MQTT_PORT           8883
#define MQTT_USERNAME       ""
#define MQTT_PASSWORD       ""
#define MQTT_TOPIC          "nbiotout"
#define MQTT_CALLBACK       "nbiotin"
#define MQTT_CLIENT         "nbiotclient1"
// ToDo: MQTT_CLIENT should/could be module serial number

#define AWAKETIME           5
#define TIME_TO_SLEEP       600          /* Time ESP32 will go to sleep (in seconds) */
