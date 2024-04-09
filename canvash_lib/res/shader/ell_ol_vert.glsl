#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in mat4 instance_model; // Instance data - model matrix
layout(location = 5) in vec4 instance_color; // Instance data - color
layout(location = 6) in float instance_width; // Instance data - width of the outline
layout(location = 7) in vec2 instance_size; // Instance data - dimensions of the ellipse

out vec4 o_color;
out vec3 v_pos; // original vertex position, unitary
out vec2 i_size; // original vertex position, unitary
out float i_stroke;
uniform mat4 u_proj;
uniform vec2 u_screen_size;

void main() {
    o_color = instance_color;
    i_stroke = instance_width;
    i_size = instance_size;
    v_pos = (a_pos * (2.0));
    gl_Position = u_proj * (instance_model * vec4(v_pos, 1.0));
}