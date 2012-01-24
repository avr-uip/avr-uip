#define UCSRA				UCSR0A
#define UCSRB				UCSR0B
#define UCSRC				UCSR0C
#define UDR					UDR0
#define UCR					UCSR0B
#define RXCIE				RXCIE0
#define TXCIE				TXCIE0
#define RXC					RXC0
#define TXC					TXC0
#define RXEN				RXEN0
#define TXEN				TXEN0
#define UBRRL				UBRR0L
#define UBRRH				UBRR0H
#define UCSZ0				UCSZ02
#define UPM0				UPM00
#define UPM1				UPM01
#define UCSZ1				UCSZ01
#define UDRE				UDRE0


	
#define SIG_UART_TRANS		SIG_USART_TRANS
#define SIG_UART_RECV		SIG_USART_RECV
#define SIG_UART_DATA		SIG_USART_DATA

void USART_Init(unsigned int);
void sendString(char* string);
void USART_Transmit( unsigned char data );
void USART_vSendByte(uint8_t u8Data);
