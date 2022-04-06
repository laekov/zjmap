//
// Created by zijia on 2022/4/1.
//

#include "shapefile.h"
#include "coordinate.h"

std::vector <std::vector<SHPPOINT>> load_shapefile(const char *file_path) {
    FILE * file = open_file(file_path, "rb");
    std::vector<std::vector<SHPPOINT>> result;
    if (!file) return result;

    SHPFILEHEADER shp_file_header;
    fread(&shp_file_header, sizeof(shp_file_header),  1, file);
    shp_file_header.file_code = reverse_bytes(shp_file_header.file_code);
    shp_file_header.file_length = reverse_bytes(shp_file_header.file_length);

#ifndef NDEBUG
    printf("File Path:       %s\n", file_path);
    printf("File Code:       %d\n", shp_file_header.file_code);
    printf("File Length:     %d\n", shp_file_header.file_length * 2);
    printf("Shape Type:      %d\n", shp_file_header.shape_type);
    printf("x range: [%lf, %lf]\n", shp_file_header.Xmin, shp_file_header.Xmax);
    printf("Y range: [%lf, %lf]\n", shp_file_header.Ymin, shp_file_header.Ymax);
    printf("Z range: [%lf, %lf]\n", shp_file_header.Zmin, shp_file_header.Zmax);
    printf("M range: [%lf, %lf]\n", shp_file_header.Mmin, shp_file_header.Mmax);
#endif

    int pnter = 50;

    int cnt_content = 0;

    while (pnter < shp_file_header.file_length) {
        SHPRECORDHEADER record_header;
        fread(&record_header, sizeof(record_header), 1, file);
        record_header.record_number = reverse_bytes(record_header.record_number);
        record_header.content_length = reverse_bytes(record_header.content_length);

        int32_t shape_type;
        fread(&shape_type, sizeof(shape_type), 1, file);

        std::vector<int32_t> parts;
        std::vector<SHPPOINT> content;

        if (shape_type == 3)
        {
            SHPCONTENTHEADER polyline_header;
            fread(&polyline_header, sizeof(polyline_header), 1, file);
            int part;
            SHPPOINT point;
            for (int i = 0; i < polyline_header.NumParts; i++)
                fread(&part, sizeof(part), 1, file);
            for (int i = 0; i < polyline_header.NumPoints; i++) {
                fread(&point, sizeof(point), 1, file);
                content.push_back(point);
            }
            result.push_back(content);
        }
        else if (shape_type == 5)
        {
            SHPCONTENTHEADER polygon_header;
            fread(&polygon_header, sizeof(polygon_header), 1, file);
            int part;
            SHPPOINT point;
            for (int i = 0; i < polygon_header.NumParts; i++) {
                fread(&part, sizeof(part), 1, file);
                parts.push_back(part);
            }
            for (int i = 0; i < polygon_header.NumPoints; i++) {
                fread(&point, sizeof(point), 1, file);
                content.push_back(point);
            }
            result.push_back(content);
        }
        else
        {
            fseek(file, record_header.content_length * 2 - 4, SEEK_CUR);
        }

//#ifndef NDEBUG
//        if (cnt_content < 5) {
//            printf("[%d:%d] - %d\n", record_header.record_number, record_header.content_length, shape_type);
//            for (int i = 0; i < content.size(); i++) {
//                bool sep = 0;
//                for (auto j : parts)
//                    if (i == j) {
//                        sep = 1;
//                        break;
//                    }
//                if (sep)
//                    puts("--------------");
//                auto pnt = content[i];
//                printf("(%lf, %lf)\n", pnt.x, pnt.Y);
//            }
//            cnt_content ++;
//        }
//        else if (cnt_content == 5)
//            printf("...\n"), cnt_content++;
//#endif
        pnter += 4 + record_header.content_length;
    }


    fclose(file);
    return result;

}



static const double pi = 3.14159265358979323846264338327950288419716939937510;
static const double a = 6378245.0;
static const double ee = 0.00669342162296594323;
static const double bd_pi = 3.14159265358979324 * 3000.0 / 180.0;

static double transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y
                 + 0.2 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    return ret;
}

static double transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    return ret;
}


SHPPOINT WGS84_to_GCJ02(SHPPOINT coord) {
    double dLat = transformLat(coord.x - 105.0, coord.y - 35.0);
    double dLon = transformLon(coord.x - 105.0, coord.y - 35.0);
    double radLat = coord.y / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    double mgLat = coord.y + dLat;
    double mgLon = coord.x + dLon;
    return SHPPOINT{mgLon, mgLat};
}

SHPPOINT GCJ02_to_WGS84(SHPPOINT coord) {
    SHPPOINT d = WGS84_to_GCJ02(coord);
    return SHPPOINT{coord.x * 2 - d.x, coord.y * 2 - d.y};
}

SHPPOINT GCJ02_to_BD09(SHPPOINT coord) {
    double x = coord.x, y = coord.y;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * bd_pi);
    double theta = atan2(y, x) + 0.000003 * cos(x * bd_pi);
    double bd_lon = z * cos(theta) + 0.0065;
    double bd_lat = z * sin(theta) + 0.006;
    return SHPPOINT{bd_lon, bd_lat};
}

SHPPOINT BD09_to_GCJ02(SHPPOINT coord) {
    double x = coord.x - 0.0065, y = coord.y - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * bd_pi);
    double theta = atan2(y, x) - 0.000003 * cos(x * bd_pi);
    double gg_lon = z * cos(theta);
    double gg_lat = z * sin(theta);
    return SHPPOINT{gg_lon, gg_lat};
}

