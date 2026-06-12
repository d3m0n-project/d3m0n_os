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

#define LOG_TMP_BUF_SIZE 65536
static char log_tmp_buf[LOG_TMP_BUF_SIZE];
static size_t log_tmp_pos = 0;

#define LOG_FILE_PATH "/log.txt"

#ifndef LOGFILE_ENABLED
 #define LOGFILE_ENABLED 1
#endif

#if LOGFILE_ENABLED == 1
static void log_buffer_append(const char *str, size_t len)
{
    size_t  i;

    i = 0;
    while (i < len && log_tmp_pos < LOG_TMP_BUF_SIZE)
    {
        log_tmp_buf[log_tmp_pos] = str[i];
        log_tmp_pos++;
        i++;
    }
}

static int  log_file_open(void)
{
    if (log_fd >= 0)
        return 0;
    if (!filesystem_ready() || log_open_guard || log_write_guard)
        return -1;
    log_open_guard = 1;
    log_fd = open(LOG_FILE_PATH, O_WRITE | O_CREATE | O_APPEND);
    log_open_guard = 0;
    if (log_fd < 0)
    {
        log_fd = -2;
        return -1;
    }
    return 0;
}

static size_t log_file_write(const char *buf, size_t len)
{
    size_t  written;
    int     result;

    if (len == 0 || log_file_open() != 0 || log_write_guard)
        return 0;
    log_write_guard = 1;
    written = 0;
    while (written < len)
    {
        result = write(log_fd, buf + written, (uint32_t)(len - written));
        if (result <= 0)
            break;
        written += (size_t)result;
    }
    log_write_guard = 0;
    return written;
}

static void log_buffer_discard(size_t count)
{
    size_t  i;
    size_t  rem;

    if (count >= log_tmp_pos)
    {
        log_tmp_pos = 0;
        return;
    }
    rem = log_tmp_pos - count;
    i = 0;
    while (i < rem)
    {
        log_tmp_buf[i] = log_tmp_buf[count + i];
        i++;
    }
    log_tmp_pos = rem;
}

static void log_buffer_flush(void)
{
    size_t  written;

    if (log_tmp_pos == 0 || log_open_guard || log_write_guard)
        return;
    written = log_file_write(log_tmp_buf, log_tmp_pos);
    log_buffer_discard(written);
}

static void log_file_record(const char *str, size_t len)
{
    size_t  written;

    if (log_open_guard || log_write_guard || !filesystem_ready())
    {
        log_buffer_append(str, len);
        return;
    }
    log_buffer_flush();
    written = log_file_write(str, len);
    if (written < len)
        log_buffer_append(str + written, len - written);
}
#endif

void    log_cleanup(void)
{
    #if LOGFILE_ENABLED == 1
    if (log_fd >= 0)
        close(log_fd);
    log_fd = -1;
    if (filesystem_ready() && file_exists(LOG_FILE_PATH))
        file_delete(LOG_FILE_PATH);
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

    while(str[i])
    {
        uart_putc(str[i]);
        i++;
    }
    #if LOGFILE_ENABLED == 1
    log_file_record(str, i);
    #endif
}

void uart_flush_log_buffer(void)
{
    #if LOGFILE_ENABLED == 1
    log_buffer_flush();
    #endif
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
