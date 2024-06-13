#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

#include "shader/shader.h"
#include "camera/camera.h"
#include "texture/texture.h"


typedef struct {
    glm::dvec2 last_down;
    bool is_last_down{false};
} MouseInfo;


const int kWidth = 800, kHeight = 600;
MouseInfo mouse_left_info, mouse_right_info;
Camera camera;
float delta_time = 0.5f;
float last_time = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset ){
    camera.update_forward(yoffset);
    // camera.info();
}


void processInput(GLFWwindow* window){

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ){
        glfwSetWindowShouldClose(window, true);
    }
    // float  speed = 0.05f;;
    float  speed = delta_time * 10;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.update_forward(speed);
        camera.info();

    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.update_forward(speed);
        camera.info();

    }
    else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.update_camera(0, -speed);
        camera.info();
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.update_camera(0, speed);
        camera.info();
    }

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        camera.reset();
        camera.info();
    }

    // ====== 鼠标右键设置 ===========


    auto process_mouse_key = [&](unsigned int left_or_right, MouseInfo& mouse_info){
        if(glfwGetMouseButton(window, left_or_right) == GLFW_PRESS){
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            glm::dvec2 cur_pos = glm::vec2(x, y);
            if(mouse_info.is_last_down){
                glm::dvec2 delta_pos = cur_pos - mouse_info.last_down;
                delta_pos *= 0.1;
                // camera.update_camera((float)(delta_pos[1]), (float)(delta_pos[0]));
                if(left_or_right == GLFW_MOUSE_BUTTON_RIGHT){
                    camera.update_camera((float)(delta_pos[1]), (float)(delta_pos[0]));
                }
                else{
                    camera.update_object((float)(delta_pos[1]), (float)(delta_pos[0]));
                }

            }
            else{
                mouse_info.is_last_down = true;
            }
            mouse_info.last_down = cur_pos;

        }

        if(glfwGetMouseButton(window, left_or_right) == GLFW_RELEASE){
            mouse_info.is_last_down = false;
        }
    };

    process_mouse_key(GLFW_MOUSE_BUTTON_RIGHT, mouse_right_info);
    process_mouse_key(GLFW_MOUSE_BUTTON_LEFT, mouse_left_info);

    // ====== 鼠标左设置 ===========
    
}

void show_mat4(const glm::mat4& mat4){
    std::cout << "[";
    for(size_t i=0; i<4; i++){
        std::cout << "[";
        for(size_t j=0; j<4; j++){
            if(j+1 < 4)
              std::cout << mat4[j][i] << ", ";
            else
              std::cout << mat4[j][i];
        }

        if(i < 3)
            std::cout << "]," << std::endl;
        else
            std::cout << "]";
    }
    std::cout << "]" << std::endl;

}

std::string get_root_path(){
    return "/home/ubt/Projects/opengl/test_opengl_test";
}

