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
    printf("X range: [%lf, %lf]\n", shp_file_header.Xmin, shp_file_header.Xmax);
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
//                printf("(%lf, %lf)\n", pnt.X, pnt.Y);
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


