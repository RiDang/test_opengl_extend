#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

out vec2 arg_tex_coord;

void main()
{
   arg_tex_coord = in_tex_coord;

   gl_Position = vec4(in_pos * 0.5 + 0.5, 1.0);
   
}