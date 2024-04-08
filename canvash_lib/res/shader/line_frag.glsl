#version 330 core
out vec4 FragColor;

in vec4 o_color;
in vec2 fragCoord;
in float thickness;

void main()
{
    FragColor = vec4(o_color);
}