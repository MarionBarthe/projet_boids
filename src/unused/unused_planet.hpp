#pragma once

#include <vector>
#include "glm/fwd.hpp"
#include "maths/random_generator.hpp"
#include "render/game_object.hpp"

class Planet {
public:
    Planet();

    ~Planet();

    void orbit(p6::Context& ctx);
    std::vector<Planet> static create_planets();

    GameObject* get_game_object() const { return m_planet_object; }

private:
    GameObject* m_planet_object;

    void set_texture();
    void place();
    void set_scale();
    void set_rotation_axes();

    std::vector<double> rotation_axes;
};
