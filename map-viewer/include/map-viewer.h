//
// Created by zijia on 2022/4/4.
//

#ifndef ZJMAP_MAP_VIEWER_H
#define ZJMAP_MAP_VIEWER_H

#include <map-core.h>
#include "glutil.h"
#include "Grassland/Math/Math.h"
#include "text-manager.h"
using namespace Grassland;

class MapViewer
{
public:
    MapViewer();
    static MapViewer* get_instance();
    void Run();

    static void ScrollFun(GLFWwindow * window, double x, double y);
    static void CurPosFun(GLFWwindow * window, double xpos, double ypos);
private:
#pragma pack(1)
    struct Vertex
    {
        GRLVec4 coord1;
        GRLVec4 coord2;
        GRLMat2 rotation;
        GRLVec4 color;
        float width;
        int level;
        int speedclass;
        int funcclass;
    };
#pragma pack()
    GRLVec2 focus_point;
    float zoom_scale;
    GRLMat4 mat_cam;
    GRLMat4 mat_screen;
    MapCore * core;
    float lat_scale;

    struct RoadLayer
    {
        uint32_t vertex_buffer;
        uint32_t index_buffer;
        int32_t num_index;
    }all_roads;
    RoadLayer layer_road5;
    RoadLayer layer_road4;
    RoadLayer layer_road3;
    RoadLayer layer_road2;
    RoadLayer layer_road1;
    RoadLayer layer_walk;

    TextManager text_manager;

    void PrepareAssets();
    RoadLayer BuildLayer(bool (*sel_func)(RouteInfo));
    static void DrawLayer(RoadLayer road_layer);
};

#endif //ZJMAP_MAP_VIEWER_H
