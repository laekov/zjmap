//
// Created by zijia on 2022/4/1.
//

#ifndef ZJMAP_SHAPEFILE_H
#define ZJMAP_SHAPEFILE_H

#include <cstdint>
#include <vector>

#include "util.h"

#pragma pack (1)
struct SHPFILEHEADER
{
    int32_t file_code;
    int32_t unused0;
    int32_t unused1;
    int32_t unused2;
    int32_t unused3;
    int32_t unused4;
    int32_t file_length;
    int32_t ver;
    int32_t shape_type;
    double Xmin;
    double Ymin;
    double Xmax;
    double Ymax;
    double Zmin;
    double Zmax;
    double Mmin;
    double Mmax;
};

struct SHPRECORDHEADER
{
    int32_t record_number;
    int32_t content_length;
};

struct SHPCONTENTHEADER
{
    double Xmin;
    double Ymin;
    double Xmax;
    double Ymax;
    int32_t  NumParts;
    int32_t  NumPoints;
};

struct SHPPOINT
{
    double X, Y;
};

#pragma pack ()

std::vector<std::vector<SHPPOINT>> load_shapefile(const char * file_path);

#endif //ZJMAP_SHAPEFILE_H
