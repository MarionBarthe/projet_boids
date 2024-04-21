#pragma once

#include <glm/glm.hpp>
#include <string>
#include "glm/fwd.hpp"
#include "model_loader.hpp"
#include "p6/p6.h"
#include "texture_manager.hpp"
#include "vao.hpp"
#include "vbo.hpp"

class GameObject {
public:
    GameObject(const std::string& model_path, const std::string& texture_path);
    GameObject(const std::string& model_path, const glm::vec3& color);

    ~GameObject();

    void      set_position(const glm::vec3& position);
    void      set_rotation(const glm::vec3& rotation);
    void      set_scale(float scale);
    glm::mat4 get_model_matrix() const { return model_matrix; }
    glm::vec3 get_base_color() const { return base_color; }
    GLuint    get_texture() const { return texture_object; }
    bool      get_use_texture() const { return use_texture; }

    // Draw the object with shader support
    void draw() const;

private:
    int       data_size;
    VAO       vao;
    VBO       vbo_vertices;
    GLuint    texture_object;
    bool      use_texture;  // Indicates whether to use a texture
    glm::vec3 base_color;   // Base color if no texture is used
    glm::mat4 model_matrix; // Transformation matrix of the object

    void load_model(const std::string& model_path);
    void load_texture(const std::string& texture_path);
};
