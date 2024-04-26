#include <cstddef>
#include <cstdlib>
#include <vector>
#include "glimac/common.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glimac/trackball_camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "p6/p6.h"
#include "render/game_object.hpp"
#include "render/texture_manager.hpp"
#include "render/vao.hpp"
#include "render/vbo.hpp"
#include "scene_objects/boid.hpp"
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "maths/color.hpp"
#include "maths/markov_chain.hpp"
#include "maths/random_generator.hpp"
#include "render/program.hpp"
#include "scene_objects/surveyor.hpp"

struct Light {
    glm::vec3 position;  // Light position in view space
    glm::vec3 intensity; // Light intensity
};

int time_events(int next_event_time, Surveyor& chain, p6::Context& ctx)
{
    const double current_time = ctx.time();
    const double lambda       = 1.0 / 3; // Every 3 seconds

    if (current_time >= next_event_time)
    {
        chain.next_feeling();

        double delay    = exponential_distribution(lambda);
        next_event_time = current_time + delay;
        std::cout << "Next event in " << delay << " seconds." << std::endl;
    }
    return next_event_time;
}

void handle_camera_input(p6::Context& ctx, TrackballCamera& camera, float& last_x, float& last_y)
{
    ctx.mouse_dragged = [&](p6::MouseDrag drag) {
        float deltaX = drag.position.x - last_x;
        float deltaY = drag.position.y - last_y;

        if (last_x != 0 && last_y != 0)
        {
            camera.rotate_left(-deltaX * 25.f);
            camera.rotate_up(deltaY * 25.f);
        }

        last_x = drag.position.x;
        last_y = drag.position.y;
    };

    if (!ctx.mouse_button_is_pressed(p6::Button::Left))
    {
        last_x = 0;
        last_y = 0;
    }

    if (ctx.mouse_button_is_pressed(p6::Button::Right))
    {
        camera.reset_camera();
    }

    ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
        camera.move_front(-scroll.dy);
    };
}

