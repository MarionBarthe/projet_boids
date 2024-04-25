#pragma once

#include <vector>
#include "game_object.hpp"
#include "markov_chain.hpp"
#include "p6/p6.h"

class Surveyor {
public:
    Surveyor(GameObject* linked_game_object, std::vector<std::vector<double>> transition_matrix, std::vector<double> initial_state);

    void move_surveyor_with_wiggle(p6::Context& ctx);
    void next_feeling();
    void adapt_feeling();

    glm::vec3 get_light_intensity() const { return m_light_intensity; }

private:
    GameObject* m_surveyor_object;
    MarkovChain m_feelings_chain;

    constexpr static const float m_speed = 2.0;

    constexpr static const float m_amplitude_position = 0.01f;
    constexpr static const float m_period_position    = 2.0f;

    constexpr static const float m_amplitude_rotation = 0.25f;
    constexpr static const float m_period_rotation    = 5.0f;

    glm::vec3 m_light_intensity;

    void move_surveyor(p6::Context& ctx);

    static std::pair<glm::vec3, glm::vec3> calculate_wiggle_offsets(p6::Context& ctx);
};
