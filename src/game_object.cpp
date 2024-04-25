#include "game_object.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "texture_manager.hpp"
// #include "glm/gtc/type_ptr.hpp"

GameObject::GameObject(const std::string& model_path, const std::string& texture_path)
    : m_use_texture(true), m_position(glm::vec3(0.0f)), m_rotation(glm::vec3(0.0f)), m_scale(1.0f), m_3D_model(Model(model_path))
{
    load_texture(texture_path);
    update_model_matrix();
    set_factors({1.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}, 64.0f);
}

GameObject::GameObject(const std::string& model_path, const glm::vec3& color)
    : m_use_texture(false), m_base_color(color), m_position(glm::vec3(0.0f)), m_rotation(glm::vec3(0.0f)), m_scale(1.0f), m_3D_model(model_path)
{
    update_model_matrix();
    set_factors({1.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}, 64.0f);
}

void GameObject::set_position(const glm::vec3& new_position)
{
    m_position = new_position;
    update_model_matrix();
}

void GameObject::set_rotation(const glm::vec3& new_rotation)
{
    m_rotation = new_rotation;
    update_model_matrix();
}

void GameObject::set_scale(const glm::vec3& new_scale)
{
    m_scale = new_scale;
    update_model_matrix();
}

void GameObject::set_factors(const glm::vec3& new_diffuse, const glm::vec3& new_specular, float new_shininess)
{
    m_diffuse_factor   = new_diffuse;
    m_specular_factor  = new_specular;
    m_shininess_factor = new_shininess;
}

void GameObject::draw() const
{
    m_3D_model.draw();
}

void GameObject::load_texture(const std::string& texture_path)
{
    // Load texture from file path
    std::cout << "Loading texture from: " << texture_path << std::endl;

    m_texture_object = TextureManager::load_texture(texture_path);
}

void GameObject::change_texture(const std::string& texture_path)
{
    load_texture(texture_path);
}

void GameObject::change_color(const glm::vec3& color)
{
    m_base_color = color;
}

void GameObject::update_model_matrix()
{
    m_model_matrix = glm::translate(glm::mat4(1.0f), m_position);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void GameObject::move_x(const float offset)
{
    m_position.x += offset;
    update_model_matrix();
}

void GameObject::move_y(const float offset)
{
    m_position.y += offset;
    update_model_matrix();
}

void GameObject::move_z(const float offset)
{
    m_position.z += offset;
    update_model_matrix();
}

void GameObject::move(const glm::vec3 offset)
{
    m_position.x += offset.x;
    m_position.y += offset.y;
    m_position.z += offset.z;
    update_model_matrix();
}

void GameObject::interpolate_material_factors(const glm::vec3& target_diffuse, const glm::vec3& target_specular, float target_shininess, float blend_factor)
{
    m_diffuse_factor   = glm::mix(m_diffuse_factor, target_diffuse, blend_factor);
    m_specular_factor  = glm::mix(m_specular_factor, target_specular, blend_factor);
    m_shininess_factor = glm::mix(m_shininess_factor, target_shininess, blend_factor);
}