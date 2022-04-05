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
            pos.push_back(Coord{shppnt.X, shppnt.Y});
    junctions.build(pos, 0);

    in_edges.reserve(junctions.size());
    out_edges.reserve(junctions.size());

    for (int i = 0; i < routes.size(); i++)
    {
        auto & route = routes[i];
        for (int j = 0; j < route.size() - 1; j++)
            insert_edge(Coord{route[j].X, route[j].Y}, Coord{route[j + 1].X, route[j + 1].Y}, i);
    }
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
}

MapCore *MapCore::get_instance() {
    static MapCore * core = nullptr;
    if (!core) core = new MapCore;
    return core;
}

