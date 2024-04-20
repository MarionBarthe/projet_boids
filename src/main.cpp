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

    BoidsProgram()
        : program{p6::load_shader("../src/shaders/3D.vs.glsl", "../src/shaders/tex_3D.fs.glsl")}
    {
        u_MVP_matrix    = glGetUniformLocation(program.id(), "u_MVP_matrix");
        u_MV_matrix     = glGetUniformLocation(program.id(), "u_MV_matrix");
        u_normal_matrix = glGetUniformLocation(program.id(), "u_normal_matrix");
        u_texture       = glGetUniformLocation(program.id(), "u_texture");
    }
};

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boids boids boids !"}};
    ctx.maximize_window();
    glEnable(GL_DEPTH_TEST);

    // Initialize the camera
    FreeflyCamera camera;

    // Initialize coefficients
    Coeffs coeffs;

    // Create a vector of Boid objects
    std::vector<Boid> boids(20);

    // Load the model
    auto model = ModelLoader::load_model("assets/models/star.obj");

    // Initialize the "star" GameObject with the loaded model
    GameObject star_object("assets/models/star.obj", ""); // Create the "star" object with the loaded model
    star_object.set_position(glm::vec3(0.f, 0.f, -5.f));
    star_object.set_scale(0.25f);

    // Vertex Buffer Object (VBO) setup for the model
    VBO vbo_model;
    vbo_model.bind();
    vbo_model.fill(model.vertices.data(), model.vertices.size() * sizeof(float), GL_STATIC_DRAW);
    vbo_model.unbind();

    // Vertex Array Object (VAO) setup for the model
    VAO vao_model;
    vao_model.bind();
    vbo_model.bind();
    vao_model.specify_attribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const GLvoid*)0);
    vao_model.unbind();
    vbo_model.bind();

    // Boids shader program
    BoidsProgram boids_program{};

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

    // Load textures
    GLuint texture_object_moon = TextureManager::load_texture("assets/textures/MoonMap.jpg");

    // Matrix setup
    glm::mat4              proj_matrix   = glm::mat4(1.0f);
    glm::mat4              normal_matrix = glm::mat4(1.0f);
    std::vector<glm::vec3> rotation_axes;
    for (int i = 0; i < 32; ++i)
    {
        rotation_axes.push_back(glm::sphericalRand(1.0f)); // Random axis of rotation
    }

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
        proj_matrix           = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        // Rendering the boids as moons
        for (const auto& boid : boids)
        {
            glm::mat4 MV_matrix = glm::translate(glm::mat4(1.0f), boid.get_position());
            MV_matrix           = glm::rotate(MV_matrix, ctx.time(), rotation_axes[0]); // Constant rotation for the example
            MV_matrix           = glm::scale(MV_matrix, glm::vec3(0.2f));               // Scale down the moon

            glm::mat4 MVP_matrix    = proj_matrix * view_matrix * MV_matrix;
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix)));

            boids_program.program.use();
            glUniform1i(boids_program.u_texture, 0);
            glBindTexture(GL_TEXTURE_2D, texture_object_moon);
            glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
            glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
            glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            vao.unbind();
        }

        // Render the star object
        glm::mat4 MVP_star           = proj_matrix * view_matrix * star_object.get_model_matrix();
        glm::mat3 normal_matrix_star = glm::transpose(glm::inverse(glm::mat3(view_matrix * star_object.get_model_matrix())));
        boids_program.program.use();
        glUniform1i(boids_program.u_texture, 0);
        glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_star));
        glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(star_object.get_model_matrix()));
        glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix_star));
        star_object.draw();
    };

    // Unbind all textures
    glBindTexture(GL_TEXTURE_2D, 0);

    // Start the render loop
    ctx.start();
}
