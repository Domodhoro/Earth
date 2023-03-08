#ifndef SHADER_HPP
#define SHADER_HPP

namespace shader {

struct shader_program {
    shader_program(const char *vertex_path, const char *fragment_path) : shader {glCreateProgram()} {
        glAttachShader(shader, compile_shader_data(vertex_path, GL_VERTEX_SHADER));
        glAttachShader(shader, compile_shader_data(fragment_path, GL_FRAGMENT_SHADER));
        glLinkProgram (shader);

        auto success {0};

        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (!success) error_log(__FILE__, __LINE__, "falha ao compilar 'shader'");
    }

    ~shader_program() { glDeleteProgram(shader); }

    void use      ()                                                const { glUseProgram(shader); }
    void set_bool (const char *name, bool value)                    const { glUniform1i       (glGetUniformLocation(shader, name), static_cast<int>(value)); }
    void set_int  (const char *name, int value)                     const { glUniform1i       (glGetUniformLocation(shader, name), value); }
    void set_float(const char *name, float value)                   const { glUniform1f       (glGetUniformLocation(shader, name), value); }
    void set_vec2 (const char *name, const glm::tvec2<float> value) const { glUniform2fv      (glGetUniformLocation(shader, name), 1, glm::value_ptr(value)); }
    void set_vec3 (const char *name, const glm::tvec3<float> value) const { glUniform3fv      (glGetUniformLocation(shader, name), 1, glm::value_ptr(value)); }
    void set_mat4 (const char *name, const glm::mat4 matrix)        const { glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, false, glm::value_ptr(matrix)); }

protected:
    unsigned int shader {0u};

    unsigned int compile_shader_data(const char *data_path, const GLenum type) {
        auto data_code   {read_file(data_path)};
        auto shader_code {data_code.c_str()};
        auto data        {glCreateShader(type)};

        glShaderSource (data, 1, &shader_code, nullptr);
        glCompileShader(data);

        auto success {0};

        glGetShaderiv(data, GL_COMPILE_STATUS, &success);

        if (success == 0u) {
            if (type == GL_VERTEX_SHADER)   error_log(__FILE__, __LINE__, "falha ao compilar 'vertex'");
            if (type == GL_FRAGMENT_SHADER) error_log(__FILE__, __LINE__, "falha ao compilar 'fragment'");
        }

        return data;
    }

    std::string read_file(const char *file_path) const {
        std::ifstream file     {file_path};
        std::stringstream code {};

        if (!file.is_open()) error_log(__FILE__, __LINE__, "falha ao abrir arquivo 'GLSL'");

        code << file.rdbuf();

        file.close();

        return code.str();
    }
};

}

#endif
