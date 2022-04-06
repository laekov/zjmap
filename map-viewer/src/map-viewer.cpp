//
// Created by zijia on 2022/4/4.
//

#include "map-viewer.h"

MapViewer *MapViewer::get_instance() {
    static MapViewer * instance = nullptr;
    if (!instance) instance = new MapViewer;
    return instance;
}

void MapViewer::Run() {
    GLFWwindow * window;
    GLInit(1280, 720, &window);
    glfwSwapInterval(1);

    BoundingBox bbox = core->get_boundaries();
    focus_point = GRLVec2 ();//GRLVec2 ((bbox.x0 + bbox.x1) * 0.5, (bbox.y0 + bbox.y1) * 0.5);
    zoom_scale = 0.1; //
    lat_scale = std::cos(Math::radian((bbox.y0 + bbox.y1) * 0.5));
    uint32_t fragment_shader = CompileShaderFromFile(u8"shader/frag.glsl", GL_FRAGMENT_SHADER);
    uint32_t vertex_shader = CompileShaderFromFile(u8"shader/vert.glsl", GL_VERTEX_SHADER);
    uint32_t program = LinkProgram(vertex_shader, fragment_shader);
    glcall(glDeleteShader, vertex_shader);
    glcall(glDeleteShader, fragment_shader);
    int32_t uniform_coord_trans = glGetUniformLocation(program, "coord_trans");
    int32_t uniform_zoom_scale = glGetUniformLocation(program, "zoom_scale");
    int32_t uniform_visible_level = glGetUniformLocation(program, "visible_level");
    int32_t uniform_screen_trans = glGetUniformLocation(program, "screen_trans");
    float visible_level;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    PrepareAssets();

    text_manager.Init("fonts/NotoSansSC-Regular.otf", nullptr, 32,0);

    while (!ShouldClose())
    {
        int32_t wnd_width, wnd_height;
        glfwGetWindowSize(window, &wnd_width, &wnd_height);
        ClearScreen(0.6, 0.7, 0.8, 1.0);
        mat_cam = GRLMat4(
                lat_scale * (100.0 / wnd_width) / zoom_scale, 0.0, 0.0, 0.0,
                0.0, (100.0 / wnd_height) / zoom_scale, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0
        )* GRLMat4(
                1.0, 0.0, 0.0, -focus_point[0],
                0.0, 1.0, 0.0, -focus_point[1],
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0
        );
        mat_screen = GRLMat4 (
                1.0 / (float)wnd_width, 0.0, 0.0, 0.0,
                0.0, 1.0 / (float)wnd_height, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0
                ) * 0.1;
        glcall(glProgramUniformMatrix4fv, program, uniform_coord_trans, 1, false, (float*)&mat_cam);
        glcall(glProgramUniformMatrix4fv, program, uniform_screen_trans, 1, false, (float*)&mat_screen);
        glcall(glProgramUniform1f, program, uniform_zoom_scale, std::max(zoom_scale, 0.001f) / zoom_scale);
        glcall(glUseProgram, program);

        static float present_visible[10] = {};
        float target_visible[10] = {};

        if (zoom_scale <= 0.001) target_visible[6] = 1.0;
        if (zoom_scale <= 0.002) target_visible[5] = 1.0;
        if (zoom_scale <= 0.007) target_visible[4] = 1.0;
        if (zoom_scale <= 0.02) target_visible[3] = 1.0;
        if (zoom_scale <= 0.07) target_visible[2] = 1.0;
        target_visible[1] = 1.0;

        for (int i = 0; i < 10; i++)
        {
            if (target_visible[i] > present_visible[i]) present_visible[i] += 1.0 / 64.0;
            if (target_visible[i] < present_visible[i]) present_visible[i] -= 1.0 / 64.0;
        }

        glcall(glProgramUniform1f, program, uniform_visible_level, present_visible[6]); if (present_visible[6]) DrawLayer(layer_walk);
        glcall(glProgramUniform1f, program, uniform_visible_level, present_visible[5]); if (present_visible[5]) DrawLayer(layer_road5);
        glcall(glProgramUniform1f, program, uniform_visible_level, present_visible[4]); if (present_visible[4]) DrawLayer(layer_road4);
        glcall(glProgramUniform1f, program, uniform_visible_level, present_visible[3]); if (present_visible[3]) DrawLayer(layer_road3);
        glcall(glProgramUniform1f, program, uniform_visible_level, present_visible[2]); if (present_visible[2]) DrawLayer(layer_road2);
        glcall(glProgramUniform1f, program, uniform_visible_level, present_visible[1]); if (present_visible[1]) DrawLayer(layer_road1);

        text_manager.DrawText(0, 32, L"我爱你，中国 Hello, World!");
        FlushScreen();
    }
}

