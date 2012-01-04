
#define led_conf()      DDRC |= (1<<DDC0)
#define led_low()       PORTC |= (1<<PORTC0)
#define led_high()      PORTC &= ~(1<<PORTC0)
#define led_blink()     PORTC ^= (1<<PORTC0)

