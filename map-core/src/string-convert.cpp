//
// Created by zijia on 2022/4/1.
//

#include "string-convert.h"
#include "util.h"


std::wstring utf8_to_unicode(std::string cstr) {
    std::wstring result = L"";
    int32_t len = 0;
    for (uint8_t c : cstr)
    {
        if ((c & 0b11000000u) == 0b10000000u)
        {
            result[len - 1] <<= 6;
            result[len - 1] |= c & 0b00111111u;
        }
        else
        {
            uint8_t mask = 0b10000000u;
            while (mask & c)
            {
                c &= ~mask;
                mask >>= 1;
            }
            result += c;
            len++;
        }
    }
    return result;
}

std::string unicode_to_utf8(std::wstring wstr) {
    std::string res = "";
    for (wchar_t c : wstr)
    {
        if (c & 0b1111111110000000)
        {
            if (c & 0b1111100000000000)
            {
                res += (char)(((c >> 12) & 0b00001111) | 0b11100000);
                res += (char)(((c >> 6) & 0b00111111) | 0b10000000);
                res += (char)((c & 0b00111111) | 0b10000000);
            }
            else
            {
                res += (char)(((c >> 6) & 0b00011111) | 0b11000000);
                res += (char)((c & 0b00111111) | 0b10000000);
            }
        }
        else
        {
            res += (char)(c&0b01111111);
        }
    }
    return res;
}

double string_to_real(std::string number) {
    int mode = 2;
    double res = 0, s = 0.1, t = 1;
    for(char c : number)
    {
        if (isdigit(c))
        {
            if (mode >= 1)
                res = res * 10.0 + (c - '0');
            else
                res = res + (c - '0') * s, s *= 0.1;

            if (mode == 2) mode = 1;
        }
        else if (c == '.')
        {
            if (mode == 0)
                return NAN;
            else
                mode = 0;
        }
        else if (c == '-')
        {
            if (mode == 2)
                mode = 1, t = -1;
            else
                return NAN;
        }
        else if(!isblank(c))
            return NAN;
    }
    return res * t;
}

int32_t string_to_integer(std::string number) {
    int32_t x = 0, t = 1, mode = 1;
    for (char c : number)
    {
        if (isdigit(c))
            x = x * 10 + c - '0', mode = 0;
        else if(c == '-')
        {
            if (mode == 1) t = -1;
            else return -1;
            mode = 0;
        }
        else if(!isblank(c))
            return -1;
    }
    return x * t;
}

