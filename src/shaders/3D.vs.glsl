#version 330 core

// Attributs de sommet
layout(location = 0) in vec3 a_vertex_position; // Position du sommet
layout(location = 1) in vec3 a_vertex_normal; // Normale du sommet
layout(location = 2) in vec2 a_vertex_tex_coords; // Coordonnées de texture du sommet

// Matrices de transformation reçues en uniform
uniform mat4 u_MVP_matrix;
uniform mat4 u_MV_matrix;
uniform mat3 u_normal_matrix; // Utilisation d'une matrice 3x3 pour les normales

// Sorties du shader
out vec3 v_position_vs; // Position du sommet transformée dans l'espace View (vs)
out vec3 v_normal_vs; // Normale du sommet transformée dans l'espace View (vs)
out vec2 v_tex_coords; // Coordonnées de texture du sommet

void main() {
    // Passage en coordonnées homogènes
    vec4 vertex_position = vec4(a_vertex_position, 1.);
    vec4 vertex_normal = vec4(a_vertex_normal, 0.);

    // Calcul des valeurs de sortie
    v_position_vs = vec3(u_MV_matrix * vertex_position);
    v_normal_vs = normalize(u_normal_matrix * a_vertex_normal); // Normalisation de la normale après transformation
    v_tex_coords = a_vertex_tex_coords;

    // Calcul de la position projetée
    gl_Position = u_MVP_matrix * vertex_position;
}
