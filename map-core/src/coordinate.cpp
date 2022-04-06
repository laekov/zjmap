//
// Created by zijia on 2022/4/4.
//

#include "coordinate.h"

static const double pi = 3.14159265358979323846264338327950288419716939937510;
static const double a = 6378245.0;
static const double ee = 0.00669342162296594323;
static const double bd_pi = 3.14159265358979324 * 3000.0 / 180.0;

static double transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y
                 + 0.2 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    return ret;
}

static double transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    return ret;
}


Coord WGS84_to_GCJ02(Coord coord) {
    double dLat = transformLat(coord.x - 105.0, coord.y - 35.0);
    double dLon = transformLon(coord.x - 105.0, coord.y - 35.0);
    double radLat = coord.y / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    double mgLat = coord.y + dLat;
    double mgLon = coord.x + dLon;
    return Coord{mgLon, mgLat};
}

Coord GCJ02_to_WGS84(Coord coord) {
    Coord d = WGS84_to_GCJ02(coord);
    return Coord{coord.x * 2 - d.x, coord.y * 2 - d.y};
}

Coord GCJ02_to_BD09(Coord coord) {
    double x = coord.x, y = coord.y;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * bd_pi);
    double theta = atan2(y, x) + 0.000003 * cos(x * bd_pi);
    double bd_lon = z * cos(theta) + 0.0065;
    double bd_lat = z * sin(theta) + 0.006;
    return Coord{bd_lon, bd_lat};
}

Coord BD09_to_GCJ02(Coord coord) {
    double x = coord.x - 0.0065, y = coord.y - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * bd_pi);
    double theta = atan2(y, x) - 0.000003 * cos(x * bd_pi);
    double gg_lon = z * cos(theta);
    double gg_lat = z * sin(theta);
    return Coord{gg_lon, gg_lat};
}

BoundingBox::BoundingBox() {
    x0 = 1e100;
    y0 = 1e100;
    x1 = -1e100;
    y1 = -1e100;
}

BoundingBox::BoundingBox(Coord p) {
    x1 = x0 = p.x;
    y1 = y0 = p.y;
}

BoundingBox merge(const BoundingBox b1, const BoundingBox b2) {
    BoundingBox bbox;
    bbox.x0 = std::min(b1.x0, b2.x0);
    bbox.y0 = std::min(b1.y0, b2.y0);
    bbox.x1 = std::max(b1.x1, b2.x1);
    bbox.y1 = std::max(b1.y1, b2.y1);
    return bbox;
}

BoundingBox intersection(const BoundingBox b1, const BoundingBox b2) {
    BoundingBox bbox;
    bbox.x0 = std::max(b1.x0, b2.x0);
    bbox.y0 = std::max(b1.y0, b2.y0);
    bbox.x1 = std::min(b1.x1, b2.x1);
    bbox.y1 = std::min(b1.y1, b2.y1);
    return bbox;
}

bool BoundingBox::is_empty() {
    return x0 > x1 || y0 > y1;
}

BoundingBox BoundingBox::expand(double x, double y) {
    BoundingBox bbox;
    bbox.x0 = x0 - x;
    bbox.y0 = y0 - y;
    bbox.x1 = x1 + x;
    bbox.y1 = y1 + y;
    return bbox;
}

BoundingBox::BoundingBox(double _x0, double _y0, double _x1, double _y1) {
    x0 = _x0;
    y0 = _y0;
    x1 = _x1;
    y1 = _y1;
}

bool BoundingBox::is_inside(Coord p) {
    return x0 <= p.x && p.x <= x1 && y0 <= p.y && p.y <= y1;
}

void CoordManager::clear() {
    bvh.clear();
    root = -1;
}

CoordManager::CoordManager(double _approx) {
    approx = _approx;
    root = -1;
}

Coord CoordManager::operator[](const int index) const {
    return bvh[index].p;
}

