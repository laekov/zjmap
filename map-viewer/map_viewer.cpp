//
// Created by zijia on 2022/4/4.
//

#include <map-core.h>
#include "Grassland/Math/Math.h"
using namespace Grassland;

using namespace std;

MapCore core;

int main()
{
    enable_utf8_output();

    core.load_route(u8"data/北京路网/九级路");
    core.load_route(u8"data/北京路网/高速");
    core.load_route(u8"data/北京路网/行人道路");
    core.load_route(u8"data/北京路网/城市快速路");
    core.load_route(u8"data/北京路网/国道");
    core.load_route(u8"data/北京路网/其它道路");
    core.load_route(u8"data/北京路网/其它道路2");
    core.load_route(u8"data/北京路网/省道");
    core.load_route(u8"data/北京路网/县道");
    core.load_route(u8"data/北京路网/乡镇村道");
    core.load_route(u8"data/北京路网/乡镇村道2");
    core.build_routes();
    core.print_info();
}