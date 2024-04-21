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

    void set_position(const glm::vec3& position);
    void set_rotation(const glm::vec3& rotation);
    void set_scale(float scale);
    void set_factors(const glm::vec3& newDiffuse, const glm::vec3& newSpecular, float newShininess);

    glm::mat4 get_model_matrix() const { return model_matrix; }
    glm::vec3 get_base_color() const { return base_color; }
    GLuint    get_texture() const { return texture_object; }
    bool      get_use_texture() const { return use_texture; }
    glm::vec3 get_diffuse_factor() const { return diffuse_factor; }
    glm::vec3 get_specular_factor() const { return specular_factor; }
    float     get_shininess_factor() const { return shininess_factor; }

    // Draw the object with shader support
    void draw() const;

private:
    int       data_size;
    VAO       vao;
    VBO       vbo_vertices;
    GLuint    texture_object;
    bool      use_texture;      // Indicates whether to use a texture
    glm::vec3 base_color;       // Base color if no texture is used
    glm::mat4 model_matrix;     // Transformation matrix of the object
    glm::vec3 diffuse_factor;   // Diffuse reflectivity
    glm::vec3 specular_factor;  // Specular reflectivity
    float     shininess_factor; // Shininess for specular highlight

    void load_model(const std::string& model_path);
    void load_texture(const std::string& texture_path);
};
