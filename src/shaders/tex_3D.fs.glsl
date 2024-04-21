#version 330 core

uniform sampler2D u_texture;
uniform vec3 u_color;
uniform bool use_color;

in vec3 v_normal_vs;  // Normale du sommet transformée dans l'espace View
in vec2 v_tex_coords; // Coordonnées de texture du sommet

out vec4 f_frag_color;

void main() {
    if(use_color) {
        f_frag_color = vec4(u_color, 1.0); // Utilisez la couleur uniforme si spécifié
    } else {
        vec4 texture_color = texture(u_texture, v_tex_coords.xy); // Échantillonne la texture
        f_frag_color = vec4(texture_color.xyz, 1.0); // Utilisez la couleur de la texture
        //f_frag_color = vec4(v_tex_coords, 1.0, 1.0);
    }
}