int main()
{
    auto ctx = p6::Context{{1280, 720, "Space Boids - Barthe & Duval"}};
    ctx.maximize_window();
    glEnable(GL_DEPTH_TEST);

    // Seed the random number generator
    srand(time(NULL));

    TrackballCamera   camera;
    BoidVariables     coeffs;
    std::vector<Boid> boids(80);
    Program           boids_program{};
    Light             lights[2];

    double next_event_time = 0.0;

    GameObject thwomp_object("assets/models/thwomp.obj", "assets/textures/thwomp_texture.jpg");
    if (bernoulli_distribution(0.1))
    {
        thwomp_object.change_texture("assets/textures/thwomp_shiny_texture.jpg");
    }

    std::vector<std::vector<double>> transition_matrix = {
        {0.4, 0.1, 0.2, 0.1, 0.2}, // Happy to Happy, Sad, Angry, Scared, Relaxed
        {0.2, 0.3, 0.1, 0.1, 0.3}, // Sad to Happy, Sad, Angry, Scared, Relaxed
        {0.1, 0.2, 0.4, 0.1, 0.2}, // Angry to Happy, Sad, Angry, Scared, Relaxed
        {0.1, 0.2, 0.1, 0.4, 0.2}, // Scared to Happy, Sad, Angry, Scared, Relaxed
        {0.3, 0.1, 0.1, 0.1, 0.4}  // Relaxed to Happy, Sad, Angry, Scared, Relaxed
    };
    std::vector<double> initial_state = {0.2, 0.1, 0.1, 0.1, 0.5};

    Surveyor player(&thwomp_object, transition_matrix, initial_state);

    thwomp_object.set_position(glm::vec3(0.f, 0.f, -5.f));
    thwomp_object.set_scale(glm::vec3(0.25f, 0.25f, 0.25f));
    thwomp_object.set_factors({0.7f, 0.7f, 0.7f}, {0.8f, 0.8f, 0.8f}, 100.0f);

    GameObject thwomp_edge_object("assets/models/thwomp.obj", glm::vec3(.0f, .0f, .0f));
    thwomp_edge_object.set_scale(glm::vec3(0.26f, 0.26f, 0.26f));
    thwomp_edge_object.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 0.0f);

    GameObject star_boid("assets/models/star.obj", generate_vivid_color());
    star_boid.set_scale(glm::vec3(0.01f, 0.01f, 0.01f));
    star_boid.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 200.0f);

    GameObject star_boid_low("assets/models/star_low.obj", generate_vivid_color());
    star_boid_low.set_scale(glm::vec3(0.01f, 0.01f, 0.01f));
    star_boid_low.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 200.0f);

    GameObject star_edge_object("assets/models/star.obj", glm::vec3(.0f, .0f, .0f));
    star_edge_object.set_scale(glm::vec3(0.011f, 0.011f, 0.011f));
    star_edge_object.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 0.0f);

    GameObject space_object("assets/models/space.obj", "assets/textures/space_texture.jpg");
    space_object.set_scale(glm::vec3(0.1f, 0.1f, 0.1f));
    space_object.set_factors({0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, 75.0f);

    GameObject jupiter_object("assets/models/planet.obj", "assets/textures/2k_jupiter.jpg");
    GameObject mars_object("assets/models/planet.obj", "assets/textures/2k_mars.jpg");
    GameObject neptune_object("assets/models/planet.obj", "assets/textures/2k_neptune.jpg");
    GameObject mercury_object("assets/models/planet.obj", "assets/textures/2k_mercury.jpg");
    GameObject uranus_object("assets/models/planet.obj", "assets/textures/2k_uranus.jpg");
    GameObject venus_surface_object("assets/models/planet.obj", "assets/textures/2k_venus_surface.jpg");
    GameObject venus_atmosphere_object("assets/models/planet.obj", "assets/textures/2k_venus_atmosphere.jpg");
    GameObject planet_edge_object("assets/models/planet.obj", glm::vec3(.0f, .0f, .0f));
    planet_edge_object.set_scale({1.01f, 1.1f, 1.01f});
    planet_edge_object.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 0.0f);

    int                              number_of_planets = binomial_distribution_cdf(12, 0.5);
    std::vector<std::vector<double>> planet_positions_and_textures(number_of_planets);

    float scale_cube = 180 * 0.1;
    for (int i = 0; i < number_of_planets; i++)
    {
        double x = (beta_distribution(1.0, 1.0) - 0.5) * scale_cube;
        double y = (beta_distribution(1.0, 1.0) - 0.5) * scale_cube;
        double z = (beta_distribution(1.0, 1.0) - 0.5) * scale_cube;

        int texture_index = discrete_uniform_distribution(0, 6);

        std::vector<double> rotation_random;
        normal_distribution(rotation_random, 0, 0.5);
        planet_positions_and_textures[i] = std::vector<double>{x, y, z, static_cast<double>(texture_index), rotation_random[0] * 10, rotation_random[1] * 10};
    }

    float last_x = 0;
    float last_y = 0;

    glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);
    float     lightMotionRadius = 8.0f;
    float     lightMotionSpeed  = 0.5f;
    lights[0].intensity         = glm::vec3(2.0f, 2.0f, 2.0f);

    ctx.update = [&]() {
        next_event_time = time_events(next_event_time, player, ctx);

        ImGui::Begin("Boids command panel");
        ImGui::Text("Play with the parameters of the flock!");
        coeffs.draw_Gui();
        ImGui::End();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        player.move_surveyor_with_wiggle(ctx);
        camera.set_center(thwomp_object.get_position());
        handle_camera_input(ctx, camera, last_x, last_y);

        glm::mat4 view_matrix = camera.get_view_matrix();
        glm::mat4 proj_matrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        // Update light position
        float time      = ctx.time();
        lightPosition.x = sin(time * lightMotionSpeed) * lightMotionRadius;
        lightPosition.z = cos(time * lightMotionSpeed) * lightMotionRadius;

        lights[0].position = glm::vec3(view_matrix * glm::vec4(lightPosition, 1.0));

        thwomp_object.move_y(1.1f);
        lights[1].position = glm::vec3(view_matrix * glm::vec4(thwomp_object.get_position(), 1.0));
        thwomp_object.move_y(-1.1f);

        player.adapt_feeling();
        lights[1].intensity = player.get_light_intensity();

        boids_program.use();
        glUniform3fv(boids_program.u_light_pos_vs_0, 1, glm::value_ptr(lights[0].position));
        glUniform3fv(boids_program.u_light_intensity_0, 1, glm::value_ptr(lights[0].intensity));
        glUniform3fv(boids_program.u_light_pos_vs_1, 1, glm::value_ptr(lights[1].position));
        glUniform3fv(boids_program.u_light_intensity_1, 1, glm::value_ptr(lights[1].intensity));

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        for (auto& b : boids)
        {
            star_edge_object.set_position(b.get_position());
            boids_program.render_game_object(star_edge_object, view_matrix, proj_matrix);
        }

        thwomp_edge_object.set_position(thwomp_object.get_position());
        thwomp_edge_object.set_rotation(thwomp_object.get_rotation());
        boids_program.render_game_object(thwomp_edge_object, view_matrix, proj_matrix);

        boids_program.render_game_object(space_object, view_matrix, proj_matrix);

        for (const auto& planet : planet_positions_and_textures)
        {
            planet_edge_object.set_position({planet[0], planet[1], planet[2]});
            boids_program.render_game_object(planet_edge_object, view_matrix, proj_matrix);
        }

        glCullFace(GL_BACK);
        boids_program.render_game_object(thwomp_object, view_matrix, proj_matrix);

        for (auto& b : boids)
        {
            if (coeffs.isLowPoly)
            {
                star_boid_low.change_color(b.get_color());
                star_boid_low.set_position(b.get_position());
                boids_program.render_game_object(star_boid_low, view_matrix, proj_matrix);
            }
            else
            {
                star_boid.change_color(b.get_color());
                star_boid.set_position(b.get_position());
                boids_program.render_game_object(star_boid, view_matrix, proj_matrix);
            }

            b.update(&ctx, boids, coeffs);
        }

        for (const auto& planet : planet_positions_and_textures)
        {
            switch (static_cast<int>(planet[3]))
            {
            case 0:
                jupiter_object.set_position({planet[0], planet[1], planet[2]});
                jupiter_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(jupiter_object, view_matrix, proj_matrix);
                break;
            case 1:
                mars_object.set_position({planet[0], planet[1], planet[2]});
                mars_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(mars_object, view_matrix, proj_matrix);
                break;
            case 2:
                neptune_object.set_position({planet[0], planet[1], planet[2]});
                neptune_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(neptune_object, view_matrix, proj_matrix);
                break;
            case 3:
                venus_atmosphere_object.set_position({planet[0], planet[1], planet[2]});
                venus_atmosphere_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(venus_atmosphere_object, view_matrix, proj_matrix);
                break;
            case 4:
                venus_surface_object.set_position({planet[0], planet[1], planet[2]});
                venus_surface_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(venus_surface_object, view_matrix, proj_matrix);
                break;
            case 5:
                mercury_object.set_position({planet[0], planet[1], planet[2]});
                mercury_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(mercury_object, view_matrix, proj_matrix);
                break;
            default:
                uranus_object.set_position({planet[0], planet[1], planet[2]});
                uranus_object.set_rotation({planet[4] * ctx.time(), planet[5] * ctx.time(), 0});
                boids_program.render_game_object(uranus_object, view_matrix, proj_matrix);
                break;
            }
        }

        glDisable(GL_CULL_FACE);
    };

    ctx.start();
}
