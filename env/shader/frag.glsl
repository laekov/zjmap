#version 330 core

in vec4 fs_color;

out vec4 res_color;

uniform float visible_level;

void main()
{
    res_color = fs_color * visible_level;
}