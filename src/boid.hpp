#pragma once
#include "p6/p6.h"

class Boid {
private:
    glm::vec2 position;
    glm::vec2 velocity;
    float     radius = 0.05;

public:
    Boid()
        : position(p6::random::point()), velocity(p6::random::point(glm::vec2(0., 0.), glm::vec2(0.01, 0.01))){};
    void update(p6::Context* ctx);
    void draw(p6::Context* ctx);
};
