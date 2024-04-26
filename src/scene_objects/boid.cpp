#include "boid.hpp"
#include "cmath"
#include "glm/gtx/norm.hpp"

static glm::vec3 limit(glm::vec3 force)
{
    float norm = glm::length(force);
    float max  = 0.03f;
    if (norm > max)
    {
        force *= max / norm;
    }
    return force;
}

static float manage_edge_collision(float position, float edge, float edge_offset)
{
    if (position < -edge)
        position = edge - edge_offset;
    if (position > edge)
        position = -(edge - edge_offset);
    return position;
}

static glm::vec3 random_position(float min, float max)
{
    float x = static_cast<float>(uniform_distribution(min, max));
    float y = static_cast<float>(uniform_distribution(min, max));
    float z = static_cast<float>(uniform_distribution(min, max));
    return {x, y, z};
}

void Boid::update(p6::Context* ctx, const std::vector<Boid>& boids, BoidVariables variables)
{
    glm::vec3 acceleration{0.f};

    acceleration += cohesion(boids, variables.radius_awareness) * variables.cohesion;
    acceleration += align(boids, variables.radius_awareness) * variables.align;
    acceleration += separate(boids, variables.radius_awareness) * variables.separate;

    m_velocity += acceleration;
    m_velocity = limit(m_velocity);
    m_position += m_velocity;

    // to keep the boids inside the cube
    float edge_offset = 4.;
    m_position.x      = manage_edge_collision(m_position.x, variables.cube_length, edge_offset);
    m_position.y      = manage_edge_collision(m_position.y, variables.cube_length, edge_offset);
    m_position.z      = manage_edge_collision(m_position.z, variables.cube_length, edge_offset);
}

glm::vec3 Boid::align(const std::vector<Boid>& boids, float radius_awareness)
{
    glm::vec3 target(0.f);
    int       count = 0;

    for (const auto& b : boids)
    {
        if (sqrt(glm::distance2(b.m_position, this->m_position)) < radius_awareness)
        {
            target += b.m_velocity;
            count++;
        }
    }
    if (count > 0)
    {
        target /= count;
        target = limit(target);
    }
    return target;
}

glm::vec3 Boid::separate(const std::vector<Boid>& boids, float radius_awareness)
{
    glm::vec3 target(0.f);

    int count = 0;
    for (const Boid& other : boids)
    {
        float distance = sqrt(glm::distance2(m_position, other.m_position));
        if (&other != this && distance < radius_awareness)
        {
            glm::vec3 diff = m_position - other.m_position;
            diff /= distance * distance;
            target += diff;
            count++;
        }
    }
    if (count == 0)
        return target;

    target /= count;
    glm::vec3 force = target - m_velocity;
    force           = limit(force);
    return force;
}

glm::vec3 Boid::cohesion(const std::vector<Boid>& boids, float radius_awareness)
{
    int count = 0;

    glm::vec3 target(0.f);
    for (const auto& b : boids)
    {
        if (sqrt(glm::distance2(m_position, b.m_position)) < radius_awareness)
        {
            target += b.m_position;
            count++;
        }
    }
    target = (count > 0) ? (target / static_cast<float>(count)) : target;
    target = limit(target);
    return target;
}