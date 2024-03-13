#include "boid.hpp"
#include <iostream>
#include "glm/ext/quaternion_geometric.hpp"
#include "math.h"

glm::vec2 limit(glm::vec2 force)
{
    double norm = glm::length(force);

    double max = 0.01;
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
    // velocity = {0.00001, 0.00001};
    glm::vec2 coh, sep, ali;
    coh = cohesion(boids, coeffs.radius_awareness);
    ali = align(boids, coeffs.radius_awareness);
    sep = separate(boids, coeffs.radius_awareness);

    // les mult par des facteurs
    coh *= coeffs.coeff_cohesion;
    ali *= coeffs.coeff_align;
    sep *= coeffs.coeff_separate;
    // appliquer les forces
    applyForce(coh);
    applyForce(ali);
    applyForce(sep);

    acceleration = limit(acceleration);
    velocity += acceleration;
    velocity = limit(velocity);
    position += velocity;
    acceleration = glm::vec2(0., 0.); // remet l'acceleration à 0

    // gère les bords
    // pour que les boids ne sortent pas de la fenetre
    if (position.x <= -0.8)
        position.x = 0.799;
    else if (position.x >= 0.8)
        position.x = -0.799;

    if (position.y <= -0.8)
        position.y = 0.799;
    else if (position.y >= 0.8)
        position.y = -0.799;
};

void Boid::draw(p6::Context& ctx, float radius_awareness)
{
    ctx.stroke_weight = 0.01f;
    ctx.use_fill      = true;
    ctx.fill          = {p6::NamedColor::OrangeRed};
    ctx.circle(p6::Center{position}, p6::Radius{0.04});
    ctx.use_fill = false;
    ctx.circle(p6::Center{position}, p6::Radius{radius_awareness});
}

glm::vec2 Boid::align(const std::vector<Boid>& boids, float radius_awareness)
{
    glm::vec2 target(0., 0.);
    int       count = 0;

    for (auto& b : boids)
    {
        if (glm::distance(b.position, this->position) < radius_awareness)
        {
            target += b.velocity;
            count++;
        }
    }
    if (count > 0)
    {
        target /= count;
        // target = normalize(target);
        target = limit(target);
    }
    return target;
}

glm::vec2 Boid::separate(const std::vector<Boid>& boids, float radius_awareness)
{
    glm::vec2 target(0., 0.);

    int count = 0;
    for (const Boid& other : boids)
    {
        float distance = glm::distance(position, other.position);
        if (&other != this && distance < radius_awareness)
        {
            glm::vec2 diff = position - other.position;
            diff /= distance * distance;
            target += diff;
            count++;
        }
    }
    if (count == 0)
        return target;

    target /= count;
    glm::vec2 force = target - velocity;
    force = limit(force);
    return force;
}

glm::vec2 Boid::cohesion(const std::vector<Boid>& boids, float radius_awareness)
{
    int count = 0;

    glm::vec2 target(0, 0);
    for (const auto& b : boids)
    {
        float distance = glm::distance(position, b.position);
        if ((distance > 0) && (distance < radius_awareness)) // if dist -isneighbor <= theta  ?
        {
            target += b.position;
            count++;
        }
    }
    target = (count > 0) ? (target / static_cast<float>(count)) : target;
    target = limit(target);
    return target;
}

glm::vec2 normalize(glm::vec2 vec)
{
    if (vec.x == 0. && vec.y == 0.)
        return vec;
    float m = sqrt(vec.x * vec.x + vec.y * vec.y);

    return ((m > 0) ? vec / m : vec);
}

void Boid::applyForce(glm::vec2 force)
{
    acceleration += force;
}