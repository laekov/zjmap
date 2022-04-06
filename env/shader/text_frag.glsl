#version 330 core

in vec2 texCoord;
layout (location = 0) out vec4 Color0;

uniform vec4 tex_color;
uniform sampler2D texture0;

void main()
{
    Color0  = tex_color * texture(texture0, texCoord).x;
}

