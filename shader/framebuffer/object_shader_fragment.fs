#version 330 core
in vec3 arg_world_pos;
in vec3 arg_world_normal;
in vec2 arg_tex_coord;

out vec4 FragColor;


uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform vec3 camera_front;


uniform sampler2D texture_color;
uniform sampler2D texture_specular;

void main()
{

   float dist = length(arg_world_pos - light_pos);
   // float decay = min(1.0, 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist));
   float decay = 1.0;
   vec3 img_color = vec3(texture(texture_color, arg_tex_coord));
   vec3 img_specular = vec3(texture(texture_specular, arg_tex_coord));


   float ratio_animate = 0.1;
   vec3 animate = ratio_animate * img_color;
   
   // 点光照
   vec3 light_dir = normalize(light_pos - arg_world_pos);
   // // 平行光照
   // vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));

   float ratio_diff = max(dot(normalize(arg_world_normal), light_dir), 0.0);
   vec3 diffusion = ratio_diff * img_color * decay ;
   

   vec3 ref_dir = reflect(-light_dir, normalize(arg_world_normal));
   vec3 camera_dir = normalize(camera_pos - arg_world_pos);
   
   float ratio_specular = pow(max(dot(ref_dir, camera_dir), 0.0), 32);
   vec3 specular = ratio_specular * img_specular * decay;
   // vec3 specular = vec3(0.1);// 0.01 * img_specular;

   float circle = max(dot(normalize(-camera_front), camera_dir), 0.0) > 0.999 ? 1 : 0;
   vec3 circle_v = circle * img_color;

   FragColor = vec4((animate + diffusion + specular + circle_v), 1.0);

}