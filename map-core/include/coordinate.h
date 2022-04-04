//
// Created by zijia on 2022/4/4.
//

#ifndef ZJMAP_COORDINATE_H
#define ZJMAP_COORDINATE_H

#include "shapefile.h"
#include <map>
#include <algorithm>

struct Coord
{
    double x;
    double y;
};

struct BoundingBox
{
    double x0;
    double y0;
    double x1;
    double y1;
    BoundingBox();
    BoundingBox(Coord p);
    BoundingBox(double _x0, double _y0, double _x1, double _y1);
    friend BoundingBox merge(const BoundingBox b1, const BoundingBox b2);
    friend BoundingBox intersection(const BoundingBox b1, const BoundingBox b2);
    bool is_empty();
    bool is_inside(Coord p);
    BoundingBox expand(double x, double y);
};

Coord WGS84_to_GCJ02(Coord coord);

Coord GCJ02_to_WGS84(Coord coord);

Coord GCJ02_to_BD09(Coord coord);

Coord BD09_to_GCJ02(Coord coord);

double hamilton_dist(Coord p1, Coord p2);

class CoordManager
{
public:
    CoordManager(double _approx = 0.0);
    Coord operator [] (const int index) const;
    int find(Coord p);
    int find_nearest(Coord p);
    int insert(Coord p);
    void build(std::vector<Coord> pos, double approx_ = 0.0);
    void clear();
    int size() const;
    BoundingBox get_boundaries();
private:
    struct tree_node
    {
        Coord p;
        BoundingBox bbox;
        int ch[4];
    };
    int root;
    std::vector<tree_node> bvh;
    double approx;

    void find(int x, Coord p, int &ret, double &dist);

    void insert(int &x, int y);
    void maintain(int x);

    void build(int &x, std::vector<Coord> pos, int cut = 0);
};

#endif //ZJMAP_COORDINATE_H
