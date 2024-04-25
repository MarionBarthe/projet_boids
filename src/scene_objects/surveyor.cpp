#include "surveyor.hpp"

Surveyor::Surveyor(GameObject* linked_game_object, std::vector<std::vector<double>> transition_matrix, std::vector<double> initial_state)
    : m_surveyor_object(linked_game_object), m_feelings_chain(transition_matrix, initial_state)
{
    m_light_intensity = glm::vec3(.01f, .01f, .01f);
}

void Surveyor::move_surveyor(p6::Context& ctx)
{
    float     speed = 0.05f;
    glm::vec3 movement(0.0f);
    // std::cout << "yolo\n";

    movement.z += ctx.key_is_pressed(GLFW_KEY_S) ? speed : 0;
    movement.z -= ctx.key_is_pressed(GLFW_KEY_W) ? speed : 0;
    movement.x += ctx.key_is_pressed(GLFW_KEY_D) ? speed : 0;
    movement.x -= ctx.key_is_pressed(GLFW_KEY_A) ? speed : 0;
    movement.y += ctx.key_is_pressed(GLFW_KEY_UP) ? speed : 0;
    movement.y -= ctx.key_is_pressed(GLFW_KEY_DOWN) ? speed : 0;

    m_surveyor_object->move(movement);
}

std::pair<glm::vec3, glm::vec3> Surveyor::calculate_wiggle_offsets(p6::Context& ctx)
{
    glm::vec3 pos_offset(
        sin(ctx.time() / m_period_position) * m_amplitude_position,
        cos(ctx.time() / m_period_position * 1.25) * m_amplitude_position,
        sin(ctx.time() / m_period_position * 1.5) * m_amplitude_position
    );

    glm::vec3 rot_offset(
        sin(ctx.time() / m_period_rotation) * m_amplitude_rotation,
        cos(ctx.time() / m_period_rotation) * m_amplitude_rotation,
        sin(ctx.time() / m_period_rotation * 1.5) * m_amplitude_rotation
    );

    return {pos_offset, rot_offset};
}

void Surveyor::move_surveyor_with_wiggle(p6::Context& ctx)
{
    move_surveyor(ctx);
    auto [pos_offset, rot_offset] = calculate_wiggle_offsets(ctx);
    m_surveyor_object->set_position(m_surveyor_object->get_position() + pos_offset);
    m_surveyor_object->set_rotation(m_surveyor_object->get_rotation() + rot_offset);
}

void Surveyor::next_feeling()
{
    // Perform transitions
    m_feelings_chain.transition_values();
    m_feelings_chain.display_state_counts();
    // Display the current state and count of states every 10 transitions
    if (m_feelings_chain.get_number_of_states() % 10 == 0)
    {
        print_container(m_feelings_chain.calculate_stationary_distribution());
    }
}
void Surveyor::adapt_feeling()
{
    switch (m_feelings_chain.get_deterministic_current_state())
    {
    case 0:                                                                                                     // Happy
        m_surveyor_object->interpolate_material_factors({0.7f, 0.7f, 0.7f}, {0.8f, 0.8f, 0.8f}, 100.0f, 0.05f); // Slightly shiny stone
        m_light_intensity = glm::mix(m_light_intensity, glm::vec3(2.5f, 2.5f, 2.0f), 0.1f);                     // Warm, bright light
        break;
    case 1:                                                                                                      // Sad
        m_surveyor_object->interpolate_material_factors({0.5f, 0.5f, 0.55f}, {0.4f, 0.4f, 0.45f}, 30.0f, 0.05f); // Duller, slightly bluish stone
        m_light_intensity = glm::mix(m_light_intensity, glm::vec3(1.0f, 1.0f, 2.0f), 0.1f);                      // Dimmer, cooler light
        break;
    case 2:                                                                                                    // Angry
        m_surveyor_object->interpolate_material_factors({0.6f, 0.3f, 0.3f}, {0.6f, 0.3f, 0.3f}, 50.0f, 0.05f); // Darker, reddish stone
        m_light_intensity = glm::mix(m_light_intensity, glm::vec3(3.0f, 0.5f, 0.5f), 0.1f);                    // Reddish, intense light
        break;
    case 3:                                                                                                    // Scared
        m_surveyor_object->interpolate_material_factors({0.4f, 0.4f, 0.5f}, {0.5f, 0.5f, 0.6f}, 25.0f, 0.05f); // Darker, slightly purplish stone
        m_light_intensity = glm::mix(m_light_intensity, glm::vec3(1.5f, 1.5f, 2.5f), 0.1f);                    // Dim, purple light
        break;
    default:                                                                                                     // Relaxed
        m_surveyor_object->interpolate_material_factors({0.6f, 0.65f, 0.7f}, {0.7f, 0.75f, 0.8f}, 70.0f, 0.05f); // Calm, slightly bluish stone
        m_light_intensity = glm::mix(m_light_intensity, glm::vec3(1.0f, 2.0f, 2.5f), 0.1f);                      // Soft, blue-green light
        break;
    }
}