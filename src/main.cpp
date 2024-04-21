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
    GLint u_color;
    GLint use_color;

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

void renderGameObject(GameObject& object, const glm::mat4& view_matrix, const glm::mat4& proj_matrix, BoidsProgram& program)
{
    glm::mat4 model_matrix  = object.get_model_matrix();
    glm::mat4 MV_matrix     = view_matrix * model_matrix;
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix)));
    glm::mat4 MVP_matrix    = proj_matrix * view_matrix * model_matrix;

    program.program.use();
    glUniformMatrix4fv(program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
    glUniformMatrix4fv(program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
    glUniformMatrix3fv(program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

    if (object.get_use_texture())
    {
        glUniform1i(program.u_texture, 0); // Bind texture unit 0
        glBindTexture(GL_TEXTURE_2D, object.get_texture());
        glUniform1i(program.use_color, 0); // Signal to use texture
    }
    else
    {
        glUniform1i(program.use_color, 1);                                         // Signal to use color
        glUniform3fv(program.u_color, 1, glm::value_ptr(object.get_base_color())); // Send the color to the shader
    }

    object.draw();
}

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boids boids boids!"}};
    ctx.maximize_window();
    glEnable(GL_DEPTH_TEST);

    FreeflyCamera     camera;
    Coeffs            coeffs;
    std::vector<Boid> boids(20);
    BoidsProgram      boids_program{};

    GameObject astronaut_object("assets/models/astronaut.obj", "assets/textures/astronaut_texture.jpg");
    astronaut_object.set_position(glm::vec3(0.f, 0.f, -5.f));
    astronaut_object.set_scale(0.25f);

    GameObject star_object("assets/models/star.obj", glm::vec3(0.0f, 1.0f, 1.0f));
    star_object.set_position(glm::vec3(0.f, 0.f, -1.f));
    star_object.set_scale(0.01f);

    GameObject space_object("assets/models/space.obj", "assets/textures/space_texture.jpg");
    space_object.set_scale(0.5f);

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

    std::vector<glm::vec3> rotation_axes(boids.size());
    std::generate(rotation_axes.begin(), rotation_axes.end(), []() { return glm::sphericalRand(1.0f); });

    ctx.update = [&]() {
        for (auto& b : boids)
        {
            b.draw(ctx, coeffs.radius_awareness);
            b.update(&ctx, boids, coeffs);
        }

        ImGui::Begin("Boids command panel");
        ImGui::Text("Play with the parameters of the flock!");
        coeffs.draw_Gui();
        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ctx.mouse_dragged = [&](p6::MouseDrag drag) {
            camera.rotateLeft(-drag.delta.x * 20.f);
            camera.rotateUp(-drag.delta.y * 20.f);
        };

        ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
            camera.moveFront(scroll.dy * 0.1);
        };

        if (ctx.key_is_pressed(GLFW_KEY_W))
            camera.moveFront(0.1);
        if (ctx.key_is_pressed(GLFW_KEY_S))
            camera.moveFront(-0.1);
        if (ctx.key_is_pressed(GLFW_KEY_D))
            camera.moveLeft(-0.1);
        if (ctx.key_is_pressed(GLFW_KEY_A))
            camera.moveLeft(0.1);

        glm::mat4 view_matrix = camera.getViewMatrix();
        glm::mat4 proj_matrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        boids_program.program.use();

        for (int i = 0; i < boids.size(); i++)
        {
            glm::mat4 MV_matrix = glm::translate(glm::mat4(1.0f), boids[i].get_position());
            MV_matrix           = glm::rotate(MV_matrix, ctx.time(), rotation_axes[i]);
            MV_matrix           = glm::scale(MV_matrix, glm::vec3(0.2f));

            glm::mat4 MVP_matrix    = proj_matrix * view_matrix * MV_matrix;
            glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix)));

            glUniform1i(boids_program.u_texture, 0);
            glBindTexture(GL_TEXTURE_2D, texture_object_moon);
            glUniform1i(boids_program.use_color, 0);
            glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
            glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
            glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            vao.unbind();
        }

        renderGameObject(astronaut_object, view_matrix, proj_matrix, boids_program);
        renderGameObject(space_object, view_matrix, proj_matrix, boids_program);
        renderGameObject(star_object, view_matrix, proj_matrix, boids_program);
    };

    ctx.start();
}
