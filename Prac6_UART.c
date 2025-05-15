#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "UART.h"

int main(void)
{
    char cad[20], eco[20];
    uint16_t num;

    UART_Ini(0, 12345, 8, 1, 2);
    UART_Ini(2, 115200, 8, 0, 1);
    UART_Ini(3, 115200, 8, 0, 1);

    UART_clrscr(0); // Limpia pantalla
    UART_setColor(0, CYAN);
    UART_puts(0, "Practica 6 - UART\r\n");
    UART_setColor(0, WHITE); // Restaurar color por defecto

    for (;;) {
        UART_clrscr(0); // Limpia pantalla
		UART_setColor(0, YELLOW);
        UART_puts(0, "Introduce un numero: ");
        UART_setColor(0, GREEN);
        UART_gets(0, cad, sizeof(cad));

        // si hay decimal, tronca
        for (char *p = cad; *p; ++p)
            if (*p == '.') { *p = '\0'; break; }

        // mando lento a COM2 (y luego leo en COM3)
        UART_puts_slow(2, cad);
        UART_puts(2, "\r");
        UART_gets(3, eco, sizeof(eco));
        while (UART_available(3)) {
            char basura = UART_getchar(3);
            if (basura == '\n') break;
        }

        UART_setColor(0,GREEN);
        UART_puts(0, "Recibe: ");
        UART_setColor(0, MAGENTA);
        UART_puts(0, eco);
        UART_puts(0, "\r\n");

        num = (uint16_t)atoi(cad);

        itoa(num, cad, 16);
        UART_setColor(0, CYAN);
        UART_puts(0, "Hex: ");
        UART_puts(0, cad);
        UART_puts(0, "\r\n");

        itoa(num, cad, 2);
        UART_setColor(0, RED);
        UART_puts(0, "Bin: ");
        UART_puts(0, cad);
        UART_puts(0, "\r\n\r\n");

        UART_setColor(0, WHITE);
        _delay_ms(500);
		UART_waitEnter(0);
    }
}

