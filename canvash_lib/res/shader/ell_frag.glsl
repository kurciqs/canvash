#version 330 core
out vec4 FragColor;

in vec4 o_color;
in vec3 v_pos; // original vertex position, unitary
uniform vec2 u_screen_size;

void main()
{
    if (distance(vec2(0,0), v_pos.xy) > 0.5) {
        discard;
    }
    else {
        FragColor = vec4(o_color);
    }
}