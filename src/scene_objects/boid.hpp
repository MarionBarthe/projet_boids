#pragma once

#include "maths/color.hpp"
#include "p6/p6.h"
#include "maths/random_generator.hpp"

struct BoidVariables {
    float cube_length      = 10.4;
    float radius_awareness = 3.;
    float separate         = 0.5;
    float align            = 0.5;
    float cohesion         = 0.5;
    bool  isLowPoly        = true;

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
    Color     m_color;

public:
    Boid()
        : m_position(static_cast<float>(uniform_distribution(-2., 2.)))
        , m_velocity(static_cast<float>(uniform_distribution(-4., 4.)))
        , m_color(generate_vivid_color()){};

    glm::vec3 get_position() const { return m_position; };
    Color     get_color() const { return m_color; }

    void      update(p6::Context* ctx, const std::vector<Boid>& boids, BoidVariables variables);
    glm::vec3 align(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 cohesion(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec3 separate(const std::vector<Boid>& boids, float radius_awareness);
};
