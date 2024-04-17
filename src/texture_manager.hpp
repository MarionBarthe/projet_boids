#pragma once

#include <string>
#include "p6/p6.h"

class TextureManager {
public:
    TextureManager() = default;

    // Load a texture from a file and return its OpenGL identifier
    static GLuint loadTexture(const std::string& filePath);

    // Binding the texture to the specified texture unit
    static void bindTexture(GLuint textureID, GLuint textureUnit);

    // Disabling the texture
    static void unbindTexture();
};
