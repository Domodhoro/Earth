/*
    MIT License

    Copyright (c) 2023 Guilherme M. Aguiar (guilhermemaguiar2022@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifdef __cplusplus

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./lib/stb_image.h"
}

#endif

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct my_exception {
    my_exception(const char *file, int line, const char *description) {
        printf("Ops! Uma falha ocorreu...\n\n");
        printf("File:        %s\n", basename(file));
        printf("Line:        %i\n", line);
        printf("Description: %s\n", description);
    }

    ~my_exception() {
        exit(EXIT_FAILURE);
    }
};

constexpr auto FPS                {60};
constexpr auto WINDOW_WIDTH       {800};
constexpr auto WINDOW_HEIGHT      {600};
constexpr auto WINDOW_TITLE       {"Earth"};
constexpr auto CAMERA_SPEED       {0.1f};
constexpr auto CAMERA_FOV         {60.0f};
constexpr auto CAMERA_SENSITIVITY {0.1f};

template<typename T>
struct vertex_3d {
    T x;
    T y;
    T z;
    T u;
    T v;
};

enum struct CAMERA_MOVEMENTS : int {
    FORWARD = 0,
    BACKWARD,
    RIGHT,
    LEFT
};

#include "./src/shader.hpp"
#include "./src/stb_image_wrapper.hpp"
#include "./src/camera.hpp"
#include "./src/sphere.hpp"

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void keyboard_callback(GLFWwindow *window, camera::camera &cam) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.keyboard_update(CAMERA_MOVEMENTS::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam.keyboard_update(CAMERA_MOVEMENTS::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.keyboard_update(CAMERA_MOVEMENTS::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam.keyboard_update(CAMERA_MOVEMENTS::LEFT);
}

static void mouse_callback(GLFWwindow *window, camera::camera &cam) {
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

    cam.mouse_update(off_set_x, off_set_y);
}

int main(int argc, char *argv[]) {
    printf("%s\n", argv[0]);

    if (glfwInit() == GLFW_NOT_INITIALIZED) my_exception {__FILE__, __LINE__, "falha ao iniciar o GLFW"};

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DECORATED, false);
    glfwWindowHint(GLFW_RESIZABLE, false);

    auto window {
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr)
    };

    if (window == nullptr) my_exception {__FILE__, __LINE__, "falha ao criar a janela de visualização"};

    glfwMakeContextCurrent(window);

    auto mode {
        glfwGetVideoMode(glfwGetPrimaryMonitor())
    };

    const auto window_pos_x {(mode->width  - WINDOW_WIDTH)  / 2};
    const auto window_pos_y {(mode->height - WINDOW_HEIGHT) / 2};

    glfwSetWindowPos(window, window_pos_x, window_pos_y);
    glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    stb_image_wrapper::load_window_icon(window, "./img/icon.bmp");

    glewExperimental = true;

    if (glewInit() != GLEW_OK) my_exception {__FILE__, __LINE__, "falha ao iniciar GLEW"};

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    camera::camera cam {
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT)
    };

    cam.disable_cursor (window);
    cam.set_speed      (CAMERA_SPEED);
    cam.set_sensitivity(CAMERA_SENSITIVITY);
    cam.set_FOV        (CAMERA_FOV);
    cam.set_position   (glm::tvec3<float>(0.0f, 0.0f, -5.0f));

    auto shader_program{shader::shader("./glsl/vertex.glsl", "./glsl/fragment.glsl")};
    auto earth_texture {stb_image_wrapper::load_texture("./img/earth.bmp")};
    auto moon_texture  {stb_image_wrapper::load_texture("./img/moon.bmp")};

    sphere::sphere earth {64, 64};
    sphere::sphere moon  {64, 64};

    glEnable   (GL_DEPTH_TEST);
    glEnable   (GL_CULL_FACE);
    glFrontFace(GL_CCW);

    auto last_frame    {0.0f};
    auto current_frame {0.0f};

    while (!glfwWindowShouldClose(window)) {
        current_frame = glfwGetTime();

        if ((current_frame - last_frame) > (1.0f / static_cast<float>(FPS))) {
            glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            keyboard_callback(window, cam);
            mouse_callback   (window, cam);

            glm::mat4 earth_model {1.0f};
            glm::mat4 moon_model  {1.0f};

            const auto angle {
                glm::radians<float>(45.0f * glfwGetTime())
            };

            earth_model = glm::rotate   (earth_model, angle, glm::tvec3<float>(0.0f, 1.0f, 0.0f));
            moon_model  = glm::rotate   (moon_model, angle, glm::tvec3<float>(0.0f, 1.0f, 0.0f));
            moon_model  = glm::translate(moon_model, glm::tvec3<float>(10.0f, 0.0, 0.0f));
            moon_model  = glm::scale    (moon_model, glm::tvec3<float>(0.3f));

            earth.draw(earth_model, shader_program, earth_texture, cam);
            moon.draw (moon_model, shader_program, moon_texture, cam);

            glfwSwapBuffers(window);
            glfwPollEvents ();

            last_frame = current_frame;
        }
    }

    glDeleteTextures (1, &earth_texture);
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
