#version 330 core

layout (location = 0) in vec4 vs_coord1;
layout (location = 1) in vec4 vs_coord2;
layout (location = 2) in vec4 vs_rotation;
layout (location = 3) in vec4 vs_color;
layout (location = 4) in float vs_width;
layout (location = 5) in int vs_level;
layout (location = 6) in int vs_speedclass;
layout (location = 7) in int vs_funcclass;


out vec4 fs_color;

uniform mat4 coord_trans;
uniform mat4 screen_trans;
uniform float zoom_scale;

float mixed_level()
{
    return 5 - vs_funcclass;
}

void main()
{
    vec4 coord1 = vs_coord1 * coord_trans;
    vec4 coord2 = vs_coord2 * coord_trans;
    vec2 direct = normalize((coord1 - coord2).xy);
    direct = vec2(
    direct.x * vs_rotation[0] + direct.y * vs_rotation[1],
    direct.x * vs_rotation[2] + direct.y * vs_rotation[3]
    );
    gl_Position = coord1 + vec4(direct, 0.0, 0.0) * vs_width * screen_trans * zoom_scale;
    fs_color = vs_color;
}



