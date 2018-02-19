#ifndef _MS_H_
#define _MS_H_

#include <stdint.h>

/** predispone timer per temporizzazione al millisecondo**/
void ms_init(void) ;

/**leggi contatore dei millisecondi dall'avvio**/
uint32_t ms();
void get_ms(uint32_t* t);

/**attendi t milisecondi e poi riprendi l'esecuzione**/
/**(nel frattempo ISR continuano a funzionare)**/
void delay_ms(uint32_t t);

#endif
