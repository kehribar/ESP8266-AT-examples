/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "transport.h"
/*---------------------------------------------------------------------------*/
static uint16_t rv;
/*---------------------------------------------------------------------------*/
int transport_init()
{
  RingBuffer_InitBuffer(&tcpBuffer, tcpBufferData, sizeof(tcpBufferData));
}
/*---------------------------------------------------------------------------*/
int transport_sendPacketBuffer(int sock, uint8_t* buf, int buflen)
{
  int8_t res;
  res = esp8266_sendTCPData(ESP8266_10SecTimeout,buf,buflen);  

  if(res != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
    return 0;
  }
  
  return 0;
}
/*---------------------------------------------------------------------------*/
int transport_getdata(uint8_t* buf, int count)
{
  int i;
  int t16;
  int8_t res;

  while(count > RingBuffer_GetCount(&tcpBuffer))
  {
    res = esp8266_getTCPData(ESP8266_1SecTimeout,commonBuffer,sizeof(commonBuffer),&rv);       

    if(res == ESP8266_TIMEOUT)
    {
      return 0;
    }
    
    for(i=0;i<rv;i++)
    {
      if(!RingBuffer_IsFull(&tcpBuffer))
      {
        RingBuffer_Insert(&tcpBuffer,commonBuffer[i]);        
      }
    }

  }

  for(i=0;i<count;i++)
  {
    buf[i] = RingBuffer_Remove(&tcpBuffer);
  }  

  return count;
}
/*---------------------------------------------------------------------------*/
int transport_open(char* addr, int port)
{  
  esp8266_openTCPSocket(addr,port);
  
  return 0;
}
/*---------------------------------------------------------------------------*/
int transport_close(int sock)
{
  /* ... */

  return 0;
}
/*---------------------------------------------------------------------------*/