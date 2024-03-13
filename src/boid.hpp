#pragma once
#include "p6/p6.h"

struct Coeffs {
    float radius_awareness = 0.2;
    float coeff_separate   = 0.02;
    float coeff_align      = 0.05;
    float coeff_cohesion   = 0.05;
};
class Boid {
private:
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 acceleration;

public:
    Boid()
        : position(p6::random::point())
        , velocity(p6::random::point(glm::vec2(-0.001, -0.001), glm::vec2(0.001, 0.001)))
        , acceleration(glm::vec2(0., 0.)){};
    void      update(p6::Context* ctx, const std::vector<Boid>& boids, Coeffs coeffs);
    void      draw(p6::Context& ctx, float radius_awareness);
    glm::vec2 align(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec2 cohesion(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec2 separate(const std::vector<Boid>& boids, float radius_awareness);
    void      applyForce(const glm::vec2 force);
};
