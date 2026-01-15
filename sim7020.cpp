/* 

Handle SIM7020 modem

2026-01-12 AO

*/

#include "sim7020.h"

#define DEBUG 1
#define DEBUG2 0

enum SimStatus {
  SIM_ERROR = 0,
  SIM_READY = 1,
  SIM_LOCKED = 2,
  SIM_ANTITHEFT_LOCKED = 3,
};

int sim7020Class::begin(uint8_t* mac, unsigned long timeout, unsigned long responseTimeout) {
  if (DEBUG) {
    Serial.println("sim7020 begin mac timeout responseTimeout");
  }
  return 0;
}

void sim7020Class::begin(uint8_t* mac, IPAddress ip) {
  if (DEBUG) {
    Serial.println("sim7020 begin mac IP");
  }
  return;
}

void sim7020Class::begin(uint8_t* mac, IPAddress ip, IPAddress dns) {
  if (DEBUG) {
    Serial.println("sim7020 begin mac IP IP-dns");
  }
  return;
}

void sim7020Class::begin(uint8_t* mac, IPAddress ip, IPAddress dns, IPAddress gateway) {
  if (DEBUG) {
    Serial.println("sim7020 begin mac IP IP-dns IP-gw");
  }
  return;
}

void sim7020Class::begin(uint8_t* mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet) {
  if (DEBUG) {
    Serial.println("sim7020 begin mac IP IP-dns IP-gw IP-subnet");
  }
  return;
}

bool sim7020Class::init(String pin) {
  uint32_t timeout_ms = 10000;
  if (DEBUG) {
    Serial.print("sim7020 init ");
    Serial.print("AT");
    Serial.println(pin);
  }
  if (!testAT) {
    return false;
  }

  if (pin.length() > 0) {
    Serial.println("+CPIN=\"" + pin + "\"");
    sendAT("+CPIN=\"" + pin + "\"");
    int8_t status = waitResponse("OK", "ERROR");
    Serial.print("Status: ");
    Serial.println(status);
  }
  delay(500);

  for (uint32_t start = millis(); millis() - start < timeout_ms;) {
    sendAT("+CPIN?");
    if (waitResponse("+CPIN:") != 1) {
      delay(1000);
      continue;
    }

    int8_t status = waitResponse("READY", "SIM PIN", "SIM PUK", "NOT INSERTED", "NOT READY");
    waitResponse();
    switch (status) {
      case 2:
      case 3:
        return SIM_LOCKED;
      case 1:
        return SIM_READY;
      default:
        return SIM_ERROR;
    }
  }
  return SIM_ERROR;
}

bool sim7020Class::nbiotConnect(String apn, uint8_t band = 0) {
  // Set APN
  sendAT("*MCGDEFCONT=\"IP\",\"" + apn + "\"");
  if (waitResponse() != 1) {
    return false;
  }
  // Set Band
  /*sendAT("+CBAND=?");
  if (waitResponse() != 1) {
    return false;
  } */
  return true;
}

void sim7020Class::sendAT(String command) {
  /* if (DEBUG2) {
    Serial.print("sim7020 sendAT ");
    Serial.print("AT");
    Serial.println(command);
    Serial.print("Command length: ");
    Serial.println(command.length());
  } */
  simSerial.print("AT");
  if (command.length() > 0)
    simSerial.print(command);
  simSerial.write('\r');
  return;
}


int8_t sim7020Class::getRegistrationStatus() {
  String data, bufstr;
  sendAT("+CEREG?");
  if (waitResponse("+CEREG:") != 1) {
    return -1;
  }
  waitResponse(255, data);
  Serial.print("CEREG: ");
  Serial.println(data.substring(data.indexOf(',') + 1, data.indexOf(',') + 2));
  bufstr = data.substring(data.indexOf(',') + 1, data.indexOf(',') + 2);
  return bufstr.toInt();
}


