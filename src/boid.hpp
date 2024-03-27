#pragma once

#include "glm/gtc/random.hpp"
#include "p6/p6.h"

struct Cube {
    float width  = 4.0f;
    float height = 4.0f;
    float depth  = 4.0f;
}; 

struct Coeffs {
    float square_length    = 0.8;
    float radius_awareness = 0.2;
    float separate         = 0.02;
    float align            = 0.05;
    float cohesion         = 0.05;

    void draw_Gui()
    {
        ImGui::SliderFloat("Align", &align, 0.0f, .1f);
        ImGui::SliderFloat("Cohesion", &cohesion, 0.0f, .1f);
        ImGui::SliderFloat("Separate", &separate, 0.0f, .1f);
        ImGui::SliderFloat("Radius of awareness", &radius_awareness, 0.0f, 1.f);
        ImGui::SliderFloat("Length of the borders", &square_length, 0.0f, 1.f);
    }
};
class Boid {
private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;

    glm::vec3 align(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 cohesion(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 separate(const std::vector<Boid>& boids, float radius_awareness);

public:
    glm::vec3 random_position()
    {
        // return static_cast<float>(rand()) / RAND_MAX;
        return {glm::linearRand(- 2.0f, 2.0f), glm::linearRand(-2.0f,  2.0f), glm::linearRand( 2.0f, 2.0f)};
    }

    Boid()
        : m_position(random_position())
        , m_velocity(glm::vec3(random_position())){};
    void      update(p6::Context* ctx, const std::vector<Boid>& boids, Coeffs coeffs);
    void      draw(p6::Context& ctx, float radius_awareness);
    glm::vec3 get_position() const
    {
        return m_position;
    };
};
