#include "game_object.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"

GameObject::GameObject(const std::string& model_path, const std::string& texture_path)
    : use_texture(true), position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), scale(1.0f)
{
    load_model(model_path);
    load_texture(texture_path);
    update_model_matrix();
    set_factors({1.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}, 64.0f);
}

GameObject::GameObject(const std::string& model_path, const glm::vec3& color)
    : use_texture(false), base_color(color), position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), scale(1.0f)
{
    load_model(model_path);
    update_model_matrix();
    set_factors({1.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}, 64.0f);
}

GameObject::~GameObject()
{
    // No resources to explicitly free since VAO and VBO objects are managed automatically
}

void GameObject::set_position(const glm::vec3& new_position)
{
    position = new_position;
    update_model_matrix();
}

void GameObject::set_rotation(const glm::vec3& new_rotation)
{
    rotation = new_rotation;
    update_model_matrix();
}

void GameObject::set_scale(const glm::vec3& new_scale)
{
    scale = new_scale;
    update_model_matrix();
}

void GameObject::set_factors(const glm::vec3& new_diffuse, const glm::vec3& new_specular, float new_shininess)
{
    diffuse_factor   = new_diffuse;
    specular_factor  = new_specular;
    shininess_factor = new_shininess;
}

void GameObject::draw() const
{
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, data_size);
    vao.unbind();
}

void GameObject::load_model(const std::string& model_path)
{
    // Load model from file path
    std::cout << "Loading model from: " << model_path << std::endl;

    ModelLoader::Model model = ModelLoader::load_model(model_path);

    vbo_vertices.bind();
    vbo_vertices.fill(model.combined_data.data(), model.combined_data.size() * sizeof(float), GL_STATIC_DRAW);

    vao.bind();
    int stride = 8 * sizeof(float);

    vao.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);                   // Position attribute
    vao.specify_attribute(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); // Normal attribute
    vao.specify_attribute(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float))); // Texture coordinate attribute
    vao.unbind();

    vbo_vertices.unbind();

    // Store the data size for use in glDrawArrays
    data_size = model.combined_data.size() / 8;
}

void GameObject::load_texture(const std::string& texture_path)
{
    // Load texture from file path
    std::cout << "Loading texture from: " << texture_path << std::endl;

    texture_object = TextureManager::load_texture(texture_path);
}

void GameObject::change_texture(const std::string& texture_path)
{
    load_texture(texture_path);
}

void GameObject::update_model_matrix()
{
    model_matrix = glm::translate(glm::mat4(1.0f), position);
    model_matrix = glm::rotate(model_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model_matrix = glm::rotate(model_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model_matrix = glm::rotate(model_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model_matrix = glm::scale(model_matrix, scale);
}

void GameObject::move_x(const float offset)
{
    position.x += offset;
    update_model_matrix();
}

void GameObject::move_y(const float offset)
{
    position.y += offset;
    update_model_matrix();
}

void GameObject::move_z(const float offset)
{
    position.z += offset;
    update_model_matrix();
}

void GameObject::move(const glm::vec3 offset)
{
    position.x += offset.x;
    position.y += offset.y;
    position.z += offset.z;
    update_model_matrix();
}

void GameObject::interpolate_material_factors(const glm::vec3& target_diffuse, const glm::vec3& target_specular, float target_shininess, float blend_factor)
{
    diffuse_factor   = glm::mix(diffuse_factor, target_diffuse, blend_factor);
    specular_factor  = glm::mix(specular_factor, target_specular, blend_factor);
    shininess_factor = glm::mix(shininess_factor, target_shininess, blend_factor);
}