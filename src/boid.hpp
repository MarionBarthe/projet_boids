#pragma once

#include "glm/gtc/random.hpp"
#include "p6/p6.h"

struct BoidVariables {
    float cube_length    = 10.4;
    float radius_awareness = 5.;
    float separate         = 0.2;
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
    glm::vec3 m_velocity;

    glm::vec3 align(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 cohesion(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 separate(const std::vector<Boid>& boids, float radius_awareness);

public:
    glm::vec3 random_position(float range)
    {
        // return static_cast<float>(rand()) / RAND_MAX;
        return {glm::linearRand(- range /2.f, range/2.f), glm::linearRand(- range /2.f, range/2.f), glm::linearRand( - range /2.f, range/2.f)};
    }

    Boid()
        : m_position(random_position(4.f))
        , m_velocity(glm::vec3(random_position(8.f))){};
    void      update(p6::Context* ctx, const std::vector<Boid>& boids, BoidVariables variables);
    void      draw(p6::Context& ctx, float radius_awareness);
    glm::vec3 get_position() const
    {
        return m_position;
    };
};