GLFWwindow* InitWindow(){
    // ============== 窗口初始化 start

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD " << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

Shader::PathMap get_path_map(const std::string& prefix){

    const std::string ROOT_PATH = get_root_path();
    const std::string vertex_path = ROOT_PATH + "/shader/" + prefix + "_shader_vertex.vs";
    const std::string frag_path = ROOT_PATH + "/shader/"+ prefix + "_shader_fragment.fs";
    Shader::PathMap path_map{{"vertex",vertex_path},
                            {"frag",frag_path}};
    return path_map;
}

int main()
{
    // ============== 窗口初始化 end

    GLFWwindow* window = InitWindow();
    glGetError(); 



    
    glEnable(GL_DEPTH_TEST);
    Shader::PathMap object_ath_map = get_path_map("framebuffer/object");
    Shader object_shader(object_ath_map);

    Shader::PathMap light_path_map = get_path_map("framebuffer/light");
    Shader light_shader(light_path_map);
;

    Shader::PathMap quad_path_map = get_path_map("framebuffer/quad");
    Shader quad_shader(quad_path_map);

    const std::string tex_color_path = get_root_path() + "/data/container2.png";
    const std::string tex_specular_path = get_root_path() + "/data/container2_specular.png";
    unsigned int tex_color = load_texture(tex_color_path.c_str());
    unsigned int tex_specular = load_texture(tex_specular_path.c_str());

    // std::vector<std::string> face_paths = {
    //         "right", "left", 
    //         "top", "bottom", 
    //         "front", "right"};
    // std::for_each(face_paths.begin(), face_paths.end(), 
    //             [](const std::string& name){return get_root_path() + "/" + name + ".jpg";});

    // unsigned int tex_cube = load_cubemap(face_paths);
    


    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float quad[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };

    unsigned int VBOs[2], VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    // ----- VertexArray 0 ------
    glBindVertexArray(VAOs[0]);

    // 定点绑定，拷贝到GPU 中
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 属性绑定 Array 0，讲属性绑定到相应的点上
    int SIZEs[] = {8, 3, 3, 2}; // 总长度，点长度，颜色长度，纹理长度
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SIZEs[0]*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SIZEs[0]*sizeof(float), (void*)((SIZEs[1] ) * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, SIZEs[0]*sizeof(float), (void*)((SIZEs[1] + SIZEs[2] ) * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ----- VertexArray 1 ------
    glBindVertexArray(VAOs[1]);

    // 定点绑定，拷贝到GPU 中
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    // 属性绑定 Array 0，讲属性绑定到相应的点上
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    
    unsigned int framebf;
    glGenFramebuffers(1, &framebf);
    glBindFramebuffer(GL_FRAMEBUFFER, framebf);

    // ( 1 )
    unsigned int tex_bf;
    glGenTextures(1, &tex_bf);
    glBindTexture(GL_TEXTURE_2D, tex_bf);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kWidth, kHeight,0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_bf, 0);
    
    // ( 2 )
    unsigned int render_bf;
    glGenRenderbuffers(1, &render_bf);
    glBindRenderbuffer(GL_RENDERBUFFER, render_bf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, kWidth, kHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_bf);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "ERROR: GL_FRAMEBUFFER create fail " << glGetError() << std::endl;
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // 全局变量
    object_shader.use();
    object_shader.set_int("texture_color", 0);
    object_shader.set_int("texture_specular", 1);
    quad_shader.use();
    quad_shader.set_int("tex_quad", 0);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::vec3 light_pos(1.0f, 1.0f, 2.0f);

    while(!glfwWindowShouldClose(window)){
        float cur_time = glfwGetTime();
        delta_time = cur_time - last_time;
        last_time = cur_time;

        glfwSetScrollCallback(window, scroll_callback);

        processInput(window);
        glBindFramebuffer(GL_FRAMEBUFFER, framebf);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          =glm::mat4(1.0f);
        glm::mat4 projection    =glm::mat4(1.0f);
        glm::mat4 normal_model_mat = glm::mat4(1.0f);


        view = camera.view_mat4_;
        projection = glm::perspective(glm::radians(45.0f), (float)kWidth / (float)kHeight, 0.1f, 100.0f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)cur_time, glm::vec3(0.2f, 1.0f, 0.2f));
        model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_color);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_specular);

        object_shader.use();
        // - 设置矩阵
        object_shader.set_mat4("model_mat", glm::value_ptr(model));
        object_shader.set_mat4("view_mat", glm::value_ptr(view));
        object_shader.set_mat4("projection_mat", glm::value_ptr(projection));
        // 设置相机
        object_shader.set_vec3("light_pos", glm::value_ptr(light_pos));
        object_shader.set_vec3("camera_pos", glm::value_ptr(camera.camera_pos_));
        object_shader.set_vec3("camera_front", glm::value_ptr(camera.camera_front_));
        
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //  2
        // -------------------------------
        // glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- 2.1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_color);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_specular);

        object_shader.use();
        // - 设置矩阵
        model = glm::mat4(1.0f);
        model = glm::rotate(model, cur_time + glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        object_shader.set_mat4("model_mat", glm::value_ptr(model));
        object_shader.set_mat4("view_mat", glm::value_ptr(view));
        object_shader.set_mat4("projection_mat", glm::value_ptr(projection));
        // 设置相机
        object_shader.set_vec3("light_pos", glm::value_ptr(light_pos));
        object_shader.set_vec3("camera_pos", glm::value_ptr(camera.camera_pos_));
        object_shader.set_vec3("camera_front", glm::value_ptr(camera.camera_front_));
        
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // --- 2.2
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_bf);
        quad_shader.use();

        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        
        glfwSwapBuffers(window);
        glfwPollEvents();

        // start = std::chrono::high_resolution_clock::now();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glfwTerminate();
    return 0;
}

