//
// Created by zijia on 2022/4/1.
//

#include "util.h"

FILE *open_file(const char *path, const char *mode) {
    FILE * file;
#ifdef _MSC_VER

    fopen_s(&file, UTF8_2_GBK(path).c_str(), mode);
#else
    file = fopen(path, mode);
#endif
#ifndef NDEBUG
    if (!file)
        printf("[FAILED] cannot open file (%s)\n", path);
#endif
    return file;
}

#ifdef _WIN32
#include <Windows.h>
void enable_utf8_output() {
    SetConsoleOutputCP(65001);
}

std::string GBK_2_UTF8(std::string gbkStr)
{
    std::string outUtf8 = "";
    int n = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
    WCHAR *str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1,str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char *str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    outUtf8 = str2;
    delete[]str1;
    str1 = NULL;
    delete[]str2;
    str2 = NULL;
    return outUtf8;
}


std::string UTF8_2_GBK(std::string utf8Str)
{
    std::string outGBK = "";
    int n = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    WCHAR *str1 = new WCHAR[n];
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, str1, n);
    n = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
    char *str2 = new char[n];
    WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, n, NULL, NULL);
    outGBK = str2;
    delete[] str1;
    str1 = NULL;
    delete[] str2;
    str2 = NULL;
    return outGBK;
}

#else
void enable_utf8_output() {
    ;
}
#endif
