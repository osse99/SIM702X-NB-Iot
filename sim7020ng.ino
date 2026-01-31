/*
 MQTT client over nbiot using SIM7020g modem

Connections
ESP32           Breakout board
Serial2 TX      RX
Serial2 RX      TX
GPIOx           PWRPIN

*/

#include "Config.h"
#include "secrets.h"
#include "sim7020.h"
#include <PubSubClient.h>

#define DEBUG 1

// Timer sleep stuff
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */



// Initiate ESP32 second UART
HardwareSerial simSerial(SIMSERPORT);

// Initiate Arduino socket client API for SIM7020g TCPIP
sim7020Client ethClient;
// Initiate MQTT client
PubSubClient client(MQTT_BROKER, MQTT_PORT, ethClient);

RTC_NOINIT_ATTR int bootCount;

long now;

void restart_ESP()
{
    /*
    Serial.println("Switching off modem");
    digitalWrite(PWRPIN, 0);
    delay(800);
    digitalWrite(PWRPIN, 1);
    */
  esp_sleep_enable_timer_wakeup(RESTART_DELAY * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to restart in " + String(RESTART_DELAY) + " Seconds");
  esp_deep_sleep_start();
}

// Called when MQTT messages are received
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// MQTT reconnect
void reconnect() {
  int retry=0;

  if (sim7020.getRegistrationStatus() == 5) {
    // Loop until we're reconnected
    while (!client.connected() && retry < RECONNECT_RETRIES) {
      // sim7020.sendAT("+CIPSTATUS");
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(MQTT_CLIENT)) {
        Serial.println("connected");
        // Once connected, publish the sensor data
        // client.publish(MQTT_TOPIC, "hello world");
        // ... and resubscribe
        client.subscribe(MQTT_CALLBACK);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
        // Don't not retry "forever"
        retry++;
      }
    }
    if (retry > RECONNECT_RETRIES)
    {
      restart_ESP();
    }
  } else {
    Serial.println("Reconnect failed");
    restart_ESP();
  }
}

// Sleep stuff
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void connect_nbiot()
{
  int regstatus;

  // Wake up modem
  delay(1000);
  digitalWrite(PWRPIN,0);
  delay(800);
  digitalWrite(PWRPIN,1);
  delay(2500);
  
  // Send AT command to initiate modem autobaud
  Serial.println("sendAT");
  sim7020.sendAT("");
  delay(2000);
  Serial.println(sim7020.waitResponse());

  // Switch of echo
  sim7020.sendAT("E0");
  if (sim7020.waitResponse("E0") != 1) {
    Serial.println("Warning: No response when sending ATE0");
  };

  // Is the modem avake and responding
  if (sim7020.testAT(10000)) {
    Serial.println("testAT true");
  } else {
    Serial.println("testAT false");
    restart_ESP();
  }

  // Is the SIM card responding and unlocked, add PIN code if needed
  Serial.print("Init: ");
  if ((regstatus = sim7020.init("")) != 1) {
    Serial.print("SIM status: ");
    Serial.println(regstatus);
    restart_ESP();
  }

  // Connect to nbiot network
  sim7020.nbiotConnect(APN, BAND);
  while ((regstatus = sim7020.getRegistrationStatus()) != 5) {
    Serial.print("regstatus: ");
    Serial.println(regstatus);
    delay(1000);
    if (regstatus == -1) {
      restart_ESP();
    }
  }
  Serial.println("Success, associated with NB-Iot network");
  delay(2000);

}

void setup() {
  String bufstr;

  char msg[75];

  // Initiate debug serial port
  Serial.begin(115200);
  // Initiate modem serial port
  simSerial.begin(S2_BAUD, SERIAL_8N1, RXD2, TXD2);

  // Set up PWRKEY pin
  pinMode(PWRPIN, OUTPUT);
  digitalWrite(PWRPIN,1);
  
  // Set up MQTT client params
  client.setCallback(callback);

  //Increment boot number and prin
  if (bootCount < 1)
  {
    bootCount=0;
  }
  bootCount++;
  
  Serial.println("Boot number: " + String(bootCount));
  
  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  
  // Wake up modem and connect to network
  // Function reboots ESP32 if failed connection
  connect_nbiot();

  // Connect MQTT client
  int retcode = client.connect(MQTT_CLIENT);
  Serial.print("Connect ret: ");
  Serial.println(retcode);
  if (retcode > 0) {
    Serial.println("connected");
    // Once connected, publish the sensor data
    snprintf(msg, sizeof(msg), "NB-Iot: %d", bootCount);
    if( DEBUG)
    { 
      Serial.print("Sending message: ");
      Serial.println(msg);
    }
    client.publish(MQTT_TOPIC, msg);
    // client.publish(MQTT_TOPIC, "Just a message");
    // ... and resubscribe
    client.subscribe(MQTT_CALLBACK);
  } else {
    Serial.println("Not able to connect");
    restart_ESP();
  }
  // Get a timestamp
  now = millis();
}

void loop() {
  if (!client.connected()) {
    if (DEBUG) {
      Serial.println("MQTT reconnecting");
    }
    reconnect();
  }
  // Do MQTT tasks
  client.loop();

  // Wait for incoming messages before going to sleep
  if (millis() > now + AWAKETIME*1000) {
    Serial.println("Going to sleep now");
    Serial.flush();

    // Shut down all services
    client.disconnect();
    ethClient.stop();

    sim7020.sendAT("+CPOWD=1");
    Serial.println(sim7020.waitResponse("NORMAL POWER DOWN"));

    Serial.println("CPOWD sent");
    Serial.flush();
    delay(2000);

    esp_deep_sleep_start();
    Serial.println("This will never be printed");
  }
}
