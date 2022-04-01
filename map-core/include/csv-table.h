//
// Created by zijia on 2022/4/1.
//

#ifndef ZJMAP_CSV_TABLE_H
#define ZJMAP_CSV_TABLE_H

#include <cstdio>
#include <vector>
#include <map>
#include <string>

#include "string-convert.h"

std::vector<std::vector<std::string>> load_csv(const char * file_path);

std::map<std::string, std::vector<std::string>> load_csv_table(const char * file_path);

#endif //ZJMAP_CSV_TABLE_H
