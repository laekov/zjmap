//
// Created by zijia on 2022/4/1.
//

#include "csv-table.h"
#include "util.h"

std::vector<std::vector<std::string>> load_csv(const char *file_path) {
    FILE * file = open_file(file_path, "rb");
    std::vector<std::vector<std::string>> result;
    if (!file) return result;
    char BOM[3];
    fread(BOM, 1, 3, file);
    int32_t c;

    std::string entry = "";
    std::vector<std::string> line;

    do
    {
        c = fgetc(file);
        if (c == '\r') continue;
        if (c == '\n' ||  c == ',' || c == -1 || c == 0)
        {
            line.push_back(entry);
            entry = "";
            if (c != ',')
            {
                result.push_back(line);
                line.clear();
            }
        }
        else
            entry += (char)c;
    }while (c != -1);
    fclose(file);
    return result;
}

std::map<std::string, std::vector<std::string>> load_csv_table(const char *file_path) {
    auto table = load_csv(file_path);
    std::map<std::string, std::vector<std::string>> result;
    if (table.size())
    {
        auto first_line = table[0];
        for (int col = 0; col < first_line.size(); col++)
        {
            std::vector<std::string> column;
            for (int row = 1; row < table.size(); row++)
                if (table[row].size() > col)
                    column.push_back(table[row][col]);
                else
                    column.push_back("");

            result[first_line[col]] = column;
        }
    }
    return result;
}

