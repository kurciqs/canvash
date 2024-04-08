#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in mat4 instance_model; // Instance data - model matrix
layout(location = 5) in vec4 instance_color; // Instance data - color
layout(location = 6) in float instance_width; // Instance data - width of the outline

out vec4 o_color;
out vec3 v_pos; // original vertex position, unitary
out float stroke;
uniform mat4 u_proj;

void main() {
    o_color = instance_color;
    stroke = instance_width;
    v_pos = a_pos * (1.0 + stroke * 2.0);
    gl_Position = u_proj * (instance_model * vec4(v_pos, 1.0));
}