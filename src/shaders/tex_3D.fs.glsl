#version 330 core

uniform sampler2D u_texture;      // Texture sampler
uniform vec3 u_color;             // Uniform color for non-textured rendering
uniform bool use_color;           // Flag to toggle between color and texture

in vec3 v_normal_vs;              // Transformed vertex normal in view space (currently unused)
in vec2 v_tex_coords;             // Texture coordinates from the vertex shader

out vec4 f_frag_color;            // Output fragment color

void main() {
    if(use_color) {
        // Use the uniform color if specified
        f_frag_color = vec4(u_color, 1.0);
    } else {
        // Sample the texture
        vec4 texture_color = texture(u_texture, v_tex_coords);
        // Use the color from the texture
        f_frag_color = vec4(texture_color.rgb, 1.0);
    }
}
