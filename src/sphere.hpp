#ifndef SPHERE_HPP
#define SPHERE_HPP

namespace sphere {

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

            vertice.push_back(vertex<float>{{x, y, z}, {PHI, THETA}});

            const std::initializer_list<unsigned int> indice {
                static_cast<unsigned int>(sectors * (r + 0) + (s + 0)),
                static_cast<unsigned int>(sectors * (r + 0) + (s + 1)),
                static_cast<unsigned int>(sectors * (r + 1) + (s + 0)),
                static_cast<unsigned int>(sectors * (r + 1) + (s + 0)),
                static_cast<unsigned int>(sectors * (r + 0) + (s + 1)),
                static_cast<unsigned int>(sectors * (r + 1) + (s + 1))
            };

            this->indice.insert(this->indice.end(), indice);
        }

        mesh_setup();
    }

    ~sphere() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers     (1, &VBO);
        glDeleteBuffers     (1, &EBO);
    }

    void draw(const glm::mat4 &model, const shader::shader_program &shader, const unsigned int &texture, camera::camera &cam) const {
        glCullFace(GL_FRONT);

        shader.use     ();
        shader.set_mat4("Model", model);
        shader.set_mat4("View", cam.get_view_matrix());
        shader.set_mat4("Projection", cam.get_projection_matrix());

        glBindTexture    (GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements   (GL_TRIANGLES, indice.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

protected:
    unsigned int VAO {0u};
    unsigned int VBO {0u};
    unsigned int EBO {0u};

    std::vector<vertex<float>> vertice;
    std::vector<unsigned int>  indice;

    void mesh_setup() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers     (1, &VBO);
        glGenBuffers     (1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertice.size() * 5 * sizeof(float), &vertice.at(0), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice.size() * sizeof(unsigned int), &indice.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer    (0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer    (1, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        if (VAO == 0u) error_log(__FILE__, __LINE__, "falha ao criar VAO do 'chunk'");

        glBindVertexArray(0);
    }
};

}

#endif
