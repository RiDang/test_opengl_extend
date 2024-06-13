#version 330 core

in vec3 arg_tex_coord;

out vec4 FragColor;

uniform samplerCube tex_skybox;

void main()
{
   FragColor = texture(tex_skybox, arg_tex_coord);
}