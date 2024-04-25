#include "program.hpp"
#include "glm/gtc/type_ptr.hpp"

void Program::use()const {
    m_program.use();
}

void Program::render_game_object(GameObject& object, const glm::mat4& view_matrix, const glm::mat4& proj_matrix, Program& program) //TODO game object
{
    glm::mat4 model_matrix  = object.get_model_matrix();
    glm::mat4 MV_matrix     = view_matrix * model_matrix;
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix * model_matrix)));
    glm::mat4 MVP_matrix    = proj_matrix * view_matrix * model_matrix;

    program.use();

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