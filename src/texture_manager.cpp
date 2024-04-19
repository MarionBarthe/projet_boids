#include "texture_manager.hpp"

GLuint TextureManager::loadTexture(const std::string& filePath)
{
    // Loading image from file
    img::Image textureImage = p6::load_image_buffer(filePath);

    // Generating the OpenGL texture object
    GLuint textureObject = 0;
    glGenTextures(1, &textureObject);
    glBindTexture(GL_TEXTURE_2D, textureObject);

    // Configuring texture settings
    GLsizei width  = static_cast<GLsizei>(textureImage.width());
    GLsizei height = static_cast<GLsizei>(textureImage.height());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Disabling the texture
    unbindTexture();

    return textureObject;
}

void TextureManager::bindTexture(GLuint textureID, GLuint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureManager::unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}