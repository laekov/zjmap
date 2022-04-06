//
// Created by zijia on 2022/4/5.
//

#include "text-manager.h"

TextManager::TextManager() {

}

void TextManager::Init(const char *main_font_path, const char *ascii_font_path, uint32_t pixel_width,
                       uint32_t pixel_height) {
    Init();
    SetMainFont(main_font_path);
    if (!ascii_font_path) ascii_font_path = main_font_path;
    SetASCIIFont(ascii_font_path);
    SetTextSize(pixel_width, pixel_height);
    SetTextColor(Grassland::GRLVec4(1.0, 1.0, 1.0, 1.0));
}

void TextManager::SetMainFont(const char *main_font_path) {
    if (map_font_face.count(main_font_path)) {
        active_main_font = map_font_face[main_font_path];
    }
    else
    {
        FT_Face new_face;
        if (FT_New_Face(ft_library, main_font_path, 0, &new_face))
        {
            printf("[ERROR] Failed to load font. (%s)\n", main_font_path);
            return;
        }
        map_font_face[main_font_path] = active_main_font = new_face;
    }

    FT_Set_Pixel_Sizes(active_main_font, pixel_width, pixel_height);
}

void TextManager::SetASCIIFont(const char *ascii_font_path) {
    if (map_font_face.count(ascii_font_path)) {
        active_ascii_font = map_font_face[ascii_font_path];
    }
    else
    {
        FT_Face new_face;
        if (FT_New_Face(ft_library, ascii_font_path, 0, &new_face))
        {
            printf("[ERROR] Failed to load font. (%s)\n", ascii_font_path);
            return;
        }
        map_font_face[ascii_font_path] = active_ascii_font = new_face;
    }
    FT_Set_Pixel_Sizes(active_ascii_font, pixel_width, pixel_height);
}

void TextManager::SetTextSize(uint32_t _pixel_width, uint32_t _pixel_height) {
    pixel_width = _pixel_width;
    pixel_height = _pixel_height;
    if (active_main_font) FT_Set_Pixel_Sizes(active_main_font, pixel_width, pixel_height);
    if (active_ascii_font) FT_Set_Pixel_Sizes(active_ascii_font, pixel_width, pixel_height);
}

void TextManager::SetTextColor(Grassland::GRLVec4 color) {
    glcall(glProgramUniform4f, render_program, uniform_tex_color, color[0], color[1], color[2], color[3]);
}

void TextManager::SetFont(const char *font_path) {
    SetMainFont(font_path);
    SetASCIIFont(font_path);
}

TextManager::CharAsset TextManager::get_asset(wchar_t wchar) {
    CharIndex char_index;
    char_index.pixel_height = pixel_height;
    char_index.pixel_width = pixel_width;
    char_index.c = wchar;
    FT_Face ft_face = (wchar < 128) ? active_ascii_font : active_main_font;

    if (map_font_char_map[ft_face].count(char_index))
        return map_font_char_map[ft_face][char_index];
    CharAsset char_asset = {};
    if (FT_Load_Char(ft_face, wchar, FT_LOAD_RENDER))
        return char_asset;

    glcall(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);

    int32_t binding_texture;
    glcall(glGetIntegerv, GL_TEXTURE_BINDING_2D, &binding_texture);
    glcall(glGenTextures, 1, &char_asset.char_tex);
    glcall(glBindTexture, GL_TEXTURE_2D, char_asset.char_tex);
    glcall(
    glTexImage2D,
            GL_TEXTURE_2D,
            0,
            GL_RED,
            ft_face->glyph->bitmap.width,
            ft_face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            ft_face->glyph->bitmap.buffer
    );
    glcall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glcall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glcall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glcall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    char_asset.char_width = ft_face->glyph->bitmap.width;
    char_asset.char_height = ft_face->glyph->bitmap.rows;
    char_asset.char_bearingX = ft_face->glyph->bitmap_left;
    char_asset.char_bearingY = ft_face->glyph->bitmap_top;
    char_asset.char_advance = ft_face->glyph->advance.x;
    glcall(glBindTexture, GL_TEXTURE_2D, binding_texture);
    glcall(glPixelStorei, GL_UNPACK_ALIGNMENT, 4);

    return map_font_char_map[ft_face][char_index] = char_asset;
}

