/*-----------------------------------------------------------------------------
/
/
/----------------------------------------------------------------------------*/
#include "hardwareLayer.h"
/*---------------------------------------------------------------------------*/
void init_hardware()
{
  initClock_32Mhz();
  
  init_uart();
  
  init_adc();

  /* Software PWM timer */   
  TCC4.CTRLA = TC45_CLKSEL_DIV256_gc; /* 32Mhz / 256 => 125 kHz */
  TCC4.PER = 9; /* 125 kHz / (9+1) => 12.5 kHz */
  TCC4.INTCTRLA = 0x01; /* Overflow interrupt level */

  /* SPI Flash pins */
  pinMode(C,0,OUTPUT); // CS
  pinMode(C,1,OUTPUT); // CLK
  pinMode(C,6,INPUT); // SO
  pinMode(C,7,OUTPUT); // SI

  /* Button 0 */
  pinMode(D,3,INPUT);
  setInternalPullup(D,3);

  /* RGB LED */
  pinMode(D,0,OUTPUT);
  pinMode(D,1,OUTPUT);
  pinMode(D,2,OUTPUT);
}
/*---------------------------------------------------------------------------*/
/* Software PWM routine */
ISR(TCC4_OVF_vect)
{
  if(r_counter++ >= r)
  {
    digitalWrite(D,2,HIGH);
  }
  else
  {
    digitalWrite(D,2,LOW);
  }

  if(g_counter++ >= g)
  {
    digitalWrite(D,1,HIGH);
  }
  else
  {
    digitalWrite(D,1,LOW);
  }

  if(b_counter++ >= b)
  {
    digitalWrite(D,0,HIGH);
  }
  else
  {
    digitalWrite(D,0,LOW);
  }

  TCC4.INTFLAGS |= (1<<0);
}
/*---------------------------------------------------------------------------*/
void initClock_32Mhz()
{
  /* Generates 32Mhz clock from internal 2Mhz clock via PLL */
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | 16;
  OSC.CTRL |= OSC_PLLEN_bm ;
  while((OSC.STATUS & OSC_PLLRDY_bm) == 0);
  CCP = CCP_IOREG_gc;
  CLK.CTRL = CLK_SCLKSEL_PLL_gc;
}
/*---------------------------------------------------------------------------*/
void dbg(uint8_t ch)
{
  while(!(USARTD0.STATUS & USART_DREIF_bm));
  USARTD0.DATA = ch;
}
/*---------------------------------------------------------------------------*/
void init_uart()
{
  /*-------------------------------------------------------------------------*/
  /* Set UART pin driver states */
  pinMode(D,6,INPUT);
  pinMode(D,7,OUTPUT);

  /* Remap the UART pins */
  PORTD.REMAP = PORT_USART0_bm;

  USARTD0.CTRLB = USART_RXEN_bm|USART_TXEN_bm;
  USARTD0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc|USART_PMODE_DISABLED_gc|USART_CHSIZE_8BIT_gc;

  /* 115200 baud rate with 32MHz clock */
  USARTD0.BAUDCTRLA = 131; USARTD0.BAUDCTRLB = (-3 << USART_BSCALE_gp);
}
/*---------------------------------------------------------------------------*/
void init_adc()
{
  /* Use external 2048 mV reference */
  ADCA.REFCTRL = ADC_REFSEL_AREFA_gc;

  /* Set ADC clock prescaler */
  ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;

  /* Signed mode: GND to AREF = Effective 11 bits resolution */
  ADCA.CTRLB |= (1<<4);

  /* Single ended measurement */
  ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;

  /* Do averaging ... */
  ADCA.CH0.AVGCTRL = ADC_SAMPNUM_8X_gc | (3 << 4);

  /* Enable the ADC */
  ADCA.CTRLA = ADC_ENABLE_bm;
} 
/*---------------------------------------------------------------------------*/
uint16_t read_adc(uint8_t ch)
{
  /* Select ADC input pin */
  ADCA.CH0.MUXCTRL = ch << 3;
  
  /* Start conversion and wait until it is done ... */
  ADCA.CH0.CTRL |= ADC_CH_START_bm;
  while(!(ADCA.CH0.INTFLAGS & ADC_CH0IF_bm)); 
  ADCA.INTFLAGS = ADC_CH0IF_bm;
  
  /* Return the result */
  return ADCA.CH0.RES;
}
/*---------------------------------------------------------------------------*/
