#ifndef serial_h_
#define serial_h_

/* mt: file added */

#ifdef __cplusplus
extern "C" {
#endif


void uart0_init (void);
int uart0_putc(int ch); 
int uart0_putchar (int ch);

void uart0_puts(char *s);

int uart0_kbhit( void );
int uart0_getc ( void );

#ifdef __cplusplus
}
#endif


#endif
