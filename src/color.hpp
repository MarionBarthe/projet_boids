#pragma once

#include <glm/glm.hpp>
#include "random_generator.hpp"

glm::vec3 hsv_to_rgb(float h, float s, float v);
glm::vec3 generate_vivid_color();