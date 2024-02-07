#include "boid.hpp"

void Boid::update(p6::Context* ctx, std::vector<Boid> boids)
{
    align(boids);
    position += velocity;

    // gère les bords
    // pour que les boids ne sortent pas de la fenetre
    if (position.x < -ctx->aspect_ratio())
        position.x = ctx->aspect_ratio();
    if (position.y < -1)
        position.y = 1;
    if (position.x > ctx->aspect_ratio())
        position.x = -ctx->aspect_ratio();
    if (position.y > 1)
        position.y = -1;
};

void Boid::draw(p6::Context& ctx)
{
    ctx.fill = {p6::NamedColor::Pink};
    ctx.circle(p6::Center{position}, p6::Radius{0.05});
}

void Boid::align(const std::vector<Boid> boids)
{
    glm::vec2 sumVel(0., 0.);

    float awareness = 0.2;
    int   count     = 0;
    for (auto& b : boids)
    {
        if (glm::distance(b.position, this->position) < awareness)
        {
            sumVel += b.velocity;
            count++;
        }
    }
    sumVel /= count;
    velocity = sumVel;
}