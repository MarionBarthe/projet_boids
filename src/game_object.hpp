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

    void set_position(const glm::vec3& new_position);
    void set_rotation(const glm::vec3& new_rotation);
    void set_scale(const glm::vec3& new_scale);

    void move(const glm::vec3 offset);
    void move_x(const float offset);
    void move_y(const float offset);
    void move_z(const float offset);

    void set_factors(const glm::vec3& new_diffuse, const glm::vec3& new_specular, float new_shininess);

    glm::vec3 get_base_color() const { return base_color; }
    GLuint    get_texture() const { return texture_object; }
    bool      get_use_texture() const { return use_texture; }

    glm::vec3 get_position() const { return position; }
    glm::vec3 get_rotation() const { return rotation; }
    glm::vec3 get_scale() const { return scale; }
    glm::mat4 get_model_matrix() const { return model_matrix; }

    glm::vec3 get_diffuse_factor() const { return diffuse_factor; }
    glm::vec3 get_specular_factor() const { return specular_factor; }
    float     get_shininess_factor() const { return shininess_factor; }

    void interpolate_material_factors(const glm::vec3& target_diffuse, const glm::vec3& target_specular, float target_shininess, float blend_factor);

    void draw() const;

private:
    int data_size;
    VAO vao;
    VBO vbo_vertices;

    GLuint    texture_object;
    bool      use_texture; // Indicates whether to use a texture
    glm::vec3 base_color;  // Base color if no texture is used

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 model_matrix; // Transformation matrix of the object

    glm::vec3 diffuse_factor;   // Diffuse reflectivity
    glm::vec3 specular_factor;  // Specular reflectivity
    float     shininess_factor; // Shininess for specular highlight

    void load_model(const std::string& model_path);
    void load_texture(const std::string& texture_path);

    void update_model_matrix();
};