void TextManager::DrawText(float base_x, float base_y, std::wstring wstr) {
    int32_t wnd_width, wnd_height;
    glfwGetWindowSize(GetWindowHandle(), &wnd_width, &wnd_height);
    float x_per_pixel = 2.0 / (float)wnd_width, y_per_pixel = 2.0 / (float)wnd_height;
    auto trans_mat = Grassland::GRLMat3(1.0);
    glUseProgram(render_program);
    int vertex_array_binding;
    int texture_binding;
    glcall(glGetIntegerv, GL_VERTEX_ARRAY_BINDING, &vertex_array_binding);
    glcall(glGetIntegerv, GL_TEXTURE_BINDING_2D, &texture_binding);
    for (auto c : wstr)
    {
        glBindVertexArray(render_vertex_array);
        CharAsset asset = get_asset(c);
        glBindTexture(GL_TEXTURE_2D, asset.char_tex);
        float
                x0 = base_x + asset.char_bearingX,
                y0 = base_y - asset.char_bearingY,
                x1 = x0 + asset.char_width,
                y1 = y0 + asset.char_height;

        x0 *= x_per_pixel; x0 = x0 - 1.0;
        y0 *= y_per_pixel; y0 = -y0 + 1.0;
        x1 *= x_per_pixel; x1 = x1 - 1.0;
        y1 *= y_per_pixel; y1 = -y1 + 1.0;
        trans_mat = Grassland::GRLMat3(
                x1 - x0, 0.0, x0,
                0.0, y1 - y0, y0,
                0.0, 0.0, 1.0
        );
        glProgramUniformMatrix3fv(render_program, uniform_trans_mat, 1, false, &trans_mat[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        base_x += asset.char_advance / 64.0f;
    }
    glBindTexture(GL_TEXTURE_2D, texture_binding);
    glBindVertexArray(vertex_array_binding);
}

void TextManager::Init() {
    if (FT_Init_FreeType(&ft_library)) {
        puts("[ERROR] Failed to init FT_Library.");
        return;
    }

    uint32_t render_vertex_shader = CompileShaderFromFile("shader/text_vert.glsl", GL_VERTEX_SHADER);
    uint32_t render_fragment_shader = CompileShaderFromFile("shader/text_frag.glsl", GL_FRAGMENT_SHADER);
    render_program = LinkProgram(render_vertex_shader, render_fragment_shader);
    glcall(glDeleteShader, render_vertex_shader);
    glcall(glDeleteShader, render_fragment_shader);

    uniform_tex_color = glGetUniformLocation(render_program, "tex_color");
    uniform_trans_mat = glGetUniformLocation(render_program, "trans_mat");

    Grassland::GRLVec2 texcoords[] = {
            Grassland::GRLVec2 (0.0, 0.0),
            Grassland::GRLVec2 (0.0, 1.0),
            Grassland::GRLVec2 (1.0, 0.0),
            Grassland::GRLVec2 (0.0, 1.0),
            Grassland::GRLVec2 (1.0, 0.0),
            Grassland::GRLVec2 (1.0, 1.0)
    };

    int vertex_array_binding;
    glcall(glGetIntegerv, GL_VERTEX_ARRAY_BINDING, &vertex_array_binding);
    glcall(glCreateBuffers, 1, &render_vertex_buffer);
    glcall(glNamedBufferData, render_vertex_buffer, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
    glcall(glGenVertexArrays, 1, &render_vertex_array);
    glcall(glBindVertexArray, render_vertex_array);
    glcall(glBindBuffer, GL_ARRAY_BUFFER, render_vertex_buffer);
    glcall(glVertexAttribPointer, 0, 2, GL_FLOAT, false, sizeof(Grassland::GRLVec2), (void*)0);
    glcall(glEnableVertexAttribArray, 0);
    glcall(glBindVertexArray, vertex_array_binding);
    pixel_width = 0;
    pixel_height = 32;
    active_main_font = active_ascii_font = nullptr;
}
