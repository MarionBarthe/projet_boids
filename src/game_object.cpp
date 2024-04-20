#include "game_object.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"

GameObject::GameObject(const std::string& model_path, const std::string& texture_path)
    : use_texture(true), model_matrix(1.0f)
{
    load_model(model_path);
    load_texture(texture_path);
}

GameObject::GameObject(const std::string& model_path, const glm::vec3& color)
    : use_texture(false), model_matrix(1.0f), base_color(color)
{
    load_model(model_path);
}

GameObject::~GameObject()
{
    // No resources to explicitly free since VAO and VBO objects are managed automatically
}

void GameObject::set_position(const glm::vec3& position)
{
    model_matrix = glm::translate(glm::mat4(1.0f), position);
}

void GameObject::set_rotation(const glm::vec3& rotation)
{
    model_matrix = glm::rotate(model_matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void GameObject::set_scale(float scale)
{
    model_matrix = glm::scale(model_matrix, glm::vec3(scale));
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

    vao.bind();
    vbo_vertices.bind();

    vbo_vertices.fill(model.vertices.data(), model.vertices.size() * sizeof(float), GL_STATIC_DRAW);

    vao.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);                   // Position attribute
    vao.specify_attribute(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float))); // Normal attribute
    vao.specify_attribute(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float))); // Texture coordinate attribute

    vbo_vertices.unbind();
    vao.unbind();

    // Store the data size for use in glDrawArrays
    data_size = model.vertices.size() / 3;
}

void GameObject::load_texture(const std::string& texture_path)
{
    // Load texture from file path
    std::cout << "Loading texture from: " << texture_path << std::endl;

    texture_object = TextureManager::load_texture(texture_path);
}
