/* Emulation of Arduino ethernetclient functionality using NB-Iot SIM7020 module

Mimimalistic approach for single TCP connection

2026-01-12 AO

*/

#include <Arduino.h>
#include "sim7020.h"

#define DEBUG 1
#define DEBUG2 0

int16_t streamGetIntBefore(char lastChar) {
  char buf[7];
  size_t bytesRead = simSerial.readBytesUntil(
    lastChar, buf, static_cast<size_t>(7));
  // if we read 7 or more bytes, it's an overflow
  if (bytesRead && bytesRead < 7) {
    buf[bytesRead] = '\0';
    int16_t res = atoi(buf);
    return res;
  }

  return -9999;
}


bool streamSkipUntil(const char c, const uint32_t timeout_ms = 1000L) {
  uint32_t startMillis = millis();
  while (millis() - startMillis < timeout_ms) {
    while (millis() - startMillis < timeout_ms && !simSerial.available()) {
      delay(1);
    }
    if (simSerial.read() == c) { return true; }
  }
  return false;
}

int sim7020Client::connect(const char *host, uint16_t port) {
  if (DEBUG)
    Serial.println("sim7020Client Connect hostname");
  // Close any existing IP connections
  stop();
  int8_t rsp = true;
  int timeout_s = 75;
  String cmd;
  String bufstr;
  uint32_t timeout_ms = ((uint32_t)timeout_s) * 1000;
  /* Select Data Transmitting Mode */
  sim7020.sendAT("+CIPQSEND=1");
  if (sim7020.waitResponse() != 1) {
    return -1;
  }
  /* Set to get data manually */
  sim7020.sendAT("+CIPRXGET=1");
  sim7020.waitResponse();
  /* Start Up TCP or UDP Connection */
  cmd = "+CIPSTART=\"TCP\",\"";
  cmd.concat(host);
  cmd.concat("\",");
  cmd.concat(port);

  sim7020.sendAT(cmd);
  sim7020.waitResponse();
  rsp = sim7020.waitResponse(255, bufstr, "CONNECT OK", "CONNECT FAIL",
                             "ALREADY CONNECT", "ERROR", "CLOSE OK");  // Happens when HTTPS handshake fails
  Serial.print("Waitresp CIPSTART ended: ");
  Serial.println(rsp);
  return (rsp);
}

int sim7020Client::connect(IPAddress ip, uint16_t port) {
  if (DEBUG)
    Serial.println("sim7020Client Connect IP");
  
  // Close any exiting TCP connections
  stop();

  int8_t rsp = true;
  int timeout_s = 75;
  String cmd;
  String bufstr;
  uint32_t timeout_ms = ((uint32_t)timeout_s) * 1000;
  /* Select Data Transmitting Mode */
  sim7020.sendAT("+CIPQSEND=1");
  if (sim7020.waitResponse() != 1) {
    return -1;
  }
  /* Set to get data manually */
  sim7020.sendAT("+CIPRXGET=1");
  sim7020.waitResponse();
  /* Start Up TCP or UDP Connection */
  cmd = "+CIPSTART=\"TCP\",\"";
  cmd.concat(ip.toString());
  cmd.concat("\",");
  cmd.concat(port);

  //="+CIPSTART=\"TCP\",\""+host+"\","+port);
  sim7020.sendAT(cmd);
  sim7020.waitResponse();
  rsp = sim7020.waitResponse(timeout_ms, bufstr, "CONNECT OK", "CONNECT FAIL",
                             "ALREADY CONNECT", "ERROR", "CLOSE OK");  // Happens when HTTPS handshake fails
  // When stopping with CIPSHUT "ALREADY CONNECT" should not occur i guess..
  return (rsp == 1 || rsp == 3);
}

int sim7020Client::availableForWrite(void) {
  if (DEBUG)
    Serial.println("sim7020Client availableForWrite");
  return 1;
}

size_t sim7020Client::write(uint8_t b) {
  if (DEBUG)
    Serial.println("sim7020Client write b");

  return 1;
}

