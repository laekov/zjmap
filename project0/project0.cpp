//
// Created by lvzijian on 2022/4/6.
//

#include <map-core.h>
#include "get-nearest-pois.h"
#include <random>


int main()
{
    enable_utf8_output();
    auto core = MapCore::get_instance();
    core->load_route(u8"data/北京路网/九级路", 2);
    core->load_route(u8"data/北京路网/高速", 4);
    core->load_route(u8"data/北京路网/行人道路", 0);
    core->load_route(u8"data/北京路网/城市快速路", 3);
    core->load_route(u8"data/北京路网/国道", 3);
    core->load_route(u8"data/北京路网/其它道路", 1);
    core->load_route(u8"data/北京路网/其它道路2", 1);
    core->load_route(u8"data/北京路网/省道", 3);
    core->load_route(u8"data/北京路网/县道", 2);
    core->load_route(u8"data/北京路网/乡镇村道", 2);
    core->load_route(u8"data/北京路网/乡镇村道2", 2);
    core->build_routes();
    core->load_poi(u8"data/北京POI/交通设施服务.csv");
    core->load_poi(u8"data/北京POI/住宿服务.csv");
    core->load_poi(u8"data/北京POI/体育休闲服务.csv");
    core->load_poi(u8"data/北京POI/公共设施.csv");
    core->load_poi(u8"data/北京POI/公司企业.csv");
    core->load_poi(u8"data/北京POI/医疗保健服务.csv");
    core->load_poi(u8"data/北京POI/商务住宅.csv");
    core->load_poi(u8"data/北京POI/政府机构及社会团体.csv");
    core->load_poi(u8"data/北京POI/生活服务.csv");
    core->load_poi(u8"data/北京POI/科教文化服务.csv");
    core->load_poi(u8"data/北京POI/购物.csv");
    core->load_poi(u8"data/北京POI/金融保险服务.csv");
    core->load_poi(u8"data/北京POI/风景名胜.csv");
    core->load_poi(u8"data/北京POI/餐饮.csv");
    core->build_pois();
    core->print_info();

    std::mt19937_64 rd(20010507);
    FILE * file = open_file("samples/proj0_test.in", "r");
    FILE * ansfile = open_file( "samples/proj0_test.ans", "r");
    std::vector<Coord> data_in;
    std::vector<Coord> data_ans;
    Coord coord;
    while (fscanf(file, "%lf%lf", &coord.x, &coord.y) == 2)
        data_in.push_back(coord);
    while (fscanf(ansfile, "%lf%lf", &coord.x, &coord.y) == 2)
        data_ans.push_back(coord);
    fclose(file);
    fclose(ansfile);

    auto true_distance = [](Coord coord1, Coord coord2) -> double{
        coord1.x *= acos(-1) / 180.0;
        coord1.y *= acos(-1) / 180.0;
        coord2.x *= acos(-1) / 180.0;
        coord2.y *= acos(-1) / 180.0;
        double x0 = cos(coord1.y) * cos(coord1.x), y0 = cos(coord1.y) * sin(coord1.x), z0 = sin(coord1.y);
        double x1 = cos(coord2.y) * cos(coord2.x), y1 = cos(coord2.y) * sin(coord2.x), z1 = sin(coord2.y);
        x1 -= x0;
        y1 -= y0;
        z1 -= z0;
        return sqrt(x1*x1 + y1*y1 + z1*z1);
    };

    for (int i = 0; i < data_in.size(); i++) {
        auto stu_ans = core->get_nearest_pois(data_in[i]);
        if (!stu_ans.size()){
            printf("FAILED\nYour function didn't give any result on case #%d.\n", i);
            return 0;
        }
        double distance_lim = true_distance(data_in[i], data_ans[i]);
        for (int j = 0; j < stu_ans.size(); j++)
        {
            if (true_distance(data_in[i], stu_ans[j].coord) > distance_lim * 1 + 1e-4)
            {
                printf("FAILED\nResult #%d of case #%d doesn't fit limit.\n", j, i);
                return 0;
            }

            auto poi_ids= core->get_poi_ids(core->find_nearest_mark(stu_ans[j].coord));
            bool pass = false;
            for (auto poi_id : poi_ids)
            {
                POI poi = core->get_poi(poi_id);
                if (poi.coord.x == stu_ans[j].coord.x && poi.coord.y == stu_ans[j].coord.y
                    && poi.name == stu_ans[j].name && poi.comment == stu_ans[j].comment)
                {
                    pass = true;
                    break;
                }
            }
            if (!pass)
            {
                printf("FAILED\nResult #%d of case #%d doesn't match any poi in database, where did you get it?\n", j, i);
                return 0;
            }
        }
    }

    puts("PASS");
    return 0;
}