int CoordManager::find(Coord p) {
    int ret = -1;
    double dist = approx;
    find(root, p, ret, dist);
    return ret;
}

double hamilton_dist(Coord p1, Coord p2)
{
    return std::max(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));
}

static int relative_pos(Coord o, Coord p)
{
    int ret = 0;
    if (o.x < p.x) ret |= 1;
    if (o.y < p.y) ret |= 2;
    return ret;
}

void CoordManager::find(int x, Coord p, int & ret, double &dist) {
    if (x == -1) return;
    if (!bvh[x].bbox.expand(dist, dist).is_inside(p)) return;
    double dt = hamilton_dist(bvh[x].p, p);
    if (dt < dist || (dt == dist && ret == -1))
    {
        dist = dt;
        ret = x;
    }
    int ich = relative_pos(bvh[x].p, p);
    find(bvh[x].ch[ich], p, ret, dist);
    find(bvh[x].ch[ich ^ 1], p, ret, dist);
    find(bvh[x].ch[ich ^ 2], p, ret, dist);
    find(bvh[x].ch[ich ^ 3], p, ret, dist);
}

int CoordManager::insert(Coord p) {
    int ret = find(p);
    if (ret != -1) return ret;
    bvh.push_back(tree_node{p, BoundingBox(p), -1, -1, -1, -1});
    ret = bvh.size() - 1;
    insert(root, ret);
    return ret;
}

void CoordManager::insert(int &x, int y) {
    if (x == -1) x = y;
    else
    {
        int d = relative_pos(bvh[x].p, bvh[y].p);
        insert(bvh[x].ch[d], y);
        maintain(x);
    }
}

void CoordManager::maintain(int x) {
    bvh[x].bbox = BoundingBox(bvh[x].p);
    for (int  i = 0; i < 4; i++)
        if (bvh[x].ch[i] != -1)
            bvh[x].bbox = merge(bvh[x].bbox, bvh[bvh[x].ch[i]].bbox);
}

void CoordManager::build(std::vector<Coord> &pos, double approx_) {
    approx = approx_;
    root = -1;
    bvh.clear();
    bvh.reserve(pos.size());
    build(root, pos);
}

void CoordManager::build(int &x, std::vector<Coord> &pos, int cut) {
    if (!pos.size()) return;
    if (cut)
        std::sort(pos.begin(), pos.end(), [](Coord p1, Coord p2) -> bool {return p1.x < p2.x;});
    else
        std::sort(pos.begin(), pos.end(), [](Coord p1, Coord p2) -> bool {return p1.y < p2.y;});
    int m = pos.size() / 2; auto mp = pos[m];
    bvh.push_back(tree_node{mp, BoundingBox(mp), -1, -1, -1, -1});
    x = bvh.size() - 1;
    std::vector<Coord> subtrees[4];

    uint32_t cnt_subtree[4] = {};

//    for (Coord p: pos)
//    {
//        if (hamilton_dist(p, pos[m]) > approx)
//            cnt_subtree[relative_pos(pos[m], p)]++;
//    }
//    for (int i = 0; i < 4; i++)
//        subtrees[i].reserve(cnt_subtree[i]);

    for (Coord p: pos)
    {
        if (hamilton_dist(p, mp) > approx)
            subtrees[relative_pos(mp, p)].push_back(p);
    }
    build(bvh[x].ch[0], subtrees[0], 1 - cut);
    build(bvh[x].ch[1], subtrees[1], 1 - cut);
    build(bvh[x].ch[2], subtrees[2], 1 - cut);
    build(bvh[x].ch[3], subtrees[3], 1 - cut);
    maintain(x);
}

int CoordManager::size() const {
    return bvh.size();
}

BoundingBox CoordManager::get_boundaries() {
    if (root != -1)
        return bvh[root].bbox;
    else
        return BoundingBox();
}

int CoordManager::find_nearest(Coord p) {
    int ret = -1;
    double dist = INFINITY;
    find(root, p, ret, dist);
    return ret;
}