MapViewer::MapViewer() {
    core = MapCore::get_instance();
    zoom_scale = 1.0;
}

void MapViewer::PrepareAssets() {


    BoundingBox bbox = core->get_boundaries();
    GRLVec2d center((bbox.x0 + bbox.x1) * 0.5, (bbox.y0 + bbox.y1) * 0.5);


    layer_road1 = BuildLayer([](RouteInfo route_info)->bool { return route_info.funcclass == 1 && route_info.level;});
    layer_road2 = BuildLayer([](RouteInfo route_info)->bool { return route_info.funcclass == 2 && route_info.level;});
    layer_road3 = BuildLayer([](RouteInfo route_info)->bool { return route_info.funcclass == 3 && route_info.level;});
    layer_road4 = BuildLayer([](RouteInfo route_info)->bool { return route_info.funcclass == 4 && route_info.level;});
    layer_road5 = BuildLayer([](RouteInfo route_info)->bool { return route_info.funcclass == 5 && route_info.level;});
    layer_walk = BuildLayer([](RouteInfo route_info)->bool { return !route_info.level;});


    glfwSetScrollCallback(GetWindowHandle(),
                          ScrollFun);
    glfwSetCursorPosCallback(GetWindowHandle(), CurPosFun);
}

void MapViewer::ScrollFun(GLFWwindow *window, double x, double y) {
    MapViewer::get_instance()->zoom_scale *= std::pow(0.5, y*0.1);
    std::cout << "zoom_scale" << MapViewer::get_instance()->zoom_scale << std::endl;
}

void MapViewer::CurPosFun(GLFWwindow *window, double xpos, double ypos) {
    static double lastx = 0.0, lasty = 0.0;
    if (glfwGetMouseButton(GetWindowHandle(), GLFW_MOUSE_BUTTON_LEFT))
    {
        MapViewer::get_instance()->focus_point = MapViewer::get_instance()->focus_point + GRLVec2((lastx - xpos) / MapViewer::get_instance()->lat_scale, ypos - lasty) * (MapViewer::get_instance()->zoom_scale * 0.02);
    }
    lastx = xpos;
    lasty = ypos;
}

