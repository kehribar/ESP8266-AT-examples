/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
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

#define HTTP_RESPONSE_HEADER "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"

/*---------------------------------------------------------------------------*/

int8_t esp8266_init();

int8_t esp8266_createTCPSocket(uint16_t port);

int8_t esp8266_closeTCPLink(uint8_t sockId);

int8_t esp8266_createWifiNetwork(char* ssidName, char* password);

int8_t esp8266_connectWifiNetwork(char* ssidName, char* password);

uint16_t esp8266_fill_tcp_data(uint8_t *buf,uint16_t pos, const char *s);

int8_t esp8266_openTCPSocket(uint8_t sockId,char* address, uint16_t port);

uint16_t esp8266_fill_tcp_data_len(uint8_t *buf,uint16_t pos, const uint8_t *s, uint8_t len);

int8_t esp8266_sendTCPData(uint32_t timeoutLimit, uint8_t sockId, uint8_t* buf, uint16_t len);

int8_t esp8266_getTCPData(uint32_t timeoutLimit, uint8_t* buf, uint16_t maxSize, uint16_t* actualDataLen, uint8_t* sockId);

/*---------------------------------------------------------------------------*/

extern RingBuffer_t esp8266_ringBuf;
extern volatile uint8_t esp8266_ringBufData[1024];

extern void esp8266_hal_init();

extern void esp8266_hal_rebootSystem();

extern void esp8266_hal_sendChar(uint8_t ch);

extern void esp8266_hal_delayMiliseconds(uint16_t delay);

extern void esp8266_hal_delayMicroseconds(uint16_t delay);

/*---------------------------------------------------------------------------*/
