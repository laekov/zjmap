//
// Created by zijia on 2022/4/4.
//

#ifndef ZJMAP_MAP_CORE_H
#define ZJMAP_MAP_CORE_H

#include "csv-table.h"
#include "shapefile.h"
#include "string-convert.h"
#include "util.h"
#include "coordinate.h"
#include <chrono>

struct RouteInfo
{
    std::string name;
    int width;
    int funcclass;
    int speedclass;
    int level;
};

struct Edge
{
    int start;
    int end;
    int route_id;
};

struct POI // Point Of Interest
{
    Coord coord;
    std::string name;
    std::string comment;
};

class MapCore
{
public:
    MapCore();
    void load_route(const char *path, int level);
    void build_routes();
    void load_poi(const char * path);
    void build_pois();
    RouteInfo get_route_info(int route_id);
    Coord get_junction(int junc_id);
    Coord get_mark(int mark_id);
    POI get_poi(int poi_id);
    Edge get_edge(int edge_id);
    std::vector<int> get_poi_ids(int mark_id);
    std::vector<int> get_in_edges_id(int junc_id);
    std::vector<int> get_out_edges_id(int junc_id);
    int route_count();
    int junction_count();
    int edge_count();
    int mark_count();
    BoundingBox get_boundaries();
    int find_nearest_junc(Coord p);
    int find_nearest_mark(Coord p);
    void print_info();
    static MapCore * get_instance();
    std::vector<POI> get_nearest_pois(Coord coord);
private:
    std::vector<std::vector<SHPPOINT>> routes;
    std::vector<RouteInfo> routes_info;
    CoordManager junctions;
    std::vector<Edge> edges;
    std::vector<std::vector<int>> out_edges;
    std::vector<std::vector<int>> in_edges;

    CoordManager marks;
    std::vector<std::vector<int>> poi_ids;
    std::vector<POI> pois;
    std::chrono::steady_clock::time_point start_loading_time;
    std::chrono::steady_clock::time_point end_build_time;
    void insert_edge(Coord start, Coord end, int route_id);
};

#endif //ZJMAP_MAP_CORE_H
