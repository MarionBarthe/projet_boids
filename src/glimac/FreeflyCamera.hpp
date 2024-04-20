#ifndef FREEFLY_CAMERA_HPP
#define FREEFLY_CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class FreeflyCamera {
private:
    glm::vec3 m_Position;
    float     m_Phi;
    float     m_Theta;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;

public:
    FreeflyCamera()
        : m_Position(glm::vec3(0.0f)), m_Phi(glm::pi<float>()), m_Theta(0.0f)
    {
        computeDirectionVectors();
    }

    void moveLeft(float t) { m_Position += t * m_LeftVector; }

    void moveFront(float t) { m_Position += t * m_FrontVector; }

    void rotateLeft(float degrees)
    {
        m_Theta += glm::radians(degrees);
        computeDirectionVectors();
    }

    void rotateUp(float degrees)
    {
        m_Phi += glm::radians(degrees);
        computeDirectionVectors();
    }

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 target = m_Position + m_FrontVector;
        return glm::lookAt(m_Position, target, m_UpVector);
    }

private:
    void computeDirectionVectors()
    {
        m_FrontVector = glm::vec3(cos(m_Phi) * sin(m_Theta), sin(m_Phi), cos(m_Phi) * cos(m_Theta));

        m_LeftVector = glm::vec3(sin(m_Theta - glm::half_pi<float>()), 0.0f, cos(m_Theta - glm::half_pi<float>()));

        m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
    }
};

#endif // FREEFLY_CAMERA_HPP
