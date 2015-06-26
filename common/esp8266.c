/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "esp8266.h"
/*---------------------------------------------------------------------------*/
static uint32_t m_timeoutCounter; 
/*---------------------------------------------------------------------------*/
static void esp8266_flushBuffer();
static uint8_t esp8266_getMessageID();
static uint16_t StrTo16Uint(char* str);
static uint16_t esp8266_getMessageLength();
static int8_t esp8266_waitForChar(uint32_t timeoutLimit);
static int8_t esp8266_checkOkError(uint32_t timeoutLimit);
static int8_t esp8266_waitForMessage(const char* checkmsg, uint32_t timeoutLimit);
/*---------------------------------------------------------------------------*/
int8_t esp8266_init()
{
  esp8266_hal_init();  

  /* Wait for the powerup */
  esp8266_waitForMessage("ready\r\n",ESP8266_10SecTimeout);
  
  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_connectWifiNetwork(char* ssidName, char* password)
{
  uint8_t ipTrial = 0;

  /* Change operating mode */
  xfprintf(esp8266_hal_sendChar,"AT+CWMODE=1\r\n");

  /* Fixed delay ... */
  esp8266_hal_delayMiliseconds(100);

  /* Reset the wifi module */  
  xfprintf(esp8266_hal_sendChar,"AT+RST\r\n");

  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }

  /* Wait for the powerup */
  esp8266_waitForMessage("ready\r\n",ESP8266_10SecTimeout);

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
int8_t esp8266_createWifiNetwork(char* ssidName, char* password)
{
  uint8_t ipTrial = 0;

  /* Change operating mode */
  xfprintf(esp8266_hal_sendChar,"AT+CWMODE=2\r\n");

  /* Fixed delay ... */
  esp8266_hal_delayMiliseconds(100);

  /* Reset the wifi module */
  xfprintf(esp8266_hal_sendChar,"AT+RST\r\n");

  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }

  /* Wait for the powerup */
  esp8266_waitForMessage("ready\r\n",ESP8266_10SecTimeout);  

  /* SSID details */
  xfprintf(esp8266_hal_sendChar,"AT+CWSAP=\"%s\",\"%s\",1,3\r\n",ssidName,password);

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
int8_t esp8266_openTCPSocket(uint8_t sockId, char* address, uint16_t port)
{
  /* TCP socket details */
  xfprintf(esp8266_hal_sendChar,"AT+CIPSTART=%d,\"TCP\",\"%s\",%d\r\n",sockId,address,port);
  
  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }  
  
  esp8266_waitForMessage("Linked\r\n",ESP8266_10SecTimeout);

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_createTCPSocket(uint16_t port)
{
  /* Host a TCP server at a specific port */
  xfprintf(esp8266_hal_sendChar,"AT+CIPSERVER=1,%u\r\n",port);

  /* Reboot if neccessary */
  if(esp8266_checkOkError(ESP8266_10SecTimeout) != ESP8266_OK)
  {
    esp8266_hal_rebootSystem();
  }

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_closeTCPLink(uint8_t sockId)
{  
  xfprintf(esp8266_hal_sendChar,"AT+CIPCLOSE=%d\r\n",sockId);

  return ESP8266_OK;
}
/*---------------------------------------------------------------------------*/
int8_t esp8266_sendTCPData(uint32_t timeoutLimit, uint8_t sockId, uint8_t* buf, uint16_t len)
{
  uint16_t t16;

  /* Prepare to send data ... */
  xfprintf(esp8266_hal_sendChar,"AT+CIPSEND=%d,%d\r\n",sockId,len);

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
int8_t esp8266_getTCPData(uint32_t timeoutLimit, uint8_t* buf, uint16_t maxSize, uint16_t* actualDataLen, uint8_t* sockId)
{
  uint16_t len;
  uint16_t t16;

  memset(buf,0x00,maxSize);

  if(esp8266_waitForMessage("+IPD,",timeoutLimit) == ESP8266_TIMEOUT)
  {
    *actualDataLen = 0;
    return ESP8266_TIMEOUT;
  }

  *sockId = esp8266_getMessageID();  
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
/*-----------------------------------------------------------------------------
/ fill a binary string of len data into the tcp packet
/ taken from tuxgraphics ip stack
/----------------------------------------------------------------------------*/
uint16_t esp8266_fill_tcp_data_len(uint8_t *buf,uint16_t pos, const uint8_t *s, uint8_t len)
{
  // fill in tcp data at position pos
  while (len) 
  {
    buf[pos]=*s;
    pos++;
    s++;
    len--;
  }
  return(pos);
}
/*-----------------------------------------------------------------------------
/ fill in tcp data at position pos. pos=0 means start of
/ tcp data. Returns the position at which the string after
/ this string could be filled.
/ taken from tuxgraphics ip stack
/----------------------------------------------------------------------------*/
uint16_t esp8266_fill_tcp_data(uint8_t *buf,uint16_t pos, const char *s)
{
  return (esp8266_fill_tcp_data_len(buf,pos,(uint8_t*)s,strlen(s)));
}
/*---------------------------------------------------------------------------*/
static int8_t esp8266_waitForChar(uint32_t timeoutLimit)
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
static int8_t esp8266_waitForMessage(const char* checkmsg, uint32_t timeoutLimit)
{
  uint8_t ch;
  uint8_t run = 1;
  uint16_t in = 0;  
  
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
static int8_t esp8266_checkOkError(uint32_t timeoutLimit)
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
static void esp8266_flushBuffer()
{
  while(!RingBuffer_IsEmpty(&esp8266_ringBuf))
  {
    RingBuffer_Remove(&esp8266_ringBuf);
  }
}
/*---------------------------------------------------------------------------*/
static uint8_t esp8266_getMessageID()
{
  uint8_t run;
  uint8_t in = 0;  
  uint8_t lenbuf[8]; 
  
  run = 1;  
  while(run)
  {
    m_timeoutCounter = 0;
    if(esp8266_waitForChar(ESP8266_1SecTimeout) == ESP8266_TIMEOUT)
    {      
      esp8266_hal_rebootSystem();
    }

    lenbuf[in] = RingBuffer_Remove(&esp8266_ringBuf);

    if(lenbuf[in] == ',')
    {      
      lenbuf[in] = '\0';
      run = 0;
    }
    else
    {
      in++;
    }
  }

  return StrTo16Uint(lenbuf);
}
/*---------------------------------------------------------------------------*/
static uint16_t esp8266_getMessageLength()
{
  uint8_t run;
  uint8_t in = 0;  
  uint8_t lenbuf[8];  
  
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
static uint16_t StrTo16Uint(char* str)
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