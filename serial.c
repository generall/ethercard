/******************************************************************************/
/*  This file has been a part of the uVision/ARM development tools            */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  SERIAL.C:  Low Level Serial Routines                                      */
/*                                                                            */
/******************************************************************************/

/* 
   Martin Thomas : 
   - slightly modified for the WinARM example 
   - changed function names to avoid conflict with "stdio"
     (so far no syscalls for the newlib in this example)
   - Keil seems to "reject" the AT91lib*.h. Don't know
     why since the layer is "thin". Maybe will change this to 
	 "lib Style" in later version...
*/

// already done in Board.h: #include <AT91SAM7S64.H>                    /* AT91SAMT7S64 definitions */
#include "project.h"

#define BR    115200                        /* Baud Rate */

#define BRD  (MCK/16/BR)                    /* Baud Rate Divisor */


AT91S_USART * pUSART = AT91C_BASE_US0;      /* Global Pointer to USART0 */


void uart0_init (void) {                   /* Initialize Serial Interface */

  /* mt: n.b: uart0 clock must be enabled to use it */

  *AT91C_PIOA_PDR = AT91C_PA5_RXD0 |        /* Enable RxD0 Pin */
                    AT91C_PA6_TXD0;         /* Enalbe TxD0 Pin */

  pUSART->US_CR = AT91C_US_RSTRX |          /* Reset Receiver      */
                  AT91C_US_RSTTX |          /* Reset Transmitter   */
                  AT91C_US_RXDIS |          /* Receiver Disable    */
                  AT91C_US_TXDIS;           /* Transmitter Disable */

  pUSART->US_MR = AT91C_US_USMODE_NORMAL |  /* Normal Mode */
                  AT91C_US_CLKS_CLOCK    |  /* Clock = MCK */
                  AT91C_US_CHRL_8_BITS   |  /* 8-bit Data  */
                  AT91C_US_PAR_NONE      |  /* No Parity   */
                  AT91C_US_NBSTOP_1_BIT;    /* 1 Stop Bit  */

  pUSART->US_BRGR = BRD;                    /* Baud Rate Divisor */

  pUSART->US_CR = AT91C_US_RXEN  |          /* Receiver Enable     */
                  AT91C_US_TXEN;            /* Transmitter Enable  */
}


int uart0_putc(int ch) 
{
	while (!(pUSART->US_CSR & AT91C_US_TXRDY));   /* Wait for Empty Tx Buffer */
	return (pUSART->US_THR = ch);                 /* Transmit Character */
}	

int uart0_putchar (int ch) {                      /* Write Character to Serial Port */

  if (ch == '\n')  {                            /* Check for LF */
    uart0_putc( '\r' );                         /* Output CR */
  }
  return uart0_putc( ch );                     /* Transmit Character */
}

int uart0_puts ( char* s )
{
	while ( *s ) uart0_putchar( *s++ );
	return 0;
}

int uart0_kbhit( void ) /* returns true if character in receive buffer */
{
	if ( pUSART->US_CSR & AT91C_US_RXRDY) {
		return 1;
	}
	else {
		return 0;
	}
}

int uart0_getc ( void )  /* Read Character from Serial Port */
{    

  while (!(pUSART->US_CSR & AT91C_US_RXRDY));   /* Wait for Full Rx Buffer */
  return (pUSART->US_RHR);                      /* Read Character */
}



