#include <cstddef>
#include <cstdlib>
#include <vector>
#include "boid.hpp"
#include "game_object.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "glimac/common.hpp"
#include "glimac/default_shader.hpp"
#include "glimac/sphere_vertices.hpp"
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

struct BoidsProgram {
    p6::Shader program;

    GLint u_MVP_matrix;
    GLint u_MV_matrix;
    GLint u_normal_matrix;
    GLint u_texture;
    GLint u_color;   // Uniform location for color
    GLint use_color; // Uniform location for use_color flag

    BoidsProgram()
        : program{p6::load_shader("../src/shaders/3D.vs.glsl", "../src/shaders/tex_3D.fs.glsl")}
    {
        u_MVP_matrix    = glGetUniformLocation(program.id(), "u_MVP_matrix");
        u_MV_matrix     = glGetUniformLocation(program.id(), "u_MV_matrix");
        u_normal_matrix = glGetUniformLocation(program.id(), "u_normal_matrix");
        u_texture       = glGetUniformLocation(program.id(), "u_texture");
        u_color         = glGetUniformLocation(program.id(), "u_color");
        use_color       = glGetUniformLocation(program.id(), "use_color");
    }
};

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boids boids boids!"}};
    ctx.maximize_window();
    glEnable(GL_DEPTH_TEST);

    // Initialize the camera
    FreeflyCamera camera;

    // Initialize coefficients
    Coeffs coeffs;

    // Create a vector of Boid objects
    std::vector<Boid> boids(20);

    // Boids shader program
    BoidsProgram boids_program{};

    // Initialize the "star" GameObject with the loaded model and a color
    GameObject star_object("assets/models/astronout.obj", glm::vec3(0.0f, 1.0f, 1.0f)); // Now initializing with a color
    star_object.set_position(glm::vec3(0.f, 0.f, -5.f));
    star_object.set_scale(0.25f);

    // Load texture for the moon rendering
    GLuint texture_object_moon = TextureManager::load_texture("assets/textures/MoonMap.jpg");

    // Create a VBO for vertices using sphere data
    VBO vbo_vertices;
    vbo_vertices.bind();
    const std::vector<glimac::ShapeVertex> vertices = glimac::sphere_vertices(1.f, 32, 16);
    vbo_vertices.fill(vertices.data(), vertices.size() * sizeof(glimac::ShapeVertex), GL_STATIC_DRAW);
    vbo_vertices.unbind();

    // Set up the VAO for spheres
    VAO vao;
    vao.bind();
    vbo_vertices.bind();
    vao.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, position)));
    vao.specify_attribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, normal)));
    vao.specify_attribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, texCoords)));
    vao.unbind();
    vbo_vertices.unbind();

    // Generate rotation axes for each boid
    std::vector<glm::vec3> rotation_axes(boids.size());
    std::generate(rotation_axes.begin(), rotation_axes.end(), []() { return glm::sphericalRand(1.0f); });

    // Render loop
    ctx.update = [&]() {
        // Update boids
        for (auto& b : boids)
        {
            b.draw(ctx, coeffs.radius_awareness);
            b.update(&ctx, boids, coeffs);
        }

        // ImGui setup
        ImGui::Begin("Boids command panel");
        ImGui::Text("Play with the parameters of the flock!");
        coeffs.draw_Gui();
        ImGui::End();

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle camera input
        ctx.mouse_dragged = [&](p6::MouseDrag drag) {
            camera.rotateLeft(-drag.delta.x * 20.f);
            camera.rotateUp(-drag.delta.y * 20.f);
        };

        ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
            camera.moveFront(scroll.dy * 0.1);
        };

        if (ctx.key_is_pressed(GLFW_KEY_W))
        {
            camera.moveFront(0.1);
        }
        if (ctx.key_is_pressed(GLFW_KEY_S))
        {
            camera.moveFront(-0.1);
        }
        if (ctx.key_is_pressed(GLFW_KEY_D))
        {
            camera.moveLeft(-0.1);
        }
        if (ctx.key_is_pressed(GLFW_KEY_A))
        {
            camera.moveLeft(0.1);
        }

        // Get the view matrix from the camera
        glm::mat4 view_matrix = camera.getViewMatrix();
        glm::mat4 proj_matrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        // Use the shader program
        boids_program.program.use();

        // Rendering the boids as moons
        for (int i = 0; i < boids.size(); i++)
        {
            glm::mat4 MV_matrix = glm::translate(glm::mat4(1.0f), boids[i].get_position());
            MV_matrix           = glm::rotate(MV_matrix, ctx.time(), rotation_axes[i]); // Rotation using generated axis
            MV_matrix           = glm::scale(MV_matrix, glm::vec3(0.2f));               // Scale down the moon

            glm::mat4 MVP_matrix    = proj_matrix * view_matrix * MV_matrix;
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix)));

            glUniform1i(boids_program.u_texture, 0);
            glBindTexture(GL_TEXTURE_2D, texture_object_moon);
            glUniform1i(boids_program.use_color, 0); // Signal to use texture instead of color
            glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
            glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
            glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            vao.unbind();
        }

        // Render the star object with updated shader uniforms for color rendering
        glm::mat4 model_matrix_star  = star_object.get_model_matrix();
        glm::mat4 MV_matrix_star     = view_matrix * model_matrix_star;
        glm::mat3 normal_matrix_star = glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix_star)));
        glm::mat4 MVP_star           = proj_matrix * view_matrix * star_object.get_model_matrix();

        boids_program.program.use(); // Assurez-vous d'utiliser le programme de shader

        glUniform1i(boids_program.u_texture, 0); // Définir l'uniforme de texture
        glUniform1i(boids_program.use_color, 1); // Signal pour utiliser la couleur au lieu de la texture
        glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix_star));
        glUniform3fv(boids_program.u_color, 1, glm::value_ptr(star_object.get_base_color()));
        glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_star));
        glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix_star));

        star_object.draw(); // Dessiner l'étoile
    };

    // Start the render loop
    ctx.start();
}
