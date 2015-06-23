/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "main.h"
/*---------------------------------------------------------------------------*/
int main()
{  
  int rc;
  int len = 0;
  uint8_t tmp8;
  int msgid = 1; 
  int mysock = 0;
  int req_qos = 0;  
  uint8_t buf[200];    
  int buflen = sizeof(buf);
  char* payload = "mypayload";
  int payloadlen = strlen(payload);  
  MQTTString topicString = MQTTString_initializer;
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

  init_hardware();

  transport_init();

  esp8266_init();
  
  /* Send data to PC */
  xfprintf(dbg,"Hello World!\r\n");
  
  /* Read the settings */
  eeprom_read_block(SSID_name,SSID_name_addr,32);
  eeprom_read_block(SSID_pass,SSID_pass_addr,32);

  /* Try to connect to network ... */
  esp8266_connectWifiNetwork(SSID_name,SSID_pass);    

  /*-------------------------------------------------------------------------*/
  transport_open("m11.cloudmqtt.com", 18979);
  
  data.clientID.cstring = "me";
  data.keepAliveInterval = 1000;
  data.cleansession = 1;
  data.username.cstring = "root";
  data.password.cstring = "pass";

  /*-------------------------------------------------------------------------*/
  len = MQTTSerialize_connect(buf, buflen, &data);
  rc = transport_sendPacketBuffer(mysock, buf, len);

  /* Wait for connack */
  if(MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
  {
    uint8_t sessionPresent, connack_rc;

    if((MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1) || (connack_rc != 0))
    {      
      esp8266_hal_rebootSystem();
    }
  }
  else
  {
    esp8266_hal_rebootSystem();
  }
  
  /*-------------------------------------------------------------------------*/
  /* subscribe */
  topicString.cstring = "substopic";
  len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
  rc = transport_sendPacketBuffer(mysock, buf, len);

  /* Wait for suback */
  if(MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK)
  {    
    int subcount;
    int granted_qos;
    uint16_t submsgid;

    rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
    if (granted_qos != 0)
    {
      esp8266_hal_rebootSystem();      
    }
  }
  else
  {
    esp8266_hal_rebootSystem();    
  }  

  /*-------------------------------------------------------------------------*/
  /* Loop getting msgs on subscribed topic and publish on specific topic */  
  topicString.cstring = "pubtopic";
  while (1)
  {             
    /*-----------------------------------------------------------------------*/
    /* Check any incoming message based on our subscriptions */
    while(MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
    {      
      int qos;
      uint8_t dup;            
      uint16_t msgid;
      uint8_t retained;
      int payloadlen_in;
      uint8_t* payload_in;            
      MQTTString receivedTopic;

      rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
          &payload_in, &payloadlen_in, buf, buflen);
        
      xfprintf(dbg,"> --------------------------\r\n");
      xfprintf(dbg,"> New msg!\r\n");
      
      /*---------------------------------------------------------------------*/
      xfprintf(dbg,"> Subject: ");
      for(tmp8=0;tmp8<(receivedTopic.lenstring.len);tmp8++)
      {
        dbg(receivedTopic.lenstring.data[tmp8]);
      }
      xfprintf(dbg,"\r\n");

      /*---------------------------------------------------------------------*/
      xfprintf(dbg,"> Payload: ");
      for(tmp8=0;tmp8<(payloadlen_in);tmp8++)
      {
        dbg(payload_in[tmp8]);
      }
      xfprintf(dbg,"\r\n");
    
    }    

    /*---------------------------------------------------------------------*/    
    /* Publish static content ... */
    len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (uint8_t*)payload, payloadlen);
    rc = transport_sendPacketBuffer(mysock, buf, len);

    /* Wait some time ... */
    _delay_ms(100);

  }
  
  return 0;
}

