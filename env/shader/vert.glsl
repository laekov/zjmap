#version 330 core

layout (location = 0) in vec4 vs_coord;
layout (location = 1) in vec4 vs_color;
layout (location = 2) in vec4 vs_expand;
layout (location = 3) in float vs_width;
layout (location = 4) in int vs_level;
layout (location = 5) in int vs_speedclass;
layout (location = 6) in int vs_funcclass;


out vec4 fs_color;

uniform mat4 coord_trans;
uniform float zoom_scale;
uniform float visible_level;

float mixed_level()
{
    return 5 - vs_funcclass;
}

void main()
{
    vec4 coord = vs_coord + vs_expand * vs_width * zoom_scale * 1e-3;
    if (mixed_level() < visible_level) coord = vs_coord;
    gl_Position = coord * coord_trans;
    fs_color = vs_color;
}



