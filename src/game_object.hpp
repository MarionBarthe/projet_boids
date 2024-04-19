#pragma once

#include <glm/glm.hpp>
#include <string>
#include "model_loader.hpp"
#include "texture_manager.hpp"
#include "vao.hpp"
#include "vbo.hpp"

class GameObject {
public:
    GameObject(const std::string& modelPath, const std::string& texturePath);
    ~GameObject();

    void      setPosition(const glm::vec3& position);
    void      setRotation(const glm::vec3& rotation);
    void      setScale(float scale);
    glm::mat4 getModelMatrix() const { return m_modelMatrix; }
    void      draw() const;

private:
    int       m_dataSize;
    VAO       m_vao;
    VBO       m_vboVertices;
    GLuint    m_textureObject;
    glm::mat4 m_modelMatrix;

    void loadModel(const std::string& modelPath);
    void loadTexture(const std::string& texturePath);
};
