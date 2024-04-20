#include "game_object.hpp"
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(const std::string& model_path, const std::string& texture_path)
    : model_matrix(1.0f)
{
    load_model(model_path);
    // load_texture(texture_path);
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
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture_object);
    // Using VAO and VBO objects
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
    vao.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
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