size_t sim7020Client::write(const uint8_t *buf, size_t size) {
  if (DEBUG)
    Serial.println("sim7020Client write buf size");
  String bufstr;
  bufstr = "+CIPSEND=";
  bufstr.concat(size);
  Serial.print("cmd: ");
  Serial.println(bufstr);
  sim7020.sendAT(bufstr);
  if (sim7020.waitResponse(">") != 1) {
    return 0;
  }
  uint8_t length;
  for (length = 0; length < size; length++) {
    if (DEBUG2)
    {
      Serial.println(*buf, HEX);
    }
    simSerial.write(*buf);
    *buf++;
  }
  simSerial.flush();
  if (sim7020.waitResponse("DATA ACCEPT:") != 1) {
    Serial.println("DATA ACCEPT not found");
    return 0;
  }

  uint8_t ret;
  ret = streamGetIntBefore('\n');
  if (DEBUG2) {
    Serial.print("streamGetIntBefore: ");
    Serial.println(ret);
  }
  return ret;
  //return size;
}

size_t modemGetAvailable() {
  sim7020.sendAT("+CIPRXGET=4");
  size_t result = 0;
  if (sim7020.waitResponse("+CIPRXGET:") == 1) {
    if (!streamSkipUntil(',')) {
      Serial.println("streamSkipUntil , failed");
      return -1;
    }
    result = streamGetIntBefore('\n');
    if (DEBUG2) {
      Serial.print("ModemGetAvailable: ");
      Serial.println(result);
    }
    sim7020.waitResponse();
  }
  return result;
}


int sim7020Client::available() {
  size_t result = 0;
  if (DEBUG2)
    Serial.println("sim7020 Client available");
  result = modemGetAvailable();
  if (DEBUG2) {
    Serial.print("Available: ");
    Serial.println(result);

    if (result == 0) {
      delay(1000);
    }
  }
  return result;
}

int sim7020Client::read(uint8_t *buf, size_t size) {
  if (DEBUG2)
    Serial.println("sim7020Client read buf size");
  // return 1;

  String cmd;
  cmd = "+CIPRXGET=2,";
  cmd.concat(size);
  sim7020.sendAT(cmd);
  if (sim7020.waitResponse("+CIPRXGET:") != 1) {
    Serial.println("Read: Waitresponse CIPRXGET: failed");
    delay(1000);
    return 0;
  }
  // #endif
  streamSkipUntil(',');  // Skip Rx mode 2/normal or 3/HEX
  int16_t len_requested = streamGetIntBefore(',');
  //  ^^ Requested number of data bytes (1-1460 bytes)to be read
  int16_t len_confirmed = streamGetIntBefore('\n');
  // ^^ Confirmed number of data bytes to be read, which may be less than
  // requested. 0 indicates that no data can be read.
  // SRGD NOTE:  Contrary to above (which is copied from AT command manual)
  // this is actually be the number of bytes that will be remaining in the
  // buffer after the read.
  for (int i = 0; i < len_requested; i++) {
    uint32_t startMillis = millis();
    startMillis = millis();
    while (!simSerial.available() && (millis() - startMillis < 10000)) {
      delay(1);
    }
    char c = simSerial.read();
    buf[0] = c;
  }
  // DBG("### READ:", len_requested, "from", mux);
  // sockets[mux]->sock_available = modemGetAvailable(mux);
  //  sockets[mux]->sock_available = len_confirmed;
  sim7020.waitResponse();
  return len_requested;
}

int sim7020Client::peek() {
  if (DEBUG)
    Serial.println("sim7020Client peek");
  return 1;
}

int sim7020Client::read() {
  if (DEBUG2)
    Serial.println("sim7020Client read");
  uint8_t c;
  if (read(&c, 1) == 1) {
    return c;
  }
  return -1;
}


void sim7020Client::flush() {
  if (DEBUG)
    Serial.println("sim7020Client flush");
  return;
}

void sim7020Client::stop() {
  String bufstr;
  if (DEBUG)
    Serial.println("sim7020Client stop");
  sim7020.sendAT("+CIPCLOSE=0");
  // Error if closed already
  sim7020.waitResponse(255, bufstr, "CLOSE OK", "ERROR");

  sim7020.sendAT("+CIPSHUT");
  // ERROR if shut already
  sim7020.waitResponse(255, bufstr, "SHUT OK", "ERROR");

  delay(1000);

  return;
}

uint8_t sim7020Client::connected() {
  if (DEBUG2)
    Serial.println("sim7020Client connected");

  sim7020.sendAT("+CIPSTATUS");
  if (!sim7020.waitResponse("STATE: ")) {
    return false;
  }
  if (sim7020.waitResponse("CONNECT OK", "TCP CLOSED", "TCP CONNECTING", "IP INITIAL") != 1) {
    return false;
  } else {
    return true;
  }
}



uint8_t sim7020Client::status() {
  if (DEBUG)
    Serial.println("sim7020Client status");
  return 1;
}
