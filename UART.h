#ifndef UART_H
#define UART_H

#include <stdint.h>

// Inicializaci�n
void UART_Ini     (uint8_t com, uint32_t baudrate,
                   uint8_t dataBits, uint8_t parity, uint8_t stopBits);

// Env�o
void UART_putchar(uint8_t com, char c);
void UART_puts   (uint8_t com, const char *s);
void UART_puts_slow(uint8_t com, const char *s);

// Recepci�n
void UART_gets   (uint8_t com, char *buf, uint8_t maxlen);
char UART_getchar(uint8_t com);
uint8_t UART_available(uint8_t com);
void UART_waitEnter(uint8_t com);

// ANSI-Escape sequences
void UART_clrscr  (uint8_t com);
void UART_setColor(uint8_t com, uint8_t color);
void UART_gotoxy  (uint8_t com, uint8_t x, uint8_t y);

// Colores b�sicos
#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7

#endif // UART_H
