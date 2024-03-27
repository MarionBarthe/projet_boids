#version 330

// Sorties du shader
in vec3 v_position_vs; // Position du sommet transformée dans l'espace View (vs)
in vec3 v_normal_vs; // Normale du sommet transformée dans l'espace View (vs)
in vec2 v_tex_coords; // Coordonnées de texture du sommet

out vec4 f_frag_color;

void main() {
    vec3 normalized_normal = normalize(v_normal_vs);
    f_frag_color = vec4(normalized_normal, 1.0);
}