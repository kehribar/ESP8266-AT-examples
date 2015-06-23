/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "esp8266.h"
/*---------------------------------------------------------------------------*/
static uint32_t m_timeoutCounter; 
/*---------------------------------------------------------------------------*/
int8_t esp8266_init()
{
  esp8266_hal_init();

  /* Wait for the powerup */
  esp8266_waitForMessage("]\r\n",ESP8266_10SecTimeout);  

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_waitForChar(uint32_t timeoutLimit)
{
  while(RingBuffer_GetCount(&esp8266_ringBuf) == 0)
  {    
    esp8266_hal_delayMicroseconds(100);

    if(m_timeoutCounter++ > timeoutLimit)
    {      
      return ESP8266_TIMEOUT; 
    }
  } 

  return ESP8266_OK;   
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_waitForMessage(const char* checkmsg, uint32_t timeoutLimit)
{
  uint8_t ch;
  uint8_t run = 1;
  uint16_t in = 0;  
  uint32_t timeout = 0;

  m_timeoutCounter = 0;
  
  while(run == 1)
  {    
    if(esp8266_waitForChar(timeoutLimit) == ESP8266_TIMEOUT)
    {      
      return ESP8266_TIMEOUT;
    }

    ch = RingBuffer_Remove(&esp8266_ringBuf);

    if(checkmsg[in] == ch)
    {
      in++;
    }
    else
    {
      if(ch == checkmsg[0])
      {
        in = 1;
      }
      else
      {
        in = 0;
      }
    }

    if(checkmsg[in] == '\0')
    {
      run = 0;
    }

  }

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_checkOkError(uint32_t timeoutLimit)
{
  uint8_t ch;
  uint8_t ok_in = 0;
  uint8_t err_in = 0;
  
  uint8_t ok_buf[4] = {'O','K','\r','\n'};
  uint8_t err_buf[7] = {'E','R','R','O','R','\r','\n'};
  
  m_timeoutCounter = 0;

  while(1)
  { 
    if(esp8266_waitForChar(timeoutLimit) == ESP8266_TIMEOUT)
    {      
      return ESP8266_TIMEOUT;
    }

    ch = RingBuffer_Remove(&esp8266_ringBuf);

    if(ok_buf[ok_in] == ch)
    {
      ok_in++;
    }
    else
    {
      if(ch == ok_buf[0])
      {
        ok_in = 1;
      }
      else
      {
        ok_in = 0;
      }
    }

    if(err_buf[err_in] == ch)
    {
      err_in++;
    }
    else
    {
      if(ch == err_buf[0])
      {
        err_in = 1;
      }
      else
      {
        err_in = 0;
      }
    }

    if(ok_in == sizeof(ok_buf))
    {
      return ESP8266_OK;
    }
    else if(err_in == sizeof(err_buf))
    {
      return ESP8266_ERROR;
    }

  }

  return ESP8266_UNKNOWN;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_connectWifiNetwork(char* ssidName, char* password)
{
  uint8_t ipTrial = 0;

  /* SSID details */
  xfprintf(esp8266_hal_sendChar,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssidName,password);

  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }  

  /* Wait until system gets IP */  
  do
  {
    ipTrial++;
    esp8266_hal_delayMiliseconds(10);     
    xfprintf(esp8266_hal_sendChar,"AT+CIFSR\r\n"); 
  }
  while((esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK) && (ipTrial < 50));

  /* Reboot if neccessary */
  if(ipTrial == 50)
  {
    esp8266_hal_rebootSystem();
  }

  xfprintf(esp8266_hal_sendChar,"AT+CIPMUX=1\r\n");
  
  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }  

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_openTCPSocket(char* address, uint16_t port)
{
  /* TCP socket details */
  xfprintf(esp8266_hal_sendChar,"AT+CIPSTART=0,\"TCP\",\"%s\",%d\r\n",address,port);
  
  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }  
  
  esp8266_waitForMessage("Linked\r\n",ESP8266_10SecTimeout);

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_sendTCPData(uint32_t timeoutLimit, uint8_t* buf, uint16_t len)
{
  uint16_t t16;

  /* Prepare to send data ... */
  xfprintf(esp8266_hal_sendChar,"AT+CIPSEND=0,%d\r\n",len);

  if(esp8266_waitForMessage("> ",timeoutLimit) != ESP8266_OK)
  {
    return ESP8266_ERROR;
  }

  /* Send the data ... */
  for(t16=0;t16<len;t16++)
  {
    esp8266_hal_sendChar(buf[t16]);
  }

  /* Reboot if neccessary */
  if(esp8266_checkOkError(timeoutLimit) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }  

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
uint16_t esp8266_getMessageLength()
{
  uint8_t run;
  uint8_t in = 0;  
  uint8_t lenbuf[8];  

  /* Skip the initial comma for 'id' */
  run = 1;  
  while(run)
  {
    m_timeoutCounter = 0;
    if(esp8266_waitForChar(ESP8266_1SecTimeout) == ESP8266_TIMEOUT)
    {      
      esp8266_hal_rebootSystem();
    }

    lenbuf[0] = RingBuffer_Remove(&esp8266_ringBuf);

    if(lenbuf[0] == ',')
    {
      run = 0;
    }
  }
  
  /* Get the data length */
  run = 1;
  while(run)
  {    
    m_timeoutCounter = 0;
    if(esp8266_waitForChar(ESP8266_1SecTimeout) == ESP8266_TIMEOUT)
    {      
      esp8266_hal_rebootSystem();
    }

    lenbuf[in] = RingBuffer_Remove(&esp8266_ringBuf);

    if(lenbuf[in] == ':')
    {
      run = 0;
      lenbuf[in] = '\0';
    }
    else
    {
      in++;
    }
  }
  
  return StrTo16Uint(lenbuf);
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_getTCPData(uint32_t timeoutLimit, uint8_t* buf, uint16_t maxSize, uint16_t* actualDataLen)
{
  uint16_t len;
  uint16_t t16;

  if(esp8266_waitForMessage("+IPD,",timeoutLimit) == ESP8266_TIMEOUT)
  {
    *actualDataLen = 0;
    return ESP8266_TIMEOUT;
  }

  len = esp8266_getMessageLength();  

  /* Handle the overflow silently ... */
  if(len > maxSize)
  {
    len = maxSize;
  }

  /* Fetch the data ... */
  for(t16=0;t16<len;t16++)
  {
    m_timeoutCounter = 0;
    if(esp8266_waitForChar(ESP8266_1SecTimeout) == ESP8266_TIMEOUT)
    {      
      esp8266_hal_rebootSystem();
    }

    buf[t16] = RingBuffer_Remove(&esp8266_ringBuf);
  }

  *actualDataLen = len;

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
uint16_t StrTo16Uint(char* str)
{
  /* taken from: https://github.com/cnlohr/wi07clight/ */
  uint16_t ret = 0;
  uint8_t yet = 0;
  char c;
  while(c = (*(str++)))
  {
    if((c >= '0')&&(c <= '9'))
    {
      yet = 1;
      ret = ret * 10 + (c - '0');
    }
    else if( yet )
    {
      //Chars in the middle of the number.
      return ret;
    }
  }
  return ret;
}
/*---------------------------------------------------------------------------*/