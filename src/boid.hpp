#pragma once
#include "p6/p6.h"

const float radius = 0.3;
const float sep_coeff = 0.02;
const float ali_coeff = 0.05;
const float coh_coeff = 0.05;

class Boid {
private:
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 acceleration;

public:
    Boid()
        : position(p6::random::point())
        , velocity(p6::random::point(glm::vec2(-0.01, -0.01), glm::vec2(0.01, 0.01)))
        , acceleration(glm::vec2(0., 0.)){};
    void update(p6::Context* ctx, const std::vector<Boid> &boids);
    void draw(p6::Context& ctx);
    glm::vec2 align(const std::vector<Boid> &boids);
    glm::vec2 cohesion(const std::vector<Boid> &boids);
    glm::vec2 separate(const std::vector<Boid> &boids);
    void apply_friction();
    void cap_speed();
    void applyForce(const glm::vec2 force);
};
