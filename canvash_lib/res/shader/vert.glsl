#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 offset;
void main()
{
gl_Position = vec4(aPos / 10.0 + offset, 1.0);
}