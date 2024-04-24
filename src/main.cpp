#include <cstddef>
#include <cstdlib>
#include <vector>
#include "boid.hpp"
#include "game_object.hpp"
#include "glimac/common.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glimac/trackball_camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "model_loader.hpp"
#include "p6/p6.h"
#include "texture_manager.hpp"
#include "vao.hpp"
#include "vbo.hpp"
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "markov_chain.hpp"
#include "random_generator.hpp"

struct BoidsProgram {
    p6::Shader program;

    GLint u_MVP_matrix;
    GLint u_MV_matrix;
    GLint u_normal_matrix;

    GLint u_texture;
    GLint u_color;
    GLint u_use_color;

    GLint u_kd;
    GLint u_ks;
    GLint u_shininess;

    GLint u_light_pos_vs_0;
    GLint u_light_intensity_0;
    GLint u_light_pos_vs_1;
    GLint u_light_intensity_1;

    double next_event_time = 0.0;
    double lambda          = 1.0 / 5; // Every 5 seconds

    BoidsProgram()
        : program{p6::load_shader(
            "../src/shaders/3D.vs.glsl",
            "../src/shaders/tex_3D.fs.glsl"
        )}
        , u_MVP_matrix(glGetUniformLocation(program.id(), "u_MVP_matrix"))
        , u_MV_matrix(glGetUniformLocation(program.id(), "u_MV_matrix"))
        , u_normal_matrix(glGetUniformLocation(program.id(), "u_normal_matrix"))
        , u_texture(glGetUniformLocation(program.id(), "u_texture"))
        , u_color(glGetUniformLocation(program.id(), "u_color"))
        , u_use_color(glGetUniformLocation(program.id(), "u_use_color"))
        , u_kd(glGetUniformLocation(program.id(), "u_kd"))
        , u_ks(glGetUniformLocation(program.id(), "u_ks"))
        , u_shininess(glGetUniformLocation(program.id(), "u_shininess"))
        , u_light_pos_vs_0(glGetUniformLocation(program.id(), "u_lights[0].position"))
        , u_light_intensity_0(glGetUniformLocation(program.id(), "u_lights[0].intensity"))
        , u_light_pos_vs_1(glGetUniformLocation(program.id(), "u_lights[1].position"))
        , u_light_intensity_1(glGetUniformLocation(program.id(), "u_lights[1].intensity"))

    {
    }
};

struct Light {
    glm::vec3 position;  // Light position in view space
    glm::vec3 intensity; // Light intensity
};

void change_astronaut_temperament(MarkovChain& chain)
{
    // Perform transitions
    chain.transition_values();
    chain.display_state_counts();
    // Display the current state and count of states every 10 transitions
    if (chain.get_number_of_states() % 10 == 0)
    {
        print_container(chain.calculate_stationary_distribution());
    }
}

