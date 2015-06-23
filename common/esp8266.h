/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include <stdint.h>
#include "xprintf.h"
#include "ringBuffer.h"

/*---------------------------------------------------------------------------*/

#define ESP8266_UNKNOWN -2
#define ESP8266_TIMEOUT -1
#define ESP8266_OK 0
#define ESP8266_ERROR 1
#define ESP8266_BUSY 2

/*---------------------------------------------------------------------------*/

#define ESP8266_1SecTimeout 10000
#define ESP8266_10SecTimeout 100000

/*---------------------------------------------------------------------------*/

int8_t esp8266_init();

int8_t esp8266_waitForChar(uint32_t timeoutLimit);

int8_t esp8266_checkOkError(uint32_t timeoutLimit);

int8_t esp8266_openTCPSocket(char* address, uint16_t port);

int8_t esp8266_connectWifiNetwork(char* ssidName, char* password);

int8_t esp8266_waitForMessage(const char* checkmsg, uint32_t timeoutLimit);

int8_t esp8266_sendTCPData(uint32_t timeoutLimit, uint8_t* buf, uint16_t len);

int8_t esp8266_getTCPData(uint32_t timeoutLimit, uint8_t* buf, uint16_t maxSize, uint16_t* actualDataLen);

uint16_t StrTo16Uint(char* str);

uint16_t esp8266_getMessageLength();

/*---------------------------------------------------------------------------*/

extern RingBuffer_t esp8266_ringBuf;
extern volatile uint8_t esp8266_ringBufData[1024];

extern void esp8266_hal_init();

extern void esp8266_hal_rebootSystem();

extern void esp8266_hal_sendChar(uint8_t ch);

extern void esp8266_hal_delayMiliseconds(uint16_t delay);

extern void esp8266_hal_delayMicroseconds(uint16_t delay);

/*---------------------------------------------------------------------------*/
