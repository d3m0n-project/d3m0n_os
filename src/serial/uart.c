#include "uart.h"
#include "types.h"
#include "math.h"
#include "d3m0n.h"
#include "time.h"
#include "filesystem.h"

#define UART0_BASE  0x20201000

#define UART0_DR    (*(volatile unsigned int*)(UART0_BASE + 0x00))
#define UART0_FR    (*(volatile unsigned int*)(UART0_BASE + 0x18))

static int  log_fd = -1;
static int  log_open_guard = 0;
static int  log_write_guard = 0;

#define LOG_TMP_BUF_SIZE 4096
static char log_tmp_buf[LOG_TMP_BUF_SIZE];
static size_t log_tmp_pos = 0;

#define LOG_FILE_PATH "/log.txt"

#ifndef LOGFILE_ENABLED
 #define LOGFILE_ENABLED 1
#endif

void    log_cleanup(void)
{
    #if LOGFILE_ENABLED == 1
    if (file_delete(LOG_FILE_PATH))
        uart_print("could not remove log\n");
    #else
    return;
    #endif
}

void    uart_putc(char c)
{
    if (c == '\n')
        uart_putc('\r');

    while (UART0_FR & (1 << 5)) // TX full
        continue;

    UART0_DR = c;
}

void uart_print(const char* str)
{
    size_t  i = 0;
    #if LOGFILE_ENABLED == 1
    if (log_fd == -1 && !log_open_guard && filesystem_ready())
    {
        log_open_guard = 1;
        log_fd = open(LOG_FILE_PATH, O_WRITE | O_CREATE | O_APPEND);
        log_open_guard = 0;
        if (log_fd < 0)
            log_fd = -2;
    }
    #endif
    while(str[i])
    {
        uart_putc(str[i]);
        i++;
    }
    #if LOGFILE_ENABLED == 1
    for (size_t j = 0; j < i; j++)
    {
        log_tmp_buf[log_tmp_pos++] = str[j];
        if (log_tmp_pos >= LOG_TMP_BUF_SIZE)
        {
            if (!log_write_guard && !log_open_guard)
            {
                log_write_guard = 1;
                if (log_fd < 0 && filesystem_ready())
                {
                    log_open_guard = 1;
                    log_fd = open("log.txt", O_WRITE | O_CREATE | O_APPEND);
                    log_open_guard = 0;
                    if (log_fd < 0)
                        log_fd = -2;
                }
                if (log_fd >= 0)
                {
                    size_t written = 0;
                    while (written < log_tmp_pos)
                    {
                        int w = write(log_fd, log_tmp_buf + written, (uint32_t)(log_tmp_pos - written));
                        if (w <= 0)
                            break;
                        written += (size_t)w;
                    }
                    if (written == log_tmp_pos)
                        log_tmp_pos = 0;
                    else
                    {
                        size_t rem = log_tmp_pos - written;
                        for (size_t k = 0; k < rem; k++)
                            log_tmp_buf[k] = log_tmp_buf[written + k];
                        log_tmp_pos = rem;
                    }
                }
                log_write_guard = 0;
            }
        }
    }
    #endif
}

void uart_flush_log_buffer(void)
{
    if (log_tmp_pos == 0)
        return;
    if (log_write_guard || log_open_guard)
        return;

    log_write_guard = 1;
    if (log_fd < 0 && filesystem_ready())
    {
        log_open_guard = 1;
        log_fd = open(LOG_FILE_PATH, O_WRITE | O_CREATE | O_APPEND);
        log_open_guard = 0;
        if (log_fd < 0)
        {
            log_fd = -2;
            log_write_guard = 0;
            return;
        }
    }
    if (log_fd < 0)
    {
        log_write_guard = 0;
        return;
    }

    size_t written = 0;
    while (written < log_tmp_pos)
    {
        int w = write(log_fd, log_tmp_buf + written, (uint32_t)(log_tmp_pos - written));
        if (w <= 0)
            break;
        written += (size_t)w;
    }
    if (written == log_tmp_pos)
        log_tmp_pos = 0;
    else
    {
        size_t rem = log_tmp_pos - written;
        for (size_t k = 0; k < rem; k++)
            log_tmp_buf[k] = log_tmp_buf[written + k];
        log_tmp_pos = rem;
    }
    log_write_guard = 0;
}

void    uart_putnbr_i(int nb)
{
    if (nb == 0)
    {
        uart_print("0");
        return;
    }
    if (nb < 0)
        uart_print("-");
    uart_putnbr_llu((unsigned long long)ABS(nb));
}

void    uart_putnbr_u(unsigned int nb)
{
    if (nb == 0)
    {
        uart_print("0");
        return;
    }
    uart_putnbr_llu((unsigned long long)nb);
}


void    uart_putnbr_llu(unsigned long long nb)
{
    char    buf[2] = {0};
    if (nb == 0)
        uart_print("0");
    else if (nb > 0)
    {
        if (nb / 10 > 0)
            uart_putnbr_llu(nb / 10);
        buf[0] = '0' + (nb % 10);
        uart_print(buf);
    }
}

void    uart_putnbr_llu_hex(unsigned long long nb)
{
    char    buf[2] = {0};
    char    set[17] = "0123456789ABCDEF";
    if (nb == 0)
    {
        uart_print("0");
    }
    else if (nb > 0)
    {
        if (nb / 16 > 0)
            uart_putnbr_llu_hex(nb / 16);
        buf[0] = set[nb % 16];
        uart_print(buf);
    }
}