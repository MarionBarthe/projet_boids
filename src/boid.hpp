#pragma once

#include "glm/gtc/random.hpp"
#include "p6/p6.h"

struct BoidVariables {
    float cube_length    = 10.4;
    float radius_awareness = 3.;
    float separate         = 0.5;
    float align            = 0.5;
    float cohesion         = 0.5;
    bool isLowPoly = true;

    void draw_Gui()
    {
        ImGui::SliderFloat("Align", &align, 0.0f, 1.f);
        ImGui::SliderFloat("Cohesion", &cohesion, 0.0f, 1.f);
        ImGui::SliderFloat("Separate", &separate, 0.0f, 1.f);
        ImGui::SliderFloat("Radius of awareness", &radius_awareness, 0.0f, 10.f);
        ImGui::Checkbox("Low Poly", &isLowPoly);
    }
};
class Boid {
private:
    glm::vec3 m_position;
    // GameObject game_object;
    glm::vec3 m_velocity;

    glm::vec3 align(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 cohesion(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 separate(const std::vector<Boid>& boids, float radius_awareness);

public:
    glm::vec3 random_position(float min, float max)
    {
        // return static_cast<float>(rand()) / RAND_MAX;
        return {glm::linearRand(min, max), glm::linearRand(min, max), glm::linearRand(min, max)};
    }

    Boid()
        : m_position(random_position(-2.f, 2.f)) //TODO utiliser fns de random_generator
        , m_velocity(glm::vec3(random_position(-4.f, 4.f))){};
    void      update(p6::Context* ctx, const std::vector<Boid>& boids, BoidVariables variables);
    void      draw(p6::Context& ctx, float radius_awareness);
    glm::vec3 get_position() const
    {
        return m_position;
    };
};
