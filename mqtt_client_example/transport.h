/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "../common/esp8266.h"

RingBuffer_t tcpBuffer;
uint8_t commonBuffer[128];
volatile uint8_t tcpBufferData[128];

int transport_init();

int transport_close(int sock);

int transport_open(char* host, int port);

int transport_getdata(uint8_t* buf, int count);

int transport_sendPacketBuffer(int sock, uint8_t* buf, int buflen);