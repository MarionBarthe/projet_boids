#pragma once
#include "p6/p6.h"

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
    glm::vec2 m_position;
    glm::vec2 m_velocity;

    glm::vec2 align(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec2 cohesion(const std::vector<Boid>& boids, float radius_awareness);
    glm::vec2 separate(const std::vector<Boid>& boids, float radius_awareness);

public:
    Boid()
        : m_position(p6::random::point())
        , m_velocity(p6::random::point(glm::vec2(-0.01, -0.01), glm::vec2(0.01, 0.01))){};
    void update(p6::Context* ctx, const std::vector<Boid>& boids, Coeffs coeffs);
    void draw(p6::Context& ctx, float radius_awareness);
};
