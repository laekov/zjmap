//
// Created by zijia on 2022/4/1.
//


#include <iostream>
#include <map-viewer.h>
#include <cstdint>
#include "csv-table.h"
#include "shapefile.h"



using namespace std;

//template<class T>
//void print_bytes(const T * buff)
//{
//    const T * pnt = buff;
//    while (*pnt)
//    {
//        const unsigned char * byte_pnter = (const unsigned char *) pnt;
//        for (int i = 0; i < sizeof(T); i++)
//            printf("%02x", (int)byte_pnter[i]);
//        pnt++;
//    }
//    puts("");
//}

int main()
{
    load_shapefile("data/北京路网/九级路.shp");
    load_shapefile("data/北京建筑轮廓/北京市建筑物轮廓及高度.shp");
}
