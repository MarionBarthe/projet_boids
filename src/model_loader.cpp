#define TINYOBJLOADER_IMPLEMENTATION

#include "model_loader.hpp"
#include <iostream>

ModelLoader::Model ModelLoader::loadModel(const std::string& filePath)
{
    Model                    model;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filePath, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    model = processModel(attrib, shapes);

    return model;
}

ModelLoader::Model ModelLoader::processModel(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes)
{
    Model model;

    // Loop over shapes
    for (const auto& shape : shapes)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            size_t fv = shape.mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                const tinyobj::index_t& idx = shape.mesh.indices[index_offset + v];
                const float             vx  = attrib.vertices[3 * idx.vertex_index + 0];
                const float             vy  = attrib.vertices[3 * idx.vertex_index + 1];
                const float             vz  = attrib.vertices[3 * idx.vertex_index + 2];
                model.vertices.push_back(vx);
                model.vertices.push_back(vy);
                model.vertices.push_back(vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0)
                {
                    const float nx = attrib.normals[3 * idx.normal_index + 0];
                    const float ny = attrib.normals[3 * idx.normal_index + 1];
                    const float nz = attrib.normals[3 * idx.normal_index + 2];
                    model.normals.push_back(nx);
                    model.normals.push_back(ny);
                    model.normals.push_back(nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0)
                {
                    const float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    const float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                    model.texcoords.push_back(tx);
                    model.texcoords.push_back(ty);
                }
            }
            index_offset += fv;

            // per-face material
            shape.mesh.material_ids[f];
        }
    }

    return model;
}