int8_t sim7020Class::waitResponse(uint8_t timeout, String& data, const char* r1 = "OK", const char* r2 = "ERROR", const char* r3 = NULL, const char* r4 = NULL, const char* r5 = NULL) {
  uint8_t index = 0;
  uint32_t startMillis = millis();
  data.reserve(64);

  if (DEBUG2) {
    Serial.print("sim7020 waitResponse ");
    Serial.println(data);
  }
  // uint8_t timeout_ms = timeout;
  uint32_t timeout_ms = 7500;

  do {
    while (simSerial.available() > 0) {
      // Serial.println("Available");
      int8_t a = simSerial.read();
      if (a <= 0)
        continue;
      data += static_cast<char>(a);
      //      if( data.endsWith("OK"))
      if (r1 && data.endsWith(r1)) {
        index = 1;
        if (DEBUG2) {
          Serial.print("Endswith r1: ");
          Serial.println(data);
        }
        goto finish;
      } else if (r2 && data.endsWith(r2)) {
        index = 2;
        goto finish;
      } else if (r3 && data.endsWith(r3)) {
        index = 3;
        goto finish;
      } else if (r4 && data.endsWith(r4)) {
        index = 4;
        goto finish;
      } else if (r5 && data.endsWith(r5)) {
        index = 5;
        goto finish;
      } else if (data.endsWith("+CIPRXGET:")) {
        // int8_t mode = streamGetIntBefore('\n');
        /* if (mode == 1) {
              int8_t mux = 0;
              if (mux >= 0 && mux < TINY_GSM_MUX_COUNT && sockets[mux]) {
                 sockets[mux]->got_data = true;
              }
              data = "";
              // DBG("### Got Data:", mux);
           } else {
              data += mode;
           } */
      } else if (data.endsWith("CLOSED")) {
        data = "";
        // DBG("### Closed: ", mux);
      }
      // if (DEBUG2)
      //  Serial.println(data);
    }
    // Serial.println("Not avai");
  } while (millis() - startMillis < timeout_ms);
  if (DEBUG2) {
    Serial.print("Millis - StartMillis and timeout");
    Serial.println(millis() - startMillis); 
    Serial.println(timeout_ms);
  }
finish:
  if (!index) {
    data.trim();
    if (data.length()) {
      Serial.print("#### Unhandled response: ");
      Serial.println(data);
    }
    data = "";
  }
  if (DEBUG2) {
    Serial.print("Data: ");
    Serial.println(data);
  }
  return index;
}

int8_t sim7020Class::waitResponse(uint8_t timeout, String& data, const char* r1 = "OK", const char* r2 = "ERROR", const char* r3 = NULL, const char* r4 = NULL) {
  return waitResponse(timeout, data, r1, r2, r3, r4, NULL);
}

int8_t sim7020Class::waitResponse(uint8_t timeout, String& data, const char* r1 = "OK", const char* r2 = "ERROR", const char* r3 = NULL) {
  return waitResponse(timeout, data, r1, r2, r3, NULL, NULL);
}

int8_t sim7020Class::waitResponse(uint8_t timeout, String& data, const char* r1 = "OK", const char* r2 = "ERROR") {
  return waitResponse(timeout, data, r1, r2, NULL, NULL, NULL);
}

int8_t sim7020Class::waitResponse(uint8_t timeout, String& data, const char* r1 = "OK") {
  return waitResponse(timeout, data, r1, NULL, NULL, NULL, NULL);
}

int8_t sim7020Class::waitResponse(uint8_t timeout, String& data) {
  return waitResponse(timeout, data, "OK", NULL, NULL, NULL, NULL);
}

int8_t sim7020Class::waitResponse(const char* r1 = "OK", const char* r2 = "ERROR", const char* r3 = NULL, const char* r4 = NULL, const char* r5 = NULL) {
  String data;
  return waitResponse(255, data, r1, r2, r3, r4, r5);
}

int8_t sim7020Class::waitResponse(const char* r1 = "OK", const char* r2 = "ERROR", const char* r3 = NULL, const char* r4 = NULL) {
  String data;
  return waitResponse(255, data, r1, r2, r3, r4, NULL);
}


int8_t sim7020Class::waitResponse(const char* r1 = "OK", const char* r2 = "ERROR", const char* r3 = NULL) {
  String data;
  return waitResponse(255, data, r1, r2, r3, NULL, NULL);
}

int8_t sim7020Class::waitResponse(const char* r1 = "OK", const char* r2 = "ERROR") {
  String data;
  return waitResponse(255, data, r1, r2, NULL, NULL, NULL);
}

int8_t sim7020Class::waitResponse(const char* r1 = "OK") {
  String data;
  return waitResponse(255, data, r1, NULL, NULL, NULL, NULL);
}

int8_t sim7020Class::waitResponse() {
  String data;
  return waitResponse(255, data, "OK", NULL, NULL, NULL, NULL);
}

bool sim7020Class::testAT(uint32_t timeout_ms = 10000L) {
  String bufstr;
  for (uint32_t start = millis(); millis() - start < timeout_ms;) {
    sendAT("");
    if (waitResponse(255, bufstr, "OK", NULL, NULL, NULL, NULL) == 1) {
      Serial.print("testAT true: ");
      Serial.println(bufstr);
      return true;
    }
    delay(100);
  }
  return false;
}