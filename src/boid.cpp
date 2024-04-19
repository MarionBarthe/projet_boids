#include "boid.hpp"
#include <iostream>
#include "cmath"
#include "glm/ext/quaternion_geometric.hpp"

static glm::vec3 limit(glm::vec3 force)
{
    float norm = glm::length(force);

    float max = 0.01f;
    // float min  = 0.01;

    if (norm > max)
    {
        force *= max / norm;
    }
    // else if (size<min){

    // }
    return force;
}

void Boid::update(p6::Context* ctx, const std::vector<Boid>& boids, Coeffs coeffs)
{
    glm::vec3 acceleration{0.f};

    acceleration += cohesion(boids, coeffs.radius_awareness) * coeffs.cohesion;
    acceleration += align(boids, coeffs.radius_awareness) * coeffs.align;
    acceleration += separate(boids, coeffs.radius_awareness) * coeffs.separate;

    m_velocity += acceleration;
    m_velocity = limit(m_velocity);
    m_position += m_velocity;

    // gère les bords pour que les boids ne sortent pas de la fenetre
    if (m_position.x <= -coeffs.square_length)
        m_position.x = coeffs.square_length - 0.005f;
    else if (m_position.x >= coeffs.square_length)
        m_position.x = -(coeffs.square_length - 0.005f);

    if (m_position.y <= -coeffs.square_length)
        m_position.y = coeffs.square_length - 0.005f;
    else if (m_position.y >= coeffs.square_length)
        m_position.y = -(coeffs.square_length - 0.005f);
};

void Boid::draw(p6::Context& ctx, float radius_awareness)
{
    ctx.stroke_weight = 0.01f;
    ctx.use_fill      = true;
    ctx.fill          = {p6::NamedColor::OrangeRed};
    ctx.circle(p6::Center{m_position}, p6::Radius{0.04});
    ctx.use_fill = false;
    ctx.stroke   = {1., 1., 1., 0.4};
    ctx.circle(p6::Center{m_position}, p6::Radius{radius_awareness});
}

glm::vec3 Boid::align(const std::vector<Boid>& boids, float radius_awareness)
{
    glm::vec3 target(0.f);
    int       count = 0;

    for (const auto& b : boids)
    {
        if (glm::distance(b.m_position, this->m_position) < radius_awareness)
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
        float distance = glm::distance(m_position, other.m_position);
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
        if ((glm::distance(m_position, b.m_position) < radius_awareness))
        {
            target += b.m_position;
            count++;
        }
    }
    target = (count > 0) ? (target / static_cast<float>(count)) : target;
    target = limit(target);
    return target;
}