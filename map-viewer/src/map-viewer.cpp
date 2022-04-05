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
    float visible_level;
    PrepareAssets();
    while (!ShouldClose())
    {
        if (zoom_scale < 0.001) visible_level = 0;
        else if (zoom_scale < 0.003) visible_level = 1;
        else if (zoom_scale < 0.01) visible_level = 2;
        else visible_level = 3;
        visible_level = std::log10(zoom_scale) * 2 + 6;
        visible_level = std::min(visible_level, 4.0f);

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
        glcall(glProgramUniformMatrix4fv, program, uniform_coord_trans, 1, false, (float*)&mat_cam);
        glcall(glProgramUniform1f, program, uniform_zoom_scale, zoom_scale);
        glcall(glProgramUniform1f, program, uniform_visible_level, visible_level);
        glcall(glUseProgram, program);
        glcall(glBindBuffer, GL_ARRAY_BUFFER, vertex_buffer);
        glcall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glcall(glVertexAttribPointer, 0, 4, GL_FLOAT, false, sizeof(Vertex), (void*)0);
        glcall(glVertexAttribPointer, 1, 4, GL_FLOAT, false, sizeof(Vertex), (void*)16);
        glcall(glVertexAttribPointer, 2, 4, GL_FLOAT, false, sizeof(Vertex), (void*)32);
        glcall(glVertexAttribPointer, 3, 1, GL_FLOAT, false, sizeof(Vertex), (void*)48);
        glcall(glVertexAttribIPointer, 4, 1, GL_INT, sizeof(Vertex), (void*)52);
        glcall(glVertexAttribIPointer, 5, 1, GL_INT, sizeof(Vertex), (void*)56);
        glcall(glVertexAttribIPointer, 6, 1, GL_INT, sizeof(Vertex), (void*)60);
        glcall(glEnableVertexAttribArray, 0);
        glcall(glEnableVertexAttribArray, 1);
        glcall(glEnableVertexAttribArray, 2);
        glcall(glEnableVertexAttribArray, 3);
        glcall(glEnableVertexAttribArray, 4);
        glcall(glEnableVertexAttribArray, 5);
        glcall(glEnableVertexAttribArray, 6);
        glcall(glDrawElements, GL_TRIANGLES, core->edge_count() * 12, GL_UNSIGNED_INT, (void*)0);
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

    Vertex * vertices = new Vertex [core->edge_count() * 6];
    uint32_t * indices = new uint32_t [core->edge_count() * 12];

    for (int i = 0; i < core->edge_count(); i++)
    {
        Edge edge = core->get_edge(i);
        Coord j1 = core->get_junction(edge.start), j2 = core->get_junction(edge.end);
        j1.x -= center[0]; j1.y -= center[1];
        j2.x -= center[0]; j2.y -= center[1];
        GRLVec4 p1(j1.x, j1.y, 0.0, 1.0), p2(j2.x, j2.y, 0.0, 1.0), direct = (p2 - p1);

        direct = direct / direct.norm();
        RouteInfo route_info = core->get_route_info(edge.route_id);
        float width = 30;

        GRLMat4 rotate90(0.0, -1.0, 0.0, 0.0,
                         1.0, 0.0,  0.0, 0.0,
                         0.0, 0.0,  1.0, 0.0,
                         0.0, 0.0,  0.0, 1.0);

        GRLVec4 color(1.0, 1.0, 1.0, 1.0);

        switch (route_info.level) {
            case 0: color = GRLVec4 (0.8, 0.7, 0.6, 1.0); width = 30 ;break;
            case 1: color = GRLVec4 (0.8, 0.8, 0.8, 1.0); width = 30 ;break;
            case 2: color = GRLVec4 (1.0, 1.0, 1.0, 1.0); width = 30 ;break;
            case 3: color = GRLVec4 (1.0, 1.0, 0.9, 1.0); width = 30 ;break;
            case 4: color = GRLVec4 (0.6, 1.0, 0.6, 1.0); width = 55 ;break;
        }

        vertices[i * 6 + 0] = Vertex{p2, color, GRLVec4() - rotate90 * direct, width, route_info.level, route_info.speedclass, route_info.funcclass};
        vertices[i * 6 + 1] = Vertex{p2, color, GRLVec4() + direct           , width, route_info.level, route_info.speedclass, route_info.funcclass};
        vertices[i * 6 + 2] = Vertex{p2, color, GRLVec4() + rotate90 * direct, width, route_info.level, route_info.speedclass, route_info.funcclass};
        vertices[i * 6 + 3] = Vertex{p1, color, GRLVec4() + rotate90 * direct, width, route_info.level, route_info.speedclass, route_info.funcclass};
        vertices[i * 6 + 4] = Vertex{p1, color, GRLVec4() - direct           , width, route_info.level, route_info.speedclass, route_info.funcclass};
        vertices[i * 6 + 5] = Vertex{p1, color, GRLVec4() - rotate90 * direct, width, route_info.level, route_info.speedclass, route_info.funcclass};
        indices[i * 12 + 0]  = i * 6 + 0;
        indices[i * 12 + 1]  = i * 6 + 1;
        indices[i * 12 + 2]  = i * 6 + 2;
        indices[i * 12 + 3]  = i * 6 + 0;
        indices[i * 12 + 4]  = i * 6 + 3;
        indices[i * 12 + 5]  = i * 6 + 2;
        indices[i * 12 + 6]  = i * 6 + 3;
        indices[i * 12 + 7]  = i * 6 + 4;
        indices[i * 12 + 8]  = i * 6 + 5;
        indices[i * 12 + 9]  = i * 6 + 3;
        indices[i * 12 + 10] = i * 6 + 5;
        indices[i * 12 + 11] = i * 6 + 0;
    }


    glcall(glCreateBuffers, 1, &vertex_buffer);
    glcall(glCreateBuffers, 1, &index_buffer);
    glcall(glNamedBufferData, vertex_buffer, sizeof(Vertex) * core->edge_count() * 6, vertices, GL_STATIC_DRAW);
    glcall(glNamedBufferData, index_buffer, sizeof(uint32_t) * core->edge_count() * 12, indices, GL_STATIC_DRAW);

    delete [] vertices;
    delete [] indices;

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
