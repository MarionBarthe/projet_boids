#include "planet.hpp"

std::vector<Planet> Planet::create_planets()
{
    std::vector<Planet> planets;

    int number_of_planets = binomial_distribution_cdf(12, 0.5);
    for (int i = 0; i < number_of_planets; i++)
    {
        Planet new_planet;
        planets.push_back(new_planet);
    }

    return planets;
}

Planet::Planet()
    : m_planet_object(new GameObject("assets/models/planet.obj", glm::vec3(0.0f, 0.0f, 0.0f)))
{
    set_texture();
    place();
    set_scale();
    set_rotation_axes();
}

Planet::~Planet()
{
    m_planet_object = nullptr;
}

void Planet::set_texture()
{
    int texture_index = discrete_uniform_distribution(0, 6);
    switch (texture_index)
    {
    case 0:
        m_planet_object->change_texture("assets/textures/2k_jupiter.jpg");
        break;
    case 1:
        m_planet_object->change_texture("assets/textures/2k_mars.jpg");
        break;
    case 2:
        m_planet_object->change_texture("assets/textures/2k_neptune.jpg");
        break;
    case 3:
        m_planet_object->change_texture("assets/textures/2k_uranus.jpg");
        break;
    case 4:
        m_planet_object->change_texture("assets/textures/2k_mercury.jpg");
        break;
    case 5:
        m_planet_object->change_texture("assets/textures/2k_venus_atmosphere.jpg");
        break;
    default:
        m_planet_object->change_texture("assets/textures/2k_venus_surface.jpg");
        break;
    }
}

void Planet::place()
{
    float scale_cube = 180 * 0.1;

    glm::vec3 new_position;

    new_position.x = static_cast<float>(beta_distribution(1.0, 1.0) - 0.5) * scale_cube;
    new_position.y = static_cast<float>(beta_distribution(1.0, 1.0) - 0.5) * scale_cube;
    new_position.z = static_cast<float>(beta_distribution(1.0, 1.0) - 0.5) * scale_cube;

    m_planet_object->set_position(new_position);
}

void Planet::set_scale()
{
    glm::vec3 new_scale(static_cast<float>(uniform_distribution(0.8, 1.2)));

    m_planet_object->set_scale(new_scale);
}

void Planet::set_rotation_axes()
{
    normal_distribution(rotation_axes, 0, 0.5);
}

void Planet::orbit(p6::Context& ctx)
{
    m_planet_object->set_rotation({rotation_axes[0] * ctx.time(), rotation_axes[1] * ctx.time(), 0});
}