#pragma once

#include <cstdarg>
#include <cstdio>

namespace modloader::ui
{
    void raw_log_string(const char* string);
    void init();
    void exit();
    void enter_loop();

    inline void raw_log_fmt(const char* fmt, ...)
    {
        std::va_list args;
        va_start(args, fmt);

        auto string_length = std::vsnprintf(nullptr, 0, fmt, args);
        auto buffer = new char[string_length + 1];
        std::vsnprintf(buffer, string_length + 1, fmt, args);
        buffer[string_length] = '\0';

        raw_log_string(buffer);

        delete buffer;
        va_end(args);
    }
}
