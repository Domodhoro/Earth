#ifndef SPHERE_HPP
#define SPHERE_HPP

namespace sphere {

struct vertex_3d {
    float X {0.0f};
    float Y {0.0f};
    float Z {0.0f};
    float U {0.0f};
    float V {0.0f};
};

constexpr auto TAU {2.0f * glm::pi<float>()};

struct sphere {
    sphere(const float rings, const float sectors) {
        auto R {1.0f / (static_cast<float>(rings)   - 1.0f)};
        auto S {1.0f / (static_cast<float>(sectors) - 1.0f)};

        for (auto r = 0; r != rings; ++r) for (auto s = 0; s != sectors; ++s) {
            const auto THETA {static_cast<float>(r * R)};
            const auto PHI   {static_cast<float>(s * S)};

            const auto x {std::cos(TAU * PHI) * std::sin(glm::pi<float>() * THETA)};
            const auto y {std::sin(-glm::pi<float>() / 2.0f + glm::pi<float>() * THETA)};
            const auto z {std::sin(TAU * PHI) * std::sin(glm::pi<float>() * THETA)};

            m_vertice.push_back({x, y, z, PHI, THETA});

            const std::initializer_list<unsigned int> indices {
                static_cast<unsigned int>(sectors *  r      + s),
                static_cast<unsigned int>(sectors *  r      + (s + 1)),
                static_cast<unsigned int>(sectors * (r + 1) + (s + 1)),
                static_cast<unsigned int>(sectors * (r + 1) + s)
            };

            m_indices.insert(m_indices.end(), indices);
        }

        mesh_setup();
    }

    ~sphere() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers     (1, &m_VBO);
        glDeleteBuffers     (1, &m_EBO);
    }

    void draw(const shader::shader_program &shader, const unsigned int &texture, camera::camera &cam) const {
        glCullFace(GL_FRONT);

        glm::mat4 model {1.0f};

        const auto angle {
            glm::radians(45.0f * static_cast<float>(glfwGetTime()))
        };

        model = glm::rotate(model, angle, glm::tvec3<float>(0.0f, 1.0f, 0.0f));

        shader.use     ();
        shader.set_mat4("model", model);
        shader.set_mat4("view", cam.get_view_matrix());
        shader.set_mat4("projection", cam.get_projection_matrix());

        glBindTexture    (GL_TEXTURE_2D, texture);
        glBindVertexArray(m_VAO);
        glDrawElements   (GL_QUADS, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

protected:
    unsigned int m_VAO {0u};
    unsigned int m_VBO {0u};
    unsigned int m_EBO {0u};

    std::vector<vertex_3d>    m_vertice;
    std::vector<unsigned int> m_indices;

    void mesh_setup() {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers     (1, &m_VBO);
        glGenBuffers     (1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertice.size() * 5 * sizeof(float), &m_vertice.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer    (0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer    (1, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        if (m_VAO == 0u) my_exception {__FILE__, __LINE__, "falha ao criar VAO do 'chunk'"};

        glBindVertexArray(0);
    }
};

}

#endif
