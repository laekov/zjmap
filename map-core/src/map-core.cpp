//
// Created by zijia on 2022/4/4.
//

#include "map-core.h"

void MapCore::load_route(const char *path, int level) {
    std::string str_path = path;
    std::string csv_file_path = str_path + ".csv";
    std::string shp_file_path = str_path + ".shp";
    auto file_routes = load_shapefile(shp_file_path.c_str());
    auto file_routes_info = load_csv_table(csv_file_path.c_str());

    auto & name_list = file_routes_info["NAME"];
    auto & width_list = file_routes_info["WIDTH"];
    auto & direction_list = file_routes_info["DIRECTION"];
    auto & funcclass_list = file_routes_info["FUNCCLASS"];
    auto & speedclass_list = file_routes_info["SPEEDCLASS"];

    for (int i = 0; i < file_routes.size(); i++)
    {
        RouteInfo rinfo;
        rinfo.name = name_list[i];
        rinfo.width = string_to_integer(width_list[i]);
        rinfo.speedclass = string_to_integer(speedclass_list[i]);
        rinfo.funcclass = string_to_integer(funcclass_list[i]);
        rinfo.level = level;
        int direction = string_to_integer(direction_list[i]);
        for (auto &p : file_routes[i])
            p = WGS84_to_GCJ02(p);

        if (direction == 1 || direction == 2){
            routes.push_back(file_routes[i]);
            routes_info.push_back(rinfo);
        }
        if (direction == 1 || direction == 3){
            std::reverse(file_routes[i].begin(), file_routes[i].end());
            routes.push_back(file_routes[i]);
            routes_info.push_back(rinfo);
        }
    }
}

void MapCore::build_routes() {
    std::vector<Coord> pos;
    for (auto & route: routes)
        for (auto & shppnt : route)
            pos.push_back(Coord{shppnt.x, shppnt.y});
    junctions.build(pos, 0);

    in_edges.resize(junctions.size());
    out_edges.resize(junctions.size());

    for (int i = 0; i < routes.size(); i++)
    {
        auto & route = routes[i];
        for (int j = 0; j < route.size() - 1; j++)
            insert_edge(Coord{route[j].x, route[j].y}, Coord{route[j + 1].x, route[j + 1].y}, i);
    }
    end_build_time = std::chrono::steady_clock::now();
}

RouteInfo MapCore::get_route_info(int route_id) {
    return routes_info[route_id];
}

Coord MapCore::get_junction(int junc_id) {
    return junctions[junc_id];
}

Edge MapCore::get_edge(int edge_id) {
    return edges[edge_id];
}

std::vector<int> MapCore::get_in_edges_id(int junc_id) {
    return in_edges[junc_id];
}

std::vector<int> MapCore::get_out_edges_id(int junc_id) {
    return out_edges[junc_id];
}

void MapCore::insert_edge(Coord start, Coord end, int route_id) {
    Edge edge = { junctions.find(start), junctions.find(end), route_id };
    int edge_id = edges.size();
    edges.push_back(edge);
    out_edges[edge.start].push_back(edge_id);
    in_edges[edge.end].push_back(edge_id);
}

int MapCore::route_count() {
    return routes_info.size();
}

int MapCore::junction_count() {
    return junctions.size();
}

int MapCore::edge_count() {
    return edges.size();
}

BoundingBox MapCore::get_boundaries() {
    return junctions.get_boundaries();
}

int MapCore::find_nearest_junc(Coord p) {
    return junctions.find_nearest(p);
}

void MapCore::print_info() {
    printf("Junction Count:            %d\n", junctions.size());
    printf("Route Count:               %d\n", (int)routes.size());
    printf("Edge Count:                %d\n", (int)edges.size());
    auto junc_bbox = junctions.get_boundaries();
    printf("Junction XRange:           [%lf, %lf]\n", junc_bbox.x0, junc_bbox.x1);
    printf("Junction YRange:           [%lf, %lf]\n", junc_bbox.y0, junc_bbox.y1);
    auto mark_bbox = marks.get_boundaries();
    printf("POI Count:                 %d\n", (int)pois.size());
    printf("Mark Count:                %d\n", (int)marks.size());
    printf("Mark point XRange:         [%lf, %lf]\n", mark_bbox.x0, mark_bbox.x1);
    printf("Mark point YRange:         [%lf, %lf]\n", mark_bbox.y0, mark_bbox.y1);
    printf("Load&Build time:           %Lfs\n", ((end_build_time - start_loading_time) / std::chrono::milliseconds(1)) * 0.001L);
}

MapCore *MapCore::get_instance() {
    static MapCore * core = nullptr;
    if (!core) core = new MapCore;
    return core;
}

void MapCore::load_poi(const char *path) {
    auto table = load_csv_table(path);
    auto& names = table[u8"名称"];
    auto& xcoords = table[u8"WGS84_经度"];
    auto& ycoords = table[u8"WGS84_纬度"];

    if (table.count(u8"地址"))
    {
        auto& addr = table[u8"地址"];
        for (int i = 0; i < names.size(); i++)
            if (names[i].size())
                pois.push_back(POI{WGS84_to_GCJ02(Coord{string_to_real(xcoords[i]), string_to_real(ycoords[i])}), names[i], addr[i]});
    }
    else
        for (int i = 0; i < names.size(); i++)
            if (names[i].size())
                pois.push_back(POI{WGS84_to_GCJ02(Coord{string_to_real(xcoords[i]), string_to_real(ycoords[i])}), names[i], ""});
}

void MapCore::build_pois() {
    std::vector<Coord> mark_coords;
    for (auto & poi : pois)
        mark_coords.push_back(poi.coord);
    marks.build(mark_coords, 0.0);
    poi_ids.resize(marks.size());
    for (int i = 0; i < pois.size(); i++)
    {
        int mark_id = marks.find(pois[i].coord);
//        if (pois[i].coord.x == 0.0 && pois[i].coord.y == 0.0)printf("%d %d (%lf, %lf) %s-%s\n", mark_id, i, pois[i].coord.x, pois[i].coord.y, pois[i].name.c_str(), pois[i].comment.c_str());
        if (hamilton_dist(marks[mark_id], pois[i].coord) > 0.0)
            printf("%d %d (%lf, %lf) (%lf, %lf) %s-%s\n", mark_id, i, marks[mark_id].x, marks[mark_id].y, pois[i].coord.x, pois[i].coord.y, pois[i].name.c_str(), pois[i].comment.c_str());
        poi_ids[mark_id].push_back(i);
    }
    end_build_time = std::chrono::steady_clock::now();
}

int MapCore::find_nearest_mark(Coord p) {
    return marks.find_nearest(p);
}

int MapCore::mark_count() {
    return marks.size();
}

Coord MapCore::get_mark(int mark_id) {
    return marks[mark_id];
}

POI MapCore::get_poi(int poi_id) {
    return pois[poi_id];
}

std::vector<int> MapCore::get_poi_ids(int mark_id) {
    return poi_ids[mark_id];
}

MapCore::MapCore() {
    start_loading_time = std::chrono::steady_clock::now();
}

