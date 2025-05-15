#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "UART.h"

// C�lculo de UBRR a partir de F_CPU y baudrate
static uint16_t calc_ubrr(uint32_t baud) {
    return (uint16_t)(F_CPU/(16UL*baud) - 1UL);
}

void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t dataBits, uint8_t parity, uint8_t stopBits) {
    uint16_t ubrr = calc_ubrr(baudrate);
    volatile uint16_t *UBRRH = (com==0? &UBRR0H : com==2? &UBRR2H : &UBRR3H);
    volatile uint16_t *UBRRL = (com==0? &UBRR0L : com==2? &UBRR2L : &UBRR3L);
    volatile uint8_t *UCSRA  = (com==0? &UCSR0A : com==2? &UCSR2A : &UCSR3A);
    volatile uint8_t *UCSRB  = (com==0? &UCSR0B : com==2? &UCSR2B : &UCSR3B);
    volatile uint8_t *UCSRC  = (com==0? &UCSR0C : com==2? &UCSR2C : &UCSR3C);

    *UBRRH = ubrr >> 8;
    *UBRRL = ubrr & 0xFF;

    // Enable RX/TX
    if (com==0) *UCSRB = (1<<RXEN0)|(1<<TXEN0);
    if (com==2) *UCSRB = (1<<RXEN2)|(1<<TXEN2);
    if (com==3) *UCSRB = (1<<RXEN3)|(1<<TXEN3);

    *UCSRC = 0;
    // tama�o de palabra: 5..8 bits ? UCSZx0,x1
    *UCSRC |= ((dataBits - 5)&0x03) << (com==0? UCSZ00 : com==2? UCSZ20 : UCSZ30);
    // paridad: 0=none,1=even,2=odd ? UPMx0,x1
    if (parity==1) *UCSRC |= (1<<(com==0? UPM01:com==2? UPM21:UPM31));
    if (parity==2) *UCSRC |= (1<<(com==0? UPM01:com==2? UPM21:UPM31))
                       | (1<<(com==0? UPM00:com==2? UPM20:UPM30));
    // stop bits: 1 � 2 ? USBSx
    if (stopBits==2) *UCSRC |= (1<<(com==0? USBS0:com==2? USBS2:USBS3));
}

char UART_getchar(uint8_t com) {
    switch (com) {
        case 0: while (!(UCSR0A & (1<<RXC0))); return UDR0;
        case 2: while (!(UCSR2A & (1<<RXC2))); return UDR2;
        case 3: while (!(UCSR3A & (1<<RXC3))); return UDR3;
        default: return 0;
    }
}

uint8_t UART_available(uint8_t com) {
    switch (com) {
        case 0: return (UCSR0A & (1<<RXC0));
        case 2: return (UCSR2A & (1<<RXC2));
        case 3: return (UCSR3A & (1<<RXC3));
        default: return 0;
    }
}

void UART_putchar(uint8_t com, char c) {
    if      (com==0) { while(!(UCSR0A&(1<<UDRE0))); UDR0=c; }
    else if (com==2) { while(!(UCSR2A&(1<<UDRE2))); UDR2=c; }
    else             { while(!(UCSR3A&(1<<UDRE3))); UDR3=c; }
}

void UART_puts(uint8_t com, const char *s) {
    while (*s) UART_putchar(com, *s++);
}

void UART_puts_slow(uint8_t com, const char *s) {
    while (*s) {
        UART_putchar(com, *s++);
        if (com == 2) _delay_us(100);  // 0.1 ms -> 115200 baud
    }
}

void UART_gets(uint8_t com, char *buf, uint8_t maxlen) {
    uint8_t idx = 0;
    char c;
    for (;;) {
        // leo un byte
        if      (com==0) { while(!(UCSR0A&(1<<RXC0))); c=UDR0; }
        else if (com==2) { while(!(UCSR2A&(1<<RXC2))); c=UDR2; }
        else             { while(!(UCSR3A&(1<<RXC3))); c=UDR3; }

        // CR/LF ? fin
        if (c=='\r' || c=='\n') {
            UART_putchar(com, '\r');
            UART_putchar(com, '\n');
            break;
        }
        // backspace / DEL
        else if ((c=='\b' || c==0x7F) && idx>0) {
            idx--;
            UART_puts(com, "\b \b");
        }
        // imprimible
        else if (c>=' ' && idx<maxlen-1) {
            buf[idx++] = c;
            UART_putchar(com, c);
        }
        // else ignorar
    }
    buf[idx] = '\0';
}

void UART_clrscr(uint8_t com) {
    UART_puts(com, "\x1B[2J");
}

void UART_setColor(uint8_t com, uint8_t color) {
    char cmd[8];
    snprintf(cmd, sizeof(cmd), "\x1B[%dm", 30 + (color & 7));
    UART_puts(com, cmd);
}

void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y) {
    char t[12];
    snprintf(t, sizeof(t), "\x1B[%u;%uH", (unsigned)y, (unsigned)x);
    UART_puts(com, t);
}

void UART_waitEnter(uint8_t com) {
    char c;
    do {
        c = UART_getchar(com);
    } while (c != '\r' && c != '\n');
}
