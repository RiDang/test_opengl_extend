#version 330 core
layout (location = 0) in vec3 in_pos;

out vec3 arg_tex_coord;


// uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
   arg_tex_coord = in_pos;

   vec4 pos = projection_mat * view_mat * vec4(in_pos, 1.0);
   gl_Position = pos.xyww;
   // gl_Position = pos;
   
}