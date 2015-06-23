/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include <stdint.h>
#include <util/delay.h>
#include "xmega_digital.h"
#include <avr/interrupt.h>
#include "../common/ringBuffer.h"

RingBuffer_t esp8266_ringBuf;
uint8_t esp8266_ringBufData[1024];

void esp8266_hal_init();

void esp8266_hal_rebootSystem();

void esp8266_hal_sendChar(uint8_t ch);

void esp8266_hal_delayMiliseconds(uint16_t delay);

void esp8266_hal_delayMicroseconds(uint16_t delay);