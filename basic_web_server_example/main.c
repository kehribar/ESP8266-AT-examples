/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "main.h"

uint8_t tcpBuffer[512];

int main()
{  
  uint16_t msgId;
  uint16_t rv;
  uint16_t tcpLen;

  init_hardware();
  esp8266_init();
    
  xfprintf(dbg,"Hello World!\r\n");
  
  esp8266_createWifiNetwork("wifi_ap","123456789");

  esp8266_createTCPSocket(80);

  while(1)
  {
    /* Check any message ... */
    if(esp8266_getTCPData(ESP8266_10SecTimeout,tcpBuffer,sizeof(tcpBuffer),&rv,&msgId) == ESP8266_OK)
    {      
      /*-----------------------------------------------------------------
      / Respond to HTTP GET messages - Basic web server
      /------------------------------------------------------------------
      / Example of a starting part of an HTTP GET message is: 
      /   GET /index.html HTTP/1.1
      /----------------------------------------------------------------*/
      if(strncmp("GET /",tcpBuffer,5) == 0)
      {        
        /* This is the root directory */
        if(strncmp("GET / ",tcpBuffer,6) == 0)
        {         
          _delay_ms(250);
         
          tcpLen = esp8266_fill_tcp_data(tcpBuffer,0,HTTP_RESPONSE_HEADER);   
          tcpLen = esp8266_fill_tcp_data(tcpBuffer,tcpLen,"This is a web page at the root directory!");          
        }
        else
        {          
          _delay_ms(250);

          tcpLen = esp8266_fill_tcp_data(tcpBuffer,0,HTTP_RESPONSE_HEADER);   
          tcpLen = esp8266_fill_tcp_data(tcpBuffer,tcpLen,"Unknown web page");          
        }

        /* Send response */
        esp8266_sendTCPData(ESP8266_1SecTimeout,msgId,tcpBuffer,tcpLen);                         
      }

      /* Close the connection */
      esp8266_closeTCPLink(msgId);
    }
  }
  
  return 0;
}

