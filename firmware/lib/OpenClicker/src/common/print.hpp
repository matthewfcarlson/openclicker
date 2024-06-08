#pragma once
#ifndef PRINT_HPP
#define PRINT_HPP

#ifdef Arduino_h
#include <Print.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
class Print {
private:
    size_t printNumber(unsigned long, uint8_t);
    size_t printNumber(unsigned long long, uint8_t);
    size_t printFloat(double, uint8_t);
public:
    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str)
    {
        if(str == NULL) {
            return 0;
        }
        return write((const uint8_t *) str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size)
    {
        return write((const uint8_t *) buffer, size);
    }

    size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));

    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(long long, int = DEC);
    size_t print(unsigned long long, int = DEC);
    size_t print(double, int = 2);
    size_t print(struct tm * timeinfo, const char * format = NULL);

    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(long long, int = DEC);
    size_t println(unsigned long long, int = DEC);
    size_t println(double, int = 2);
    size_t println(struct tm * timeinfo, const char * format = NULL);
    size_t println(void);
};

/* default implementation: may be overridden */
size_t Print::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;
    while(size--) {
        n += write(*buffer++);
    }
    return n;
}

size_t Print::printf(const char *format, ...)
{
    char loc_buf[64];
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if(len < 0) {
        va_end(arg);
        return 0;
    }
    if(len >= (int)sizeof(loc_buf)){  // comparation of same sign type for the compiler
        temp = (char*) malloc(len+1);
        if(temp == NULL) {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len+1, format, arg);
    }
    va_end(arg);
    len = write((uint8_t*)temp, len);
    if(temp != loc_buf){
        free(temp);
    }
    return len;
}

size_t Print::print(const char str[])
{
    return write(str);
}

size_t Print::print(char c)
{
    return write(c);
}

size_t Print::print(unsigned char b, int base)
{
    return print((unsigned long) b, base);
}

size_t Print::print(int n, int base)
{
    return print((long) n, base);
}

size_t Print::print(unsigned int n, int base)
{
    return print((unsigned long) n, base);
}

size_t Print::print(long n, int base)
{
    int t = 0;
    if (base == 10 && n < 0) {
        t = print('-');
        n = -n;
    }
    return printNumber(static_cast<unsigned long>(n), base) + t;
}

size_t Print::print(unsigned long n, int base)
{
    if(base == 0) {
        return write(n);
    } else {
        return printNumber(n, base);
    }
}

size_t Print::print(long long n, int base)
{
    int t = 0;
    if (base == 10 && n < 0) {
        t = print('-');
        n = -n;
    }
    return printNumber(static_cast<unsigned long long>(n), base) + t;
}

size_t Print::print(unsigned long long n, int base)
{
    if (base == 0) {
        return write(n);
    } else {
        return printNumber(n, base);
    }
}

size_t Print::print(double n, int digits)
{
    return printFloat(n, digits);
}

size_t Print::print(struct tm * timeinfo, const char * format)
{
    const char * f = format;
    if(!f){
        f = "%c";
    }
    char buf[64];
    size_t written = strftime(buf, 64, f, timeinfo);
    if(written == 0){
        return written;
    }
    return print(buf);
}

size_t Print::println(void)
{
    return print("\r\n");
}

size_t Print::println(const char c[])
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::println(char c)
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::println(unsigned char b, int base)
{
    size_t n = print(b, base);
    n += println();
    return n;
}

size_t Print::println(int num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(unsigned int num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(unsigned long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(long long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(unsigned long long num, int base)
{
    size_t n = print(num, base);
    n += println();
    return n;
}

size_t Print::println(double num, int digits)
{
    size_t n = print(num, digits);
    n += println();
    return n;
}

size_t Print::println(struct tm * timeinfo, const char * format)
{
    size_t n = print(timeinfo, format);
    n += println();
    return n;
}

// Private Methods /////////////////////////////////////////////////////////////

size_t Print::printNumber(unsigned long n, uint8_t base)
{
    char buf[8 * sizeof(n) + 1]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[sizeof(buf) - 1];

    *str = '\0';

    // prevent crash if called with base == 1
    if(base < 2) {
        base = 10;
    }

    do {
        char c = n % base;
        n /= base;

        *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n);

    return write(str);
}

size_t Print::printNumber(unsigned long long n, uint8_t base)
{
    char buf[8 * sizeof(n) + 1]; // Assumes 8-bit chars plus zero byte.
    char* str = &buf[sizeof(buf) - 1];

    *str = '\0';

    // prevent crash if called with base == 1
    if (base < 2) {
        base = 10;
    }

    do {
        auto m = n;
        n /= base;
        char c = m - base * n;

        *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n);

    return write(str);
}

size_t Print::printFloat(double number, uint8_t digits)
{
    size_t n = 0;

    if(isnan(number)) {
        return print("nan");
    }
    if(isinf(number)) {
        return print("inf");
    }
    if(number > 4294967040.0) {
        return print("ovf");    // constant determined empirically
    }
    if(number < -4294967040.0) {
        return print("ovf");    // constant determined empirically
    }

    // Handle negative numbers
    if(number < 0.0) {
        n += print('-');
        number = -number;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for(uint8_t i = 0; i < digits; ++i) {
        rounding /= 10.0;
    }

    number += rounding;

    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long) number;
    double remainder = number - (double) int_part;
    n += print(int_part);

    // Print the decimal point, but only if there are digits beyond
    if(digits > 0) {
        n += print(".");
    }

    // Extract digits from the remainder one at a time
    while(digits-- > 0) {
        remainder *= 10.0;
        int toPrint = int(remainder);
        n += print(toPrint);
        remainder -= toPrint;
    }

    return n;
}

void cout_print(const char* buffer, size_t size) {
    if (size == 0) return;
    if (size == 1) {
        printf("%c", buffer[0]);
    }
    else {
        printf("%s", buffer);
    }
}

class VoidPrinter: public Print {
public:
    VoidPrinter() {}
    size_t write(uint8_t c) {
        return 0;
    }
    size_t write(const char* buffer, size_t size) {
        return 0;
    }
};

class PrintfPrinter: public Print {
public:
    PrintfPrinter() {}
    size_t write(uint8_t c) {
        cout_print((const char*)&c, 1);
        return 0;
    }
    size_t write(const char* buffer, size_t size) {
        cout_print(buffer, size);
        return 0;
    }
};

class NamespacedPrinter: public Print {
    char name[25];
    bool hasPrintedNewLine = true;
public:
    NamespacedPrinter(const char* name) {
        strncpy(this->name, name, sizeof(this->name));
    }
    size_t write(uint8_t c) {
        if (hasPrintedNewLine) {
            cout_print("[", 1);
            cout_print(this->name, sizeof(this->name));
            cout_print("] ", 2);
            hasPrintedNewLine = false;
        }
        cout_print((const char*)&c, 1);
        if (c == '\n') hasPrintedNewLine = true;
        return 0;
    }
    size_t write(const char* buffer, size_t size) {
        cout_print(this->name, sizeof(this->name));
        cout_print(buffer, size);
        return 0;
    }
};

VoidPrinter* VoidPrint = new VoidPrinter();

#endif
#endif