MapViewer::RoadLayer MapViewer::BuildLayer(bool (*sel_func)(RouteInfo)) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    BoundingBox bbox = core->get_boundaries();
    GRLVec2d center((bbox.x0 + bbox.x1) * 0.5, (bbox.y0 + bbox.y1) * 0.5);

    for (int i = 0; i < core->edge_count(); i++)
    {
        Edge edge = core->get_edge(i);
        RouteInfo route_info = core->get_route_info(edge.route_id);
        if (!sel_func(route_info)) continue;
        Coord j1 = core->get_junction(edge.start), j2 = core->get_junction(edge.end);

        j1.x -= center[0]; j1.y -= center[1];
        j2.x -= center[0]; j2.y -= center[1];
        GRLVec4 p1(j1.x, j1.y, 0.0, 1.0), p2(j2.x, j2.y, 0.0, 1.0), direct;
        float width = 30;

        GRLMat2 rotate90(0.0, -1.0,
                         1.0, 0.0);


        GRLVec4 color(1.0, 1.0, 1.0, 1.0);

        switch (route_info.level) {
            case 0: color = GRLVec4 (0.8, 0.7, 0.6, 1.0); width = 30 ;break;
            case 1: color = GRLVec4 (0.8, 0.8, 0.8, 1.0); width = 30 ;break;
            case 2: color = GRLVec4 (1.0, 1.0, 1.0, 1.0); width = 30 ;break;
            case 3: color = GRLVec4 (1.0, 1.0, 0.9, 1.0); width = 30 ;break;
            case 4: color = GRLVec4 (0.6, 1.0, 0.6, 1.0); width = 55 ;break;
        }

        width = 60;
        switch (route_info.speedclass) {
            case 8: width = 20; break;
            case 7: width = 25; break;
            case 6: width = 30; break;
            case 5: width = 35; break;
            case 4: width = 40; break;
            case 3: width = 45; break;
            case 2: width = 50; break;
            case 1: width = 55; break;
        }

        int vertex_base = vertices.size();
        vertices.push_back(Vertex{p2, p1, GRLMat2(0.0) - rotate90, color, width, route_info.level, route_info.speedclass, route_info.funcclass});
        vertices.push_back(Vertex{p2, p1, GRLMat2(1.0)           , color, width, route_info.level, route_info.speedclass, route_info.funcclass});
        vertices.push_back(Vertex{p2, p1, GRLMat2(0.0) + rotate90, color, width, route_info.level, route_info.speedclass, route_info.funcclass});
        vertices.push_back(Vertex{p1, p2, GRLMat2(0.0) - rotate90, color, width, route_info.level, route_info.speedclass, route_info.funcclass});
        vertices.push_back(Vertex{p1, p2, GRLMat2(1.0)           , color, width, route_info.level, route_info.speedclass, route_info.funcclass});
        vertices.push_back(Vertex{p1, p2, GRLMat2(0.0) + rotate90, color, width, route_info.level, route_info.speedclass, route_info.funcclass});
        indices.push_back(vertex_base + 0);
        indices.push_back(vertex_base + 1);
        indices.push_back(vertex_base + 2);
        indices.push_back(vertex_base + 0);
        indices.push_back(vertex_base + 3);
        indices.push_back(vertex_base + 2);
        indices.push_back(vertex_base + 3);
        indices.push_back(vertex_base + 4);
        indices.push_back(vertex_base + 5);
        indices.push_back(vertex_base + 3);
        indices.push_back(vertex_base + 5);
        indices.push_back(vertex_base + 0);
    }

    RoadLayer road_layer;

    glcall(glCreateBuffers, 1, &road_layer.vertex_buffer);
    glcall(glCreateBuffers, 1, &road_layer.index_buffer);
    glcall(glNamedBufferData, road_layer.vertex_buffer, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glcall(glNamedBufferData, road_layer.index_buffer, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
    road_layer.num_index = indices.size();

    return road_layer;
}

void MapViewer::DrawLayer(MapViewer::RoadLayer road_layer) {
    glcall(glBindBuffer, GL_ARRAY_BUFFER, road_layer.vertex_buffer);
    glcall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, road_layer.index_buffer);
    glcall(glVertexAttribPointer, 0, 4, GL_FLOAT, false, sizeof(Vertex), (void*)0);
    glcall(glVertexAttribPointer, 1, 4, GL_FLOAT, false, sizeof(Vertex), (void*)16);
    glcall(glVertexAttribPointer, 2, 4, GL_FLOAT, false, sizeof(Vertex), (void*)32);
    glcall(glVertexAttribPointer, 3, 4, GL_FLOAT, false, sizeof(Vertex), (void*)48);
    glcall(glVertexAttribPointer, 4, 1, GL_FLOAT, false, sizeof(Vertex), (void*)64);
    glcall(glVertexAttribIPointer, 5, 1, GL_INT, sizeof(Vertex), (void*)68);
    glcall(glVertexAttribIPointer, 6, 1, GL_INT, sizeof(Vertex), (void*)72);
    glcall(glVertexAttribIPointer, 7, 1, GL_INT, sizeof(Vertex), (void*)76);
    glcall(glEnableVertexAttribArray, 0);
    glcall(glEnableVertexAttribArray, 1);
    glcall(glEnableVertexAttribArray, 2);
    glcall(glEnableVertexAttribArray, 3);
    glcall(glEnableVertexAttribArray, 4);
    glcall(glEnableVertexAttribArray, 5);
    glcall(glEnableVertexAttribArray, 6);
    glcall(glEnableVertexAttribArray, 7);
    glcall(glDrawElements, GL_TRIANGLES, road_layer.num_index, GL_UNSIGNED_INT, (void*)0);
}
