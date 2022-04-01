//
// Created by zijia on 2022/4/1.
//

#ifndef ZJMAP_STRING_CONVERT_H
#define ZJMAP_STRING_CONVERT_H

#include <cstdint>
#include <cwchar>
#include <string>
#include <cstdio>

std::wstring utf8_to_unicode(std::string cstr);

std::string unicode_to_utf8(std::wstring wstr);

double string_to_real(std::string number);

int32_t string_to_integer(std::string number);

#endif //ZJMAP_STRING_CONVERT_H
