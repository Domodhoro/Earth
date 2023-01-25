#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "./lib/stb_image.h"

extern "C" {
#include "./lualib/lua.h"
#include "./lualib/lualib.h"
#include "./lualib/lauxlib.h"
#include "./lualib/luaconf.h"
}

constexpr auto FPS {60};

struct my_exception {
    my_exception(char *file, int line, const char *description) {
        printf("Ops! Uma falha ocorreu...\n\n");
        printf("File:        %s\n", basename(file));
        printf("Line:        %i\n", line);
        printf("Description: %s\n", description);
    }

    ~my_exception() {
        exit(EXIT_FAILURE);
    }
};

auto WINDOW_WIDTH       {800};
auto WINDOW_HEIGHT      {600};
auto WINDOW_TITLE       {"Earth"};
auto CAMERA_SPEED       {0.5f};
auto CAMERA_FOV         {60.0f};
auto CAMERA_SENSITIVITY {0.1f};

#include "./src/lua_script.hpp"
#include "./src/shader.hpp"
#include "./src/stb_image_wrapper.hpp"
#include "./src/camera.hpp"
#include "./src/sphere.hpp"

lua_script::lua_script lua {"./script.lua"};
camera::camera cam         {static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT)};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void keyboard_callback        (GLFWwindow *window);
void mouse_callback           (GLFWwindow *window);

int main(int argc, char *argv[]) {
    printf("%s\n", argv[0]);

    if (glfwInit() == GLFW_NOT_INITIALIZED) my_exception {__FILE__, __LINE__, "falha ao iniciar o GLFW"};

    glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window {
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr)
    };

    if (window == nullptr) my_exception {__FILE__, __LINE__, "falha ao criar a janela de visualização"};

    glfwMakeContextCurrent(window);

    const GLFWvidmode *mode {
        glfwGetVideoMode(glfwGetPrimaryMonitor())
    };

    auto window_pos_x {(mode->width  - WINDOW_WIDTH)  / 2};
    auto window_pos_y {(mode->height - WINDOW_HEIGHT) / 2};

    glfwSetWindowPos(window, window_pos_x, window_pos_y);

    stb_image_wrapper::load_window_icon(window, "./img/icon.bmp");

    glewExperimental = true;

    if (glewInit() != GLEW_OK) my_exception {__FILE__, __LINE__, "falha ao iniciar GLEW"};

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    cam.disable_cursor (window);
    cam.set_speed      (CAMERA_SPEED);
    cam.set_sensitivity(CAMERA_SENSITIVITY);
    cam.set_FOV        (CAMERA_FOV);
    cam.set_position   (glm::tvec3<float>(0.0f, 0.0f, -5.0f));

    glfwSetWindowPos(window, (mode->width - WINDOW_WIDTH) / 2, (mode->height - WINDOW_HEIGHT) / 2);

    auto earth_shader  {shader::shader_program         ("./glsl/vertex.glsl", "./glsl/fragment.glsl")};
    auto earth_texture {stb_image_wrapper::load_texture("./img/earth.bmp")};

    sphere::sphere earth {64, 64};

    glEnable   (GL_DEPTH_TEST);
    glEnable   (GL_CULL_FACE);
    glFrontFace(GL_CCW);

    auto last_frame    {0.0f};
    auto current_frame {0.0f};

    while (!glfwWindowShouldClose(window)) {
        current_frame = static_cast<float>(glfwGetTime());

        if ((current_frame - last_frame) > (1.0f / static_cast<float>(FPS))) {
            glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            keyboard_callback(window);
            mouse_callback   (window);

            earth.draw(earth_shader, earth_texture, cam);

            glfwSwapBuffers(window);
            glfwPollEvents ();

            last_frame = current_frame;
        }
    }

    glDeleteTextures(1, &earth_texture);

    glfwDestroyWindow(window);
    glfwTerminate    ();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboard_callback(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.keyboard_process(camera::CAMERA_MOVEMENTS::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.keyboard_process(camera::CAMERA_MOVEMENTS::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.keyboard_process(camera::CAMERA_MOVEMENTS::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.keyboard_process(camera::CAMERA_MOVEMENTS::LEFT);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam.get_position().y += 0.5f;
}

void mouse_callback(GLFWwindow *window) {
    auto x {0.0d};
    auto y {0.0d};

    glfwGetCursorPos(window, &x, &y);

    static auto first_mouse {true};
    static auto last_x      {0.0f};
    static auto last_y      {0.0f};

    if (first_mouse) {
        last_x      = static_cast<float>(x);
        last_y      = static_cast<float>(y);
        first_mouse = false;
    }

    auto off_set_x {x - last_x};
    auto off_set_y {last_y - y};

    last_x = x;
    last_y = y;

    cam.mouse_process(off_set_x, off_set_y);
}
