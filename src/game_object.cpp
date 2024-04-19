#include "game_object.hpp"
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(const std::string& modelPath, const std::string& texturePath)
    : m_modelMatrix(1.0f)
{
    loadModel(modelPath);
    // loadTexture(texturePath);
}

GameObject::~GameObject()
{
    // Aucune ressource à libérer explicitement car les objets VAO et VBO sont gérés automatiquement
}

void GameObject::setPosition(const glm::vec3& position)
{
    m_modelMatrix = glm::translate(glm::mat4(1.0f), position);
}

void GameObject::setRotation(const glm::vec3& rotation)
{
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void GameObject::setScale(float scale)
{
    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(scale));
}

void GameObject::draw() const
{
    m_vao.bind();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_textureObject);
    //  Utilisation des objets VAO et VBO
    glDrawArrays(GL_TRIANGLES, 0, m_dataSize);
    m_vao.unbind();
}

void GameObject::loadModel(const std::string& modelPath)
{
    // Charger le modèle à partir du chemin du fichier
    std::cout << "Loading model from: " << modelPath << std::endl;

    ModelLoader::Model model = ModelLoader::loadModel(modelPath);

    m_vao.bind();
    m_vboVertices.bind();
    m_vboVertices.fill(model.vertices.data(), model.vertices.size() * sizeof(float), GL_STATIC_DRAW);
    m_vao.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
    m_vboVertices.unbind();
    m_vao.unbind();

    // Stocker la taille des données pour l'utiliser dans glDrawArrays
    m_dataSize = model.vertices.size() / 3;
}

void GameObject::loadTexture(const std::string& texturePath)
{
    // Charger la texture à partir du chemin du fichier
    std::cout << "Loading texture from: " << texturePath << std::endl;

    m_textureObject = TextureManager::loadTexture(texturePath);
}
