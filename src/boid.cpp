#include "boid.hpp"


void Boid::update(p6::Context* ctx)
{
    position += velocity;
    if (position.x < -ctx->aspect_ratio())
        position.x = ctx->aspect_ratio();
    if (position.y < -1)
        position.y = 1;
    if (position.x > ctx->aspect_ratio())
        position.x = -ctx->aspect_ratio();
    if (position.y > 1)
        position.y = -1;
};

void Boid::draw(p6::Context* ctx)
{
    ctx->fill = {p6::NamedColor::Pink};
    ctx->circle(p6::Center{position}, p6::Radius{radius});
}
