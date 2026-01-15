

#include <Arduino.h>
#include "Client.h"

class sim7020Client;

class sim7020Class {
public:
	// Initialise the Ethernet shield to use the provided MAC address and
	// gain the rest of the configuration through DHCP.
	// Returns 0 if the DHCP configuration failed, and 1 if it succeeded
	static int begin(uint8_t *mac, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);

	// Manual configuration
	static void begin(uint8_t *mac, IPAddress ip);
	static void begin(uint8_t *mac, IPAddress ip, IPAddress dns);
	static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway);
	static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
  static bool init(String pin);
  static bool nbiotConnect(String apn, uint8_t band);
  static void sendAT(String command);
  static int8_t waitResponse(uint8_t timeout, String &data, const char* r1, const char* r2, const char* r3, const char* r4, const char* r5);
  static int8_t waitResponse(uint8_t timeout, String &data, const char* r1, const char* r2, const char* r3, const char* r4);
  static int8_t waitResponse(uint8_t timeout, String &data, const char* r1, const char* r2, const char* r3);
  static int8_t waitResponse(uint8_t timeout, String &data, const char* r1, const char* r2);
  static int8_t waitResponse(uint8_t timeout, String &data, const char* r1);
  static int8_t waitResponse(uint8_t timeout, String &data);
  static int8_t waitResponse(const char* r1, const char* r2, const char* r3, const char* , const char* r5);
  static int8_t waitResponse(const char* r1, const char* r2, const char* r3, const char* r4);
  static int8_t waitResponse(const char* r1, const char* r2, const char* r3);
  static int8_t waitResponse(const char* r1, const char* r2);
  static int8_t waitResponse(const char* r1);
  static int8_t waitResponse();
  static int8_t waitResponse(uint8_t timeout, const char* r1);
  static bool testAT(uint32_t timeout_ms);
  static int8_t getRegistrationStatus();
 };

extern sim7020Class sim7020;

extern HardwareSerial simSerial;

class sim7020Client : public Client {
public:
/*	sim7020Client() : _sockindex(MAX_SOCK_NUM), _timeout(1000) { }
	sim7020Client(uint8_t s) : _sockindex(s), _timeout(1000) { }
*/
	virtual ~sim7020Client() {};

	uint8_t status();
	virtual int connect(IPAddress ip, uint16_t port);
	virtual int connect(const char *host, uint16_t port);
	virtual int availableForWrite(void);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buf, size_t size);
	virtual int available();
	virtual int read();
	virtual int read(uint8_t *buf, size_t size);
	virtual int peek();
	virtual void flush();
	virtual void stop();
	virtual uint8_t connected();
  virtual operator bool() { return 0; }
//  virtual operator bool() { return _sockindex < MAX_SOCK_NUM; }
/*	virtual bool operator==(const bool value) { return bool() == value; }
	virtual bool operator!=(const bool value) { return bool() != value; }
	virtual bool operator==(const sim7020Client&);
	virtual bool operator!=(const sim7020Client& rhs) { return !this->operator==(rhs); }
	uint8_t getSocketNumber() const { return _sockindex; }
	virtual uint16_t localPort();
	virtual IPAddress remoteIP();
	virtual uint16_t remotePort(); */
	virtual void setConnectionTimeout(uint16_t timeout) { _timeout = timeout; }

	using Print::write;

private:
//	uint8_t _sockindex; // MAX_SOCK_NUM means client not in use
	uint16_t _timeout;
};
