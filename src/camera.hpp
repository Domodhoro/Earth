#ifndef CAMERA_HPP
#define CAMERA_HPP

namespace camera {

struct camera {
    camera(const float aspect) : m_aspect {aspect} {}

    void disable_cursor(GLFWwindow *window) const { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

    void set_position   (const glm::tvec3<float> Position) { m_Position    = Position; }
    void set_speed      (const float speed)                { m_speed       = speed; }
    void set_sensitivity(const float sensitivity)          { m_sensitivity = sensitivity; }
    void set_FOV        (const float FOV)                  { m_FOV         = FOV; }
    void set_near_plane (const float near)                 { m_near        = near; }
    void set_far_plane  (const float far)                  { m_far         = far; }

    glm::tvec3<float> get_position () const { return m_Position; }
    glm::mat4 get_projection_matrix() const { return glm::perspective<float>(glm::radians(m_FOV), m_aspect, m_near, m_far); }
    glm::mat4 get_view_matrix      () const { return glm::lookAt<float>     (m_Position, m_Position + m_Front, m_Up); }

    void keyboard_update(const CAMERA_MOVEMENTS input) {
        if (input == CAMERA_MOVEMENTS::FORWARD)  m_Position += m_speed * m_Front;
        if (input == CAMERA_MOVEMENTS::RIGHT)    m_Position -= m_speed * m_Front;
        if (input == CAMERA_MOVEMENTS::BACKWARD) m_Position -= m_speed * glm::normalize(glm::cross(m_Front, m_Up));
        if (input == CAMERA_MOVEMENTS::LEFT)     m_Position += m_speed * glm::normalize(glm::cross(m_Front, m_Up));
    }

    void mouse_update(double &off_set_x, double &off_set_y) {
        m_alpha += off_set_x * m_sensitivity;
        m_beta  += off_set_y * m_sensitivity;

        const auto angle_max {89.0f};

        if (m_beta >=  angle_max) m_beta =  angle_max;
        if (m_beta <= -angle_max) m_beta = -angle_max;

        m_Direction.x = cos(glm::radians(m_alpha)) * cos(glm::radians(m_beta));
        m_Direction.y = sin(glm::radians(m_beta));
        m_Direction.z = sin(glm::radians(m_alpha)) * cos(glm::radians(m_beta));

        m_Front = glm::normalize(m_Direction);
    }

protected:
    float m_aspect      {1.0f};
    float m_speed       {1.0f};
    float m_sensitivity {0.1f};
    float m_FOV         {60.0f};
    float m_near        {0.001f};
    float m_far         {1000.0f};
    float m_alpha       {90.0f};
    float m_beta        {0.0f};

    glm::tvec3<float> m_Front     {0.0f, 0.0f, 1.0f};
    glm::tvec3<float> m_Position  {0.0f};
    glm::tvec3<float> m_Up        {0.0f, 1.0f, 0.0f};
    glm::tvec3<float> m_Direction {m_Front};
};

}

#endif
