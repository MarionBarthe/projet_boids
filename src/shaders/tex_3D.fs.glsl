#version 330

uniform sampler2D u_texture;
uniform vec3 u_color;
uniform bool use_color;

// Sorties du shader
in vec3 v_position_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 v_normal_vs; // Normale du sommet transformée dans l'espace View (vs)
in vec2 v_tex_coords; // Coordonnées de texture du sommet

out vec4 f_frag_color;

void main() {
    if(use_color) {
        // f_frag_color = vec4(u_color, 1.0);
        f_frag_color = vec4(v_normal_vs, 1.);
    } else {
        vec4 texture = texture(u_texture, v_tex_coords);
        f_frag_color = vec4(texture.xyz, 1.);
        //f_frag_color = vec4(v_normal_vs, 1.);
    }
}
