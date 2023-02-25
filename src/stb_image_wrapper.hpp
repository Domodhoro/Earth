#ifndef STB_IMAGE_WRAPPER_HPP
#define STB_IMAGE_WRAPPER_HPP

namespace stb_image_wrapper {

void load_window_icon(GLFWwindow *window, const char *icon_path) {
    GLFWimage img;

    img.pixels = stbi_load(icon_path, &img.width, &img.height, 0, 4);

    if (img.pixels == nullptr) my_exception {__FILE__, __LINE__, "falha ao carregar ícone da janela de visualização"};

    glfwSetWindowIcon(window, 1, &img);

    stbi_image_free(img.pixels);
}

unsigned int load_texture(const char *texture_path) {
    auto texture {0u};

    glGenTextures  (1, &texture);
    glBindTexture  (GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);

    auto width    {0};
    auto height   {0};
    auto channels {0};

    auto pixels {stbi_load(texture_path, &width, &height, &channels, 0)};

    if (pixels == nullptr) my_exception {__FILE__, __LINE__, "falha ao carregar textura"};

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);

    return texture;
}

unsigned int load_cube_map_texture(const std::vector<std::string> &faces) {
    auto texture {0u};

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(false);

    auto width    {0};
    auto height   {0};
    auto channels {0};

    for (size_t i = 0; i != faces.size(); ++i) {
        auto pixels {stbi_load(faces.at(i).c_str(), &width, &height, &channels, 0)};

        if (pixels == nullptr) my_exception {__FILE__, __LINE__, "falha ao abrir os arquivos de textura"};

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        stbi_image_free(pixels);
    }

    return texture;
}

}

#endif
