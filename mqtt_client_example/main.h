/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include <avr/io.h>	
#include <string.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "transport.h"
#include <avr/interrupt.h>
#include "xmega_digital.h"
#include "hardwareLayer.h"
#include "../common/esp8266.h"
#include "../common/xprintf.h"
#include "../common/ringBuffer.h"
#include "./MQTTPacket/MQTTPacket.h"
/*---------------------------------------------------------------------------*/
uint8_t SSID_name[32];
uint8_t SSID_pass[32];
/*---------------------------------------------------------------------------*/
uint8_t* SSID_name_addr = (uint8_t*)32; /* EEPROM base address */
uint8_t* SSID_pass_addr = (uint8_t*)64; /* EEPROM base address */
/*---------------------------------------------------------------------------*/
