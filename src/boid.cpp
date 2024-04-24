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
static void edge_control(float position, float edge_limit)
{
    float edge_offset = 0.05;

    if (position <= -edge_limit)
        position = edge_limit - edge_offset;
    else if (position >= edge_limit)
        position = -(edge_limit - edge_offset);
}

void Boid::update(p6::Context* ctx, const std::vector<Boid>& boids, Boid_variables variables)
{
    glm::vec3 acceleration{0.f};

    acceleration += cohesion(boids, variables.radius_awareness) * variables.cohesion;
    acceleration += align(boids, variables.radius_awareness) * variables.align;
    acceleration += separate(boids, variables.radius_awareness) * variables.separate;

    m_velocity += acceleration;
    m_velocity = limit(m_velocity);
    m_position += m_velocity;

    // gère les bords pour que les boids ne sortent pas de la fenetre
    edge_control(m_position.x, variables.cube_length);
    edge_control(m_position.y, variables.cube_length);
    edge_control(m_position.z, variables.cube_length);
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