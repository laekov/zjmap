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

class MapCore
{
public:
    void load_route(const char *path, int level);
    void build_routes();
    RouteInfo get_route_info(int route_id);
    Coord get_junction(int junc_id);
    Edge get_edge(int edge_id);
    std::vector<int> get_in_edges_id(int junc_id);
    std::vector<int> get_out_edges_id(int junc_id);
    int route_count();
    int junction_count();
    int edge_count();
    BoundingBox get_boundaries();
    int find_nearest_junc(Coord p);
    void print_info();
    static MapCore * get_instance();
private:
    std::vector<std::vector<SHPPOINT>> routes;
    std::vector<RouteInfo> routes_info;
    CoordManager junctions;
    std::vector<Edge> edges;
    std::vector<std::vector<int>> out_edges;
    std::vector<std::vector<int>> in_edges;

    void insert_edge(Coord start, Coord end, int route_id);
};

#endif //ZJMAP_MAP_CORE_H
