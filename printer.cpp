#include "printer.hpp"

void print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // setting color
    std::cout << "\033[0;" << colorCode << "m";

    std::cout << "[" << rank << ", " << timestamp << "] ";
    std::string buf = "";

    while (*fmt != '\0') {
        if (*fmt == '%') {
            std::cout << buf;
            buf = "";
            ++fmt;
            if (*fmt == 'd') {
                int i = va_arg(args, int);
                std::cout << i;
            } else if (*fmt == 'c') {
                int c = va_arg(args, int);
                std::cout << static_cast<char>(c);
            } else if (*fmt == 'f') {
                double d = va_arg(args, double);
                std::cout << d;
            } else if (*fmt == 's') {
                const char* s = va_arg(args, const char*);
                std::cout << s;
            }
            ++fmt;
            continue;
        }
        buf += *fmt;
        ++fmt;
    }

    if (buf.size())
        std::cout << buf;

    // reseting color
    std::cout << "\033[0m" << std::endl;

    va_end(args);
}