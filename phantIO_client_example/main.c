/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "main.h"

int main()
{  
  int integer;
  int decimal;
  uint16_t rv;
  uint16_t tcpLen;
  uint8_t sockId = 0;  
  int32_t tempReading;
  uint8_t tcpBuffer[512];
  uint8_t urlBuffer[128];
  
  /* Init the system ... */
  init_hardware();
  esp8266_init();    
  xfprintf(dbg,"Hello World!\r\n");
  
  /* Read the settings */
  eeprom_read_block(SSID_name,SSID_name_addr,32);
  eeprom_read_block(SSID_pass,SSID_pass_addr,32);
  eeprom_read_block(PublicKey,PublicKey_addr,32);
  eeprom_read_block(PrivateKey,PrivateKey_addr,32);

  /* Try to connect to network ... */
  esp8266_connectWifiNetwork(SSID_name,SSID_pass);     

  while(1)
  {
    /*-------------------------------------------------------------------------
    / ADC VRef = 2048 mV
    / Effective resolution is 11 bits => 2048 step
    / Each ADC step corresponds to 1mV
    / MCP9700 has 500 mV 0 °C offset and output is 10.0 mV/°C 
    /------------------------------------------------------------------------*/

    /* Get the ADC readings  */
    tempReading  = read_adc(1);
    tempReading += read_adc(1);
    tempReading += read_adc(1);
    tempReading += read_adc(1);
    tempReading  = tempReading >> 2;
    
    /* sensor offset for 0 celcius */
    tempReading -= 500;

    /* Crude way to convert the reading into 0.1 resolution float number */
    integer = tempReading / 10;
    decimal = tempReading - (10 * integer);  

    /*-----------------------------------------------------------------------*/
    /* Open a socket to "data.sparkfun.com" address at port 80 */
    esp8266_openTCPSocket(sockId,"data.sparkfun.com",80);    

    /* Prepare the request */
    xsprintf(urlBuffer,"/input/%s?private_key=%s&temp=%d.%d",PublicKey,PrivateKey,integer,decimal);
    tcpLen = esp8266_create_GetRequest("data.sparkfun.com", urlBuffer, tcpBuffer, 0);

    /* Send the request */
    if(esp8266_sendTCPData(ESP8266_10SecTimeout,sockId,tcpBuffer,tcpLen) != ESP8266_OK)
    {
      esp8266_hal_rebootSystem(); 
    }

    /* Wait for the response ... */
    if(esp8266_getTCPData(ESP8266_10SecTimeout,tcpBuffer,sizeof(tcpBuffer),&rv,&sockId) != ESP8266_OK)
    {
      esp8266_hal_rebootSystem();
    }

    /* Close the socket. */
    esp8266_closeTCPLink(sockId);

    /*-----------------------------------------------------------------------*/
    /* Wait some time ... */
    _delay_ms(10000);    

  }
  
  return 0;
}

