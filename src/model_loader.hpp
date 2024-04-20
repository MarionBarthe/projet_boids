#pragma once

#include <string>
#include <vector>
#include "tiny_obj_loader.h"

class ModelLoader {
public:
    struct Model {
        std::vector<float>        vertices;
        std::vector<float>        normals;
        std::vector<float>        texcoords;
        std::vector<unsigned int> indices;
    };

    static Model load_model(const std::string& file_path);

private:
    static Model process_model(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes);
};
