//
// Created by zijia on 2022/4/1.
//

#include "util.h"

FILE *open_file(const char *path, const char *mode) {
    FILE * file;
#ifdef _MSC_VER
    fopen_s(&file, path, mode);
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
static BOOL console_utf8 = SetConsoleOutputCP(65001);
#else
static int32_t console_utf8 = 1;
#endif