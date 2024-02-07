#pragma once
#include "p6/p6.h"

class Boid {
private:
    glm::vec2 position;
    glm::vec2 velocity;

public:
    Boid()
        : position(p6::random::point())
        , velocity(p6::random::point(glm::vec2(0., 0.), glm::vec2(0.01, 0.01))){};
    void update(p6::Context* ctx, std::vector<Boid> boids);
    void draw(p6::Context& ctx);
    void align(const std::vector<Boid> boids);
    void separate(const std::vector<Boid> boids);
};
