#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in mat4 instance_model; // Instance data - model matrix
layout(location = 5) in vec4 instance_color; // Instance data - color
layout(location = 6) in float instance_thickness; // Instance data - stroke strength

out vec4 o_color;
out vec2 fragCoord;
out float thickness;
vec2 res = vec2(800.0,600.0);

uniform mat4 u_proj;

void main() {
    o_color = instance_color;
    thickness = instance_thickness;
    gl_Position = u_proj * (instance_model * vec4(vec3(a_pos.x, a_pos.y * instance_thickness, a_pos.z), 1.0));
}