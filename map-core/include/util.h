//
// Created by zijia on 2022/4/1.
//

#ifndef ZJMAP_UTIL_H
#define ZJMAP_UTIL_H

#include <iostream>
#include <cstdio>

FILE * open_file(const char * path, const char * mode);

template <class T>
T reverse_bytes(T x)
{
    char * bytes = (char *) &x;
    for (int i = 0; i < (sizeof(T) >> 1); i++)
    {
        std::swap(bytes[i], bytes[sizeof(T) - 1 - i]);
    }
    return x;
}

#if defined(_WIN32)
std::string GBK_2_UTF8(std::string gbkStr);
std::string UTF8_2_GBK(std::string utf8Str);
#endif

void enable_utf8_output();

#endif //ZJMAP_UTIL_H
