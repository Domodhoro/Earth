#ifndef CAMERA_HPP
#define CAMERA_HPP

namespace camera {

struct camera {
    camera() = default;

    camera(const int width, const int height) : aspect {static_cast<float>(width) / static_cast<float>(height)} {}

    void disable_cursor(GLFWwindow *window) const { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

    void set_position   (const glm::tvec3<float> Position) { this->Position    = Position; }
    void set_aspect     (const float aspect)               { this->aspect      = aspect; }
    void set_speed      (const float speed)                { this->speed       = speed; }
    void set_FOV        (const float FOV)                  { this->FOV         = FOV; }
    void set_near_plane (const float near)                 { this->near        = near; }
    void set_far_plane  (const float far)                  { this->far         = far; }

    glm::tvec3<float> get_position () const { return Position; }
    glm::mat4 get_projection_matrix() const { return glm::perspective<float>(glm::radians(FOV), aspect, near, far); }
    glm::mat4 get_view_matrix      () const { return glm::lookAt<float>     (Position, Position + Front, Up); }

    void keyboard_update(const CAMERA_MOVEMENTS input) {
        if (input == CAMERA_MOVEMENTS::FORWARD)  Position += speed * Front;
        if (input == CAMERA_MOVEMENTS::RIGHT)    Position -= speed * Front;
        if (input == CAMERA_MOVEMENTS::BACKWARD) Position -= speed * glm::normalize(glm::cross(Front, Up));
        if (input == CAMERA_MOVEMENTS::LEFT)     Position += speed * glm::normalize(glm::cross(Front, Up));
    }

    void mouse_update(double &off_set_x, double &off_set_y, const float sensitivity) {
        alpha += off_set_x * sensitivity;
        beta  += off_set_y * sensitivity;

        const auto angle_max {89.0f};

        if (beta >=  angle_max) beta =  angle_max;
        if (beta <= -angle_max) beta = -angle_max;

        Direction.x = cos(glm::radians(alpha)) * cos(glm::radians(beta));
        Direction.y = sin(glm::radians(beta));
        Direction.z = sin(glm::radians(alpha)) * cos(glm::radians(beta));

        Front = glm::normalize(Direction);
    }

protected:
    float aspect      {1.0f};
    float speed       {1.0f};
    float FOV         {60.0f};
    float near        {0.001f};
    float far         {1000.0f};
    float alpha       {90.0f};
    float beta        {0.0f};

    glm::tvec3<float> Front     {0.0f, 0.0f, 1.0f};
    glm::tvec3<float> Position  {0.0f};
    glm::tvec3<float> Up        {0.0f, 1.0f, 0.0f};
    glm::tvec3<float> Direction {Front};
};

}

#endif
