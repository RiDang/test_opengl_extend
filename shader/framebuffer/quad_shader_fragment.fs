#version 330 core

in vec2 arg_tex_coord;

out vec4 FragColor;

uniform sampler2D tex_quad;

void main()
{
   FragColor = vec4(vec3(texture(tex_quad, arg_tex_coord)), 1.0);
}