void time_events(BoidsProgram& program, MarkovChain& chain, p6::Context& ctx)
{
    double current_time = ctx.time();
    if (current_time >= program.next_event_time)
    {
        change_astronaut_temperament(chain);

        double delay            = exponential_distribution(program.lambda);
        program.next_event_time = current_time + delay;
        std::cout << "Next event in " << delay << " seconds." << std::endl;
    }
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

void move_surveyor(GameObject& surveyor, p6::Context& ctx)
{
    float     speed = 0.05f;
    glm::vec3 movement(0.0f);

    if (ctx.key_is_pressed(GLFW_KEY_W))
    {
        movement.z -= speed;
    }
    if (ctx.key_is_pressed(GLFW_KEY_S))
    {
        movement.z += speed;
    }
    if (ctx.key_is_pressed(GLFW_KEY_A))
    {
        movement.x -= speed;
    }
    if (ctx.key_is_pressed(GLFW_KEY_D))
    {
        movement.x += speed;
    }
    if (ctx.key_is_pressed(GLFW_KEY_UP))
    {
        movement.y += speed;
    }
    if (ctx.key_is_pressed(GLFW_KEY_DOWN))
    {
        movement.y -= speed;
    }

    surveyor.move(movement);
}

std::pair<glm::vec3, glm::vec3> calculate_wiggle_offsets(p6::Context& ctx)
{
    const float amplitude_position = 0.01f;
    const float amplitude_rotation = 1.f;
    const float period_position    = 2.0f;
    const float period_rotation    = 5.0f;

    glm::vec3 pos_offset(
        sin(ctx.time() / period_position) * amplitude_position,
        cos(ctx.time() / period_position * 1.25) * amplitude_position,
        sin(ctx.time() / period_position * 1.5) * amplitude_position
    );

    glm::vec3 rot_offset(
        sin(ctx.time() / period_rotation) * amplitude_rotation,
        cos(ctx.time() / period_rotation) * amplitude_rotation,
        sin(ctx.time() / period_rotation * 1.5) * amplitude_rotation
    );

    return {pos_offset, rot_offset};
}

void move_surveyor_with_wiggle(GameObject& surveyor, p6::Context& ctx)
{
    move_surveyor(surveyor, ctx);

    auto [pos_offset, rot_offset] = calculate_wiggle_offsets(ctx);
    surveyor.set_position(surveyor.get_position() + pos_offset);
    surveyor.set_rotation(surveyor.get_rotation() + rot_offset);
}

void render_game_object(GameObject& object, const glm::mat4& view_matrix, const glm::mat4& proj_matrix, BoidsProgram& program)
{
    glm::mat4 model_matrix  = object.get_model_matrix();
    glm::mat4 MV_matrix     = view_matrix * model_matrix;
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * model_matrix)));
    glm::mat4 MVP_matrix    = proj_matrix * view_matrix * model_matrix;

    program.program.use();

    glUniformMatrix4fv(program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
    glUniformMatrix4fv(program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
    glUniformMatrix3fv(program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

    glUniform3fv(program.u_kd, 1, glm::value_ptr(object.get_diffuse_factor()));
    glUniform3fv(program.u_ks, 1, glm::value_ptr(object.get_specular_factor()));
    glUniform1f(program.u_shininess, object.get_shininess_factor());

    if (object.get_use_texture())
    {
        glUniform1i(program.u_texture, 0); // Bind texture unit 0
        glBindTexture(GL_TEXTURE_2D, object.get_texture());
        glUniform1i(program.u_use_color, 0); // Signal to use texture
    }
    else
    {
        glUniform1i(program.u_use_color, 1);                                       // Signal to use color
        glUniform3fv(program.u_color, 1, glm::value_ptr(object.get_base_color())); // Send the color to the shader
    }

    object.draw();
}

int main()
{
    auto ctx = p6::Context{{1280, 720, "Space Boids - Barthe & Duval"}};
    ctx.maximize_window();
    glEnable(GL_DEPTH_TEST);

    // Seed the random number generator
    srand(time(NULL));

    TrackballCamera   camera;
    Coeffs            coeffs;
    std::vector<Boid> boids(20);
    BoidsProgram      boids_program{};
    Light             lights[2];

    std::vector<std::vector<double>> astronaut_transition_matrix = {
        {0.4, 0.1, 0.2, 0.1, 0.2}, // Happy to Happy, Sad, Angry, Scared, Relaxed
        {0.2, 0.3, 0.1, 0.1, 0.3}, // Sad to Happy, Sad, Angry, Scared, Relaxed
        {0.1, 0.2, 0.4, 0.1, 0.2}, // Angry to Happy, Sad, Angry, Scared, Relaxed
        {0.1, 0.2, 0.1, 0.4, 0.2}, // Scared to Happy, Sad, Angry, Scared, Relaxed
        {0.3, 0.1, 0.1, 0.1, 0.4}  // Relaxed to Happy, Sad, Angry, Scared, Relaxed
    };

    std::vector<double> astronaut_initial_state = {0.2, 0.1, 0.1, 0.1, 0.5};

    // Create Markov chain object
    MarkovChain astronaut_chain(astronaut_transition_matrix, astronaut_initial_state);

    GameObject astronaut_object("assets/models/astronaut2.obj", "assets/textures/astronaut_texture.jpg");
    astronaut_object.set_position(glm::vec3(0.f, 0.f, -5.f));
    astronaut_object.set_scale(glm::vec3(0.25f, 0.25f, 0.25f));
    astronaut_object.set_factors({1.f, 0.5f, 0.5f}, {1.f, 0.5f, 0.5f}, 100.0f);

    GameObject astronaut_edge_object("assets/models/astronaut2.obj", glm::vec3(.0f, .0f, .0f));
    astronaut_edge_object.set_scale(glm::vec3(0.26f, 0.26f, 0.26f));
    astronaut_edge_object.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 0.0f);

    GameObject star_object("assets/models/star.obj", glm::vec3(0.0f, 1.0f, 1.0f));
    star_object.set_position(glm::vec3(0.f, 0.f, -1.f));
    star_object.set_scale(glm::vec3(0.01f, 0.01f, 0.01f));

    GameObject star_object_2("assets/models/star.obj", glm::vec3(1.0f, 0.0f, 0.0f));
    star_object_2.set_scale(glm::vec3(0.01f, 0.01f, 0.01f));

    GameObject star_edge_object("assets/models/star.obj", glm::vec3(.0f, .0f, .0f));
    star_edge_object.set_scale(glm::vec3(0.011f, 0.011f, 0.011f));
    star_edge_object.set_factors({1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, 0.0f);

    GameObject space_object("assets/models/space.obj", "assets/textures/space_texture.jpg");
    space_object.set_scale(glm::vec3(0.1f, 0.1f, 0.1f));
    space_object.set_factors({0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, 50.0f);

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

    int number_of_planets = binomial_distribution_cdf(12, 0.5);
    std::cout << "" << number_of_planets << std::endl;
    std::vector<glm::vec4> planet_positions_and_textures(number_of_planets);

    float scale_cube = 200 * 0.1;

    for (int i = 0; i < number_of_planets; i++)
    {
        double x = (beta_distribution(1.0, 1.0) - 0.5) * scale_cube; // edge
        double y = (beta_distribution(1.0, 1.0) - 0.5) * scale_cube; // uniform
        double z = (beta_distribution(1.0, 1.0) - 0.5) * scale_cube; // edge

        int texture_index = discrete_uniform_distribution(0, 6);

        planet_positions_and_textures[i] = glm::vec4(x, y, z, static_cast<float>(texture_index));
    }

    GLuint texture_object_moon = TextureManager::load_texture("assets/textures/MoonMap.jpg");

    VBO vbo_vertices;
    vbo_vertices.bind();
    const std::vector<glimac::ShapeVertex> vertices = glimac::sphere_vertices(1.f, 32, 16);
    vbo_vertices.fill(vertices.data(), vertices.size() * sizeof(glimac::ShapeVertex), GL_STATIC_DRAW);
    vbo_vertices.unbind();

    VAO vao;
    vao.bind();
    vbo_vertices.bind();
    vao.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, position)));
    vao.specify_attribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, normal)));
    vao.specify_attribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, texCoords)));
    vao.unbind();
    vbo_vertices.unbind();

    std::vector<glm::vec3> rotation_axes(boids.size(), glm::vec3(0.0f, 0.0f, 0.0f));

    float last_x = 0;
    float last_y = 0;

    glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);
    float     lightMotionRadius = 8.0f;
    float     lightMotionSpeed  = 0.5f;
    lights[0].intensity         = glm::vec3(4.0f, 4.0f, 4.0f);

    ctx.update = [&]() {
        for (auto& b : boids)
        {
            b.draw(ctx, coeffs.radius_awareness);
            b.update(&ctx, boids, coeffs);
        }

        time_events(boids_program, astronaut_chain, ctx);

        ImGui::Begin("Boids command panel");
        ImGui::Text("Play with the parameters of the flock!");
        coeffs.draw_Gui();
        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        move_surveyor_with_wiggle(astronaut_object, ctx);
        camera.set_center(astronaut_object.get_position());
        handle_camera_input(ctx, camera, last_x, last_y);

        glm::mat4 view_matrix = camera.get_view_matrix();
        glm::mat4 proj_matrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        // Update light position
        float time      = ctx.time();
        lightPosition.x = sin(time * lightMotionSpeed) * lightMotionRadius;
        lightPosition.z = cos(time * lightMotionSpeed) * lightMotionRadius;

        star_object.set_position(lightPosition);
        lights[0].position = glm::vec3(view_matrix * glm::vec4(lightPosition, 1.0));

        astronaut_object.move_y(1.1f);
        star_object_2.set_position(astronaut_object.get_position());
        lights[1].position = glm::vec3(view_matrix * glm::vec4(astronaut_object.get_position(), 1.0));
        astronaut_object.move_y(-1.1f);

        switch (astronaut_chain.get_deterministic_current_state())
        {
        case 0:                                                                                                // Happy
            astronaut_object.interpolate_material_factors({0.8f, 0.8f, 0.2f}, {1.f, 1.f, 1.f}, 100.0f, 0.05f); // Bright and shiny materials
            lights[1].intensity = glm::mix(lights[1].intensity, glm::vec3(3.f, 3.f, 1.5f), 0.1f);              // Bright and warm color
            break;
        case 1:                                                                                                  // Sad
            astronaut_object.interpolate_material_factors({0.3f, 0.4f, 0.7f}, {0.5f, 0.5f, 0.8f}, 50.0f, 0.05f); // Darker and less reflective materials
            lights[1].intensity = glm::mix(lights[1].intensity, glm::vec3(0.5f, 0.5f, 2.f), 0.1f);               // Cooler, dimmer blue light
            break;
        case 2:                                                                                                 // Angry
            astronaut_object.interpolate_material_factors({0.9f, 0.2f, 0.2f}, {1.f, 0.3f, 0.3f}, 80.0f, 0.05f); // Vibrant and less reflective materials
            lights[1].intensity = glm::mix(lights[1].intensity, glm::vec3(3.f, 0.3f, 0.3f), 0.1f);              // Intense red to enhance the feeling of anger
            break;
        case 3:                                                                                                  // Scared
            astronaut_object.interpolate_material_factors({0.3f, 0.1f, 0.3f}, {0.4f, 0.2f, 0.7f}, 20.0f, 0.05f); // Darker violet materials
            lights[1].intensity = glm::mix(lights[1].intensity, glm::vec3(0.5f, 0.2f, 0.8f), 0.1f);              // Darker, purple light
            break;
        default:                                                                                               // Relaxed
            astronaut_object.interpolate_material_factors({0.4f, 0.6f, 0.9f}, {0.8f, 1.f, 1.f}, 70.0f, 0.05f); // Calm and reflective materials
            lights[1].intensity = glm::mix(lights[1].intensity, glm::vec3(0.2f, 0.8f, 2.f), 0.1f);             // Soft blue-green
            break;
        }

        boids_program.program.use();
        glUniform3fv(boids_program.u_light_pos_vs_0, 1, glm::value_ptr(lights[0].position));
        glUniform3fv(boids_program.u_light_intensity_0, 1, glm::value_ptr(lights[0].intensity));
        glUniform3fv(boids_program.u_light_pos_vs_1, 1, glm::value_ptr(lights[1].position));
        glUniform3fv(boids_program.u_light_intensity_1, 1, glm::value_ptr(lights[1].intensity));

        for (int i = 0; i < boids.size(); i++)
        {
            glm::mat4 MV_matrix = glm::translate(glm::mat4(1.0f), boids[i].get_position());
            MV_matrix           = glm::rotate(MV_matrix, ctx.time(), rotation_axes[i]);
            MV_matrix           = glm::scale(MV_matrix, glm::vec3(0.2f));

            glm::mat4 MVP_matrix    = proj_matrix * view_matrix * MV_matrix;
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix)));

            glUniform1i(boids_program.u_texture, 0);
            glBindTexture(GL_TEXTURE_2D, texture_object_moon);
            glUniform1i(boids_program.u_use_color, 0);
            glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
            glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
            glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            vao.unbind();
        }

        astronaut_edge_object.set_position(astronaut_object.get_position());
        astronaut_edge_object.set_rotation(astronaut_object.get_rotation());

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        astronaut_edge_object.set_position(astronaut_object.get_position());
        render_game_object(astronaut_edge_object, view_matrix, proj_matrix, boids_program);

        star_edge_object.set_position(star_object.get_position());
        render_game_object(star_edge_object, view_matrix, proj_matrix, boids_program);

        star_edge_object.set_position(star_object_2.get_position());
        render_game_object(star_edge_object, view_matrix, proj_matrix, boids_program);
        render_game_object(space_object, view_matrix, proj_matrix, boids_program);

        for (const auto& planet : planet_positions_and_textures)
        {
            planet_edge_object.set_position({planet[0], planet[1], planet[2]});
            render_game_object(planet_edge_object, view_matrix, proj_matrix, boids_program);
        }

        glCullFace(GL_BACK);
        render_game_object(astronaut_object, view_matrix, proj_matrix, boids_program);
        render_game_object(star_object, view_matrix, proj_matrix, boids_program);
        render_game_object(star_object_2, view_matrix, proj_matrix, boids_program);

        for (const auto& planet : planet_positions_and_textures)
        {
            switch (static_cast<int>(planet.w))
            {
            case 0:
                jupiter_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(jupiter_object, view_matrix, proj_matrix, boids_program);
                break;
            case 1:
                mars_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(mars_object, view_matrix, proj_matrix, boids_program);
                break;
            case 2:
                neptune_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(neptune_object, view_matrix, proj_matrix, boids_program);
                break;
            case 3:
                // Supposons que "earth_object" est une autre planète à rendre
                venus_atmosphere_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(venus_atmosphere_object, view_matrix, proj_matrix, boids_program);
                break;
            case 4:
                // Supposons que "venus_object" est une autre planète à rendre
                venus_surface_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(venus_surface_object, view_matrix, proj_matrix, boids_program);
                break;
            case 5:
                // Supposons que "mercury_object" est une autre planète à rendre
                mercury_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(mercury_object, view_matrix, proj_matrix, boids_program);
                break;
            default:
                // Supposons que "saturn_object" est une autre planète à rendre
                uranus_object.set_position({planet[0], planet[1], planet[2]});
                render_game_object(uranus_object, view_matrix, proj_matrix, boids_program);
                break;
            }
        }

        glDisable(GL_CULL_FACE);

        render_game_object(star_object, view_matrix, proj_matrix, boids_program);
    };

    ctx.start();
}
