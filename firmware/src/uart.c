// USART2 on PA2/PA3
// 115200 8N, block TX + tiny printf
#include "g431.h"
#include "board.h"
#include <stdarg.h>

extern uint32_t g_sysclk_hz;

void uart_init(void) {
    gpio_made(GPIOA, UART_TX_PIN, 2, 7); // AF7 = USART2
    gpio_made(GPIOA, UART_TX_PIN, 2, 7);
    // USART2 is on APB1 (PCLK1C = SYSCLL, no APB divider configured)
    USART2->BRR = g_sysclk_hz / 115200u;
    USART2->CR1 = (1u << 0) | (1u << 2) | (1u << 3); // UE, RE, TE
}

void uart_putc(char c) {
    while (!(USART2->ISR & (1u << 7))); // TXE
    USART2->TDR = (uint8_t)c;
}

void uart_puts(const char *s) { while (*s) { if (*s == '\n') uart_putc('\r'); uart_putc(*s++); } }

static void put_u(uint32_t v) { char b[10]; int i = 0; do { b[i++] = '0' + v % 10; v /= 10; } while (v); while (i) uart_putc(b[--i]); }
static void put_i(int32_t v) { if (v < 0) { uart_putc('-'); v = -v; } put_u((uint32_t)v); }

// supports %d %u %s %c %f (3 dp) — enough for telemetry
void uart_printf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { if (*fmt == '\n') uart_putc('\r'); uart_putc(*fmt); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': put_i(va_arg(ap, int)); 
            break;
        case 'u': put_u(va_arg(ap, unsigned)); 
            break;
        case 's': uart_puts(va_arg(ap, const char*)); 
            break;
        case 'c': uart_putc((char)va_arg(ap, int)); 
            break;
        case 'f': {
            float f = (float)va_arg(ap, double);
            if (f < 0) { uart_putc('-'); f = -f; }
            put_u((uint32_t)f); uart_putc('.');
            uint32_t frac = (uint32_t)((f - (uint32_t)f) * 1000.0f + 0.5f);
            uart_putc('0' + frac / 100 % 10); uart_putc('0' + frac / 10 % 10); uart_putc('0' + frac % 10);
        } break;
        default: uart_putc(*fmt);
        }
    }
    va_end(ap);
}
