#pragma once

#include <glm/glm.hpp>
#include <string>
#include "model_loader.hpp"
#include "texture_manager.hpp"
#include "vao.hpp"
#include "vbo.hpp"

class GameObject {
public:
    GameObject(const std::string& model_path, const std::string& texture_path);
    ~GameObject();

    void      set_position(const glm::vec3& position);
    void      set_rotation(const glm::vec3& rotation);
    void      set_scale(float scale);
    glm::mat4 get_model_matrix() const { return model_matrix; }
    void      draw() const;

private:
    int       data_size;
    VAO       vao;
    VBO       vbo_vertices;
    GLuint    texture_object;
    glm::mat4 model_matrix;

    void load_model(const std::string& model_path);
    void load_texture(const std::string& texture_path);
};
