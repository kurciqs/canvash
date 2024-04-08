#version 330 core
out vec4 FragColor;

in vec4 o_color;
in vec3 v_pos; // original vertex position, unitary
in float stroke;
uniform vec2 u_screen_size;

void main()
{
    float dist = distance(vec2(0,0), v_pos.xy);
    if (dist < 0.5 - stroke || dist > 0.5 + stroke) {
        discard;
    }
    else {
        FragColor = vec4(o_color);
    }
}