#ifndef TRACKBALL_CAMERA_HPP
#define TRACKBALL_CAMERA_HPP

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

class TrackballCamera {
private:
    float m_Distance;
    float m_AngleX;
    float m_AngleY;

public:
    TrackballCamera()
        : m_Distance(5.0f), m_AngleX(0.0f), m_AngleY(0.0f) {}

    void move_front(float delta)
    {
        m_Distance += delta;
    }

    void rotate_left(float degrees)
    {
        m_AngleY += glm::radians(degrees);
    }

    void rotate_up(float degrees)
    {
        m_AngleX += glm::radians(degrees);
    }

    glm::mat4 get_view_matrix() const
    {
        glm::mat4 viewMatrix(1.0f);

        viewMatrix = glm::rotate(viewMatrix, m_AngleX, glm::vec3(1.0f, 0.0f, 0.0f));
        viewMatrix = glm::rotate(viewMatrix, m_AngleY, glm::vec3(0.0f, 1.0f, 0.0f));
        viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -m_Distance));
        return viewMatrix;
    }
};

#endif // TRACKBALL_CAMERA_HPP
