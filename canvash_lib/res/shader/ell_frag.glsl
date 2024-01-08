#version 330 core
out vec4 FragColor;

in vec4 o_color;
in vec3 v_pos; // original vertex position, unitary

void main()
{
    // Normalized coordinates (from 0 to 1) of the fragment within the screen
    vec2 normalizedCoords = gl_FragCoord.xy / vec2(800.0, 600.0); // Replace 800x600 with your screen size

    if (distance(vec2(0,0), v_pos.xy) > 0.5) {
        discard;
    }
    else {
        FragColor = vec4(o_color);
    }
}