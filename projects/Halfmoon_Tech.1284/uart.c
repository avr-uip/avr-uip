#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart-t.h"

//Initialize the serial port, also called a universal asynchronous receive-transmit port
void USART_Init( unsigned int ubrr)
{
/*Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
//UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1 << RXCIE0);
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void sendString(char* string) {
	//PORTD = (1<<PORTD2);

	//While the character pointed to by 'string' is != NULL (0)
	while (*string) {

		//Wait until the hardware transmitter is ready
		while (!(UCSRA & _BV(UDRE)));

		//Send character then post-increment pointer to next character
		UDR = *(string++);
	}
	//PORTD = (0<<PORTD2);
}

void USART_Transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) )
;
/* Put data into buffer, sends the data */
UDR0 = data;
}

void USART_vSendByte(uint8_t u8Data)

{

// Wait if a byte is being transmitted

while((UCSRA&(1<<UDRE)) == 0);

// Transmit data

UDR = u8Data;

}
