#include "./glimac/FreeflyCamera.hpp" // Ajout de l'include pour la classe TrackballCamera
#include "./glimac/common.hpp"
#include "./glimac/default_shader.hpp"
#include "./glimac/sphere_vertices.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
// #include "glm/gtc/random.hpp"
#include <cstddef>
#include <cstdlib>
#include "glm/gtc/type_ptr.hpp"
#include "p6/p6.h"
#define DOCTEST_CONFIG_IMPLEMENT
#include "Boid.hpp"
#include "doctest/doctest.h"

GLuint load_and_bind_texture(const img::Image& texture_image)
{
    GLuint texture_object = 0;
    glGenTextures(1, &texture_object);
    glBindTexture(GL_TEXTURE_2D, texture_object);

    GLsizei width  = static_cast<GLsizei>(texture_image.width());
    GLsizei height = static_cast<GLsizei>(texture_image.height());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_object;
}

struct boids_program {
    p6::Shader m_program;

    GLint u_MVP_matrix;
    GLint u_MV_matrix;
    GLint u_normal_matrix;
    GLint u_texture;

    boids_program()
        : m_program{
            p6::load_shader("./shaders/3D.vs.glsl", "./shaders/tex_3D.fs.glsl")
        }
    {
        u_MVP_matrix    = glGetUniformLocation(m_program.id(), "u_MVP_matrix");
        u_MV_matrix     = glGetUniformLocation(m_program.id(), "u_MV_matrix");
        u_normal_matrix = glGetUniformLocation(m_program.id(), "u_normal_matrix");
        u_texture       = glGetUniformLocation(m_program.id(), "u_texture");
    }
};

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boids boids boids !"}};
    ctx.maximize_window();

    glEnable(GL_DEPTH_TEST);

    // Initialisation de la caméra
    FreeflyCamera camera;

    // Cube cube;
    Coeffs coeffs;

    std::vector<Boid> boids(20);

    // TODO : faire une struct Dimensions pour cube_width etc ?
    //  for (auto &boid : boids) {
    //    boid.x = glm::linearRand(-cube_width / 2.0f, cube_width / 2.0f);
    //    boid.y = glm::linearRand(-cube_height / 2.0f, cube_height / 2.0f);
    //    boid.z = glm::linearRand(-cube_depth / 2.0f, cube_depth / 2.0f);
    //  }

    /*********************************
     * INITIALIZATION
     *********************************/

    boids_program boids_program{};

    /*********************************
     * LE VERTEX BUFFER OBJECT *
     *********************************/

    // Création d'un seul VBO
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // Binding de ce VBO sur la cible GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Remplissage du VBO

    const std::vector<glimac::ShapeVertex> vertices =
        glimac::sphere_vertices(1.f, 32, 16);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glimac::ShapeVertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /*********************************
     * LE VERTEX ARRAY OBJECT *
     *********************************/

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Activation des attributs de vertex
    static constexpr GLuint VERTEX_ATTR_POSITION = 0;
    static constexpr GLuint VERTEX_ATTR_NORMAL   = 1;
    static constexpr GLuint VERTEX_ATTR_TEXTURE  = 2;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);
    // Spécification des attributs de vertex
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(
        VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex),
        (const GLvoid*)offsetof(glimac::ShapeVertex, position)
    );
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(
        VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex),
        (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords)
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*********************************
     * LES TEXTURES *
     *********************************/

    const img::Image texture_image_moon =
        p6::load_image_buffer("assets_copy/textures/MoonMap.jpg");

    GLuint texture_object_moon = load_and_bind_texture(texture_image_moon);

    /*********************************
     * LES MATRICES *
     *********************************/

    glm::mat4 proj_matrix   = glm::mat4(1.0f);
    glm::mat4 normal_matrix = glm::mat4(1.0f);

    std::vector<glm::vec3> rotation_axes;
    for (int i = 0; i < 32; ++i)
    {
        rotation_axes.push_back(
            glm::sphericalRand(1.0f)
        ); // Random axis of rotation
    }

    // Déclaration de la boucle de rendu
    ctx.update = [&]() {
        /*** on update les boids ***/
        for (auto& b : boids)
        {
            b.draw(ctx, coeffs.radius_awareness);
            b.update(&ctx, boids, coeffs);
        }
        /*** IMGUI ***/
        
        {
            ImGui::Begin("Boids command panel");
            ImGui::Text("Play with the parameters of the flock !");
            // ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::SeparatorText("Rules");

            coeffs.draw_Gui();

            ImGui::End();
        }
        /*********************************
         * RENDERING
         *********************************/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);

        ctx.mouse_dragged = [&](p6::MouseDrag drag) {
            camera.rotateLeft(-drag.delta.x * 20.f);
            camera.rotateUp(-drag.delta.y * 20.f);
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

        ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
            camera.moveFront(scroll.dy * 0.1);
        };
        // Obtention de la matrice de vue depuis la caméra
        glm::mat4 view_matrix = camera.getViewMatrix();

        proj_matrix =
            glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        std::vector<glm::mat4> MV_matrixes = {};

        // Définir les dimensions du cube
        float cube_width  = 4.0f;
        float cube_height = 4.0f;
        float cube_depth  = 4.0f;

        for (const auto& boid : boids)
        {
            glm::mat4 moon = glm::mat4(1.0f);

            // Positionner chaque sphère à la position du boid
            glm::vec3 sphere_position = boid.get_position() - glm::vec3(0.f, 0.f, -5.f);

            moon = glm::translate(moon, sphere_position);

            moon = glm::rotate(moon, ctx.time(),
                               rotation_axes[0]); // Rotation constante pour l'exemple
            moon = glm::scale(moon, glm::vec3(0.2f));
            MV_matrixes.push_back(moon);
        }

        for (auto& MV_matrix : MV_matrixes)
        {
            // Calculer la matrice ModelViewProjection (MVP)
            glm::mat4 MVP_matrix = proj_matrix * view_matrix * MV_matrix;

            // Calculer la matrice Normale
            glm::mat3 normal_matrix =
                glm::transpose(glm::inverse(glm::mat3(view_matrix * MV_matrix)));

            boids_program.m_program.use();
            glUniform1i(boids_program.u_texture, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_object_moon);

            glUniformMatrix4fv(boids_program.u_MVP_matrix, 1, GL_FALSE, glm::value_ptr(MVP_matrix));
            glUniformMatrix4fv(boids_program.u_MV_matrix, 1, GL_FALSE, glm::value_ptr(MV_matrix));
            glUniformMatrix3fv(boids_program.u_normal_matrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));

            // Rendu de l'objet
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }
        glBindVertexArray(0);
    };

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Démarrer la boucle de rendu
    ctx.start();

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
