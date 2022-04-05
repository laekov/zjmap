//
// Created by zijia on 2022/4/4.
//

#ifndef ZJMAP_MAP_VIEWER_H
#define ZJMAP_MAP_VIEWER_H

#include <map-core.h>
#include "glutil.h"
#include "Grassland/Math/Math.h"
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
        GRLVec4 coord;
        GRLVec4 color;
        GRLVec4 expand;
        float width;
        int level;
        int speedclass;
        int funcclass;
    };
#pragma pack()
    GRLVec2 focus_point;
    float zoom_scale;
    GRLMat4 mat_cam;
    MapCore * core;
    uint32_t vertex_buffer;
    uint32_t index_buffer;
    float lat_scale;

    struct GraphLayer
    {
        uint32_t vertex_buffer;
        uint32_t index_buffer;
        int32_t num_index;
    };

    void PrepareAssets();
    void BuildLevel(bool (*sel_func)(RouteInfo));
};

#endif //ZJMAP_MAP_VIEWER_H
