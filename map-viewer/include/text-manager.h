//
// Created by zijia on 2022/4/5.
//

#ifndef ZJMAP_TEXT_MANAGER_H
#define ZJMAP_TEXT_MANAGER_H


#include <ft2build.h>
#include FT_FREETYPE_H
#include "Grassland/Math/Math.h"
#include "glutil.h"
#include <map>

class TextManager
{
public:
    TextManager();
    void Init();
    void Init(
            const char * main_font_path,
            const char * ascii_font_path,
            uint32_t pixel_width,
            uint32_t pixel_height
            );
    void SetMainFont(const char * main_font_path);
    void SetASCIIFont(const char * ascii_font_path);
    void SetFont(const char * font_path);
    void SetTextSize(uint32_t pixel_width, uint32_t pixel_height);
    void SetTextColor(Grassland::GRLVec4 color);
    void DrawText(float x, float y, std::wstring wstr);
private:
    FT_Library ft_library;
    struct CharIndex
    {
    public:
        uint32_t pixel_width;
        uint32_t pixel_height;
        wchar_t c;
        bool operator < (const CharIndex& index2) const
        {
            return (pixel_width < index2.pixel_width) || (pixel_width == index2.pixel_width) &&
                                                         ((pixel_height < index2.pixel_height) ||
                                                          (pixel_height ==
                                                           index2.pixel_height) &&
                                                          (c < index2.c));
        }
    };

    struct CharAsset
    {
    public:
        uint32_t char_tex;
        uint32_t char_width;
        uint32_t char_height;
        int32_t char_bearingX;
        int32_t char_bearingY;
        int32_t char_advance;
    };

    std::map<std::string, FT_Face> map_font_face;
    std::map<FT_Face, std::map<CharIndex, CharAsset>> map_font_char_map;
    FT_Face active_main_font;
    FT_Face active_ascii_font;
    uint32_t pixel_width, pixel_height;

    uint32_t render_program;
    uint32_t render_vertex_buffer;
    uint32_t render_vertex_array;
    int32_t uniform_trans_mat;
    int32_t uniform_tex_color;

    CharAsset get_asset(wchar_t wchar);
};

#endif //ZJMAP_TEXT_MANAGER_H
