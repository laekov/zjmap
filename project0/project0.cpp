//
// Created by lvzijian on 2022/4/6.
//

#include <map-core.h>

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
}