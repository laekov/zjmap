//
// Created by zijia on 2022/4/4.
//

#include "map-viewer.h"

int main()
{
    enable_utf8_output();

    MapCore *core = MapCore::get_instance();

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
    core->print_info();

    MapViewer::get_instance()->Run();
}