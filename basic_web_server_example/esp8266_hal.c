/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "esp8266_hal.h"
/*---------------------------------------------------------------------------*/
ISR(USARTC0_RXC_vect)
{ 
  uint8_t ch = USARTC0.DATA; 

  if(!RingBuffer_IsFull(&esp8266_ringBuf))
  {
    RingBuffer_Insert(&esp8266_ringBuf,ch);
  }

}
/*---------------------------------------------------------------------------*/
void esp8266_hal_init()
{
  /* Wifi module reset & control pins */
  pinMode(C,4,OUTPUT);
  digitalWrite(C,4,HIGH);

  pinMode(C,5,OUTPUT);
  digitalWrite(C,5,HIGH);

  /* Init the ring buffer ... */
  RingBuffer_InitBuffer(&esp8266_ringBuf, esp8266_ringBufData, sizeof(esp8266_ringBufData));

  /* Set UART pin driver states */
  pinMode(C,2,INPUT);
  pinMode(C,3,OUTPUT);

  USARTC0.CTRLB = USART_RXEN_bm|USART_TXEN_bm;
  USARTC0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc|USART_PMODE_DISABLED_gc|USART_CHSIZE_8BIT_gc;

  /* 115200 baud rate with 32MHz clock */
  USARTC0.BAUDCTRLA = 131; USARTC0.BAUDCTRLB = (-3 << USART_BSCALE_gp);

  /* Enable UART data reception interrupt */
  USARTC0.CTRLA |= USART_DRIE_bm;
  
  /* Set UART data reception interrupt priority */
  USARTC0.CTRLA |= (1<<5);
  USARTC0.CTRLA |= (1<<4);
  
  /* Enable all interrupt levels */
  PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
  sei();
}
/*---------------------------------------------------------------------------*/
void esp8266_hal_rebootSystem()
{
  /* Apply complete software reset */
  CCP = CCP_IOREG_gc;
  RST.CTRL = RST_SWRST_bm;
}
/*---------------------------------------------------------------------------*/
void esp8266_hal_sendChar(uint8_t ch)
{
  while(!(USARTC0.STATUS & USART_DREIF_bm));
  USARTC0.DATA = ch;
}
/*---------------------------------------------------------------------------*/
/* Non-accurate but somewhat easy to port delay function */
void esp8266_hal_delayMiliseconds(uint16_t delay)
{  
  uint16_t tmp16;

  for(tmp16=0;tmp16<delay;tmp16++)
  {
    _delay_ms(1);
  }

}
/*---------------------------------------------------------------------------*/
/* Non-accurate but somewhat easy to port delay function */
void esp8266_hal_delayMicroseconds(uint16_t delay)
{
  uint16_t tmp16;

  for(tmp16=0;tmp16<delay;tmp16++)
  {
    _delay_us(1);
  }

}
/*---------------------------------------------------------------------------*/
