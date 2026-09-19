#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "usart.h"

uint8_t old_state[64];
uint8_t new_state[64];
uint8_t nbin[4];
uint8_t i, j, k;

// funkcija pretvara decimalan broj u binarni
void dec2bin(uint8_t n);

int16_t main()
{
	// D4,D5,D6,D7 selekcioni signali
    DDRD |= (1 << PD4) |
            (1 << PD5) |
            (1 << PD6) |
            (1 << PD7);

    // Postavljanje pinova 8, 9, 10 i 11 (PB0, PB1, PB2, PB3) kao ULAZE
    DDRB &= ~((1 << PB0) | 
	          (1 << PB1) | 
			  (1 << PB2) | 
			  (1 << PB3));
	
    // Inicijalizacija serijske komunikacije
    usartInit(9600);

    i = 0;
    for(i = 0; i < 64; i++){
        if(i > 15 && i < 47)
            old_state[i] = 1;
        else
            old_state[i] = 0;
    }

    while (1)
    {
        // Ocitavanje Hall senzora
		i = 0;
        for(i = 0; i < 4; i++){
			j = 0;
            for(j = 0; j < 16; j++){
                uint8_t temp, idx;
                dec2bin(j);
				
				k = 0;
                for (k = 0; k < 4; k++)
                {
                    if (nbin[k])
                        PORTD |= (1 << (PD4 + k));
                    else
                        PORTD &= ~(1 << (PD4 + k));
                }
                _delay_us(20);
				
                if (i == 0) {
                    temp = (PINB & (1 << PB0)); 
                    idx = (j / 4) + (j % 4) * 8;
                } else if (i == 1) {
                    temp = (PINB & (1 << PB1)) >> PB1;
                    idx = ((j / 4) + 4) + (j % 4) * 8;
                } else if (i == 2) {
                    temp = (PINB & (1 << PB2)) >> PB2;
                    idx = ((15 - j) / 4) + (((15 - j) % 4) + 4) * 8;
                } else {
                    temp = (PINB & (1 << PB3)) >> PB3;
                    idx = (((15 - j) / 4) + 4) + (((15 - j) % 4) + 4) * 8;
                }
                new_state[idx] = temp;
            }
        }

		// BLUETOOTH
        for (i = 0; i < 8; i++) 
        {
            uint8_t byte = 0; 
            for (j = 0; j < 8; j++) 
            {
                if (new_state[8 * i + j]) 
                    byte |= (1 << j); 
            }
            usartPutChar(byte);
        }
		_delay_ms(100);
    }

    return 0;
}

void dec2bin(uint8_t n)
{
	uint8_t i = 0;
    for(i = 0; i < 4; i++)
		nbin[i] = 0;

    i = 0;
    while (n && i < 4) {
        nbin[i++] = n % 2;
        n /= 2;
    }
}