#version 330 core
out vec4 FragColor;

in vec4 o_color;
in vec2 fragCoord;
in float thickness;
vec2 res = vec2(800.0,600.0);

float sdRoundRect( in vec2 p, in vec2 b, in float r )
{
    vec2 q = abs(p)-b+r;
    return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r;
}

void main()
{
    vec2 uv = (2.0*gl_FragCoord.xy-res)/res.y;
//    vec2 uv = (2.0*fragCoord-res)/res.y;
//    vec2 uv = fragCoord;
    //    FragColor = vec4(uv, 1.0, 1.0);

//    float d = distance(vec2(0.5, 0.0), uv);
    float width = 300.0;
    float d = sdRoundRect(uv - vec2((width) / res.y, 0.0), vec2((width + thickness) / res.y * 0.5, thickness / res.y * 0.5), thickness / res.y * 0.5);
//    if (d > 0.1) discard;
    FragColor = vec4(uv, 0.0, 1.0) * step(d, 0.0);
//    FragColor = vec4(step(d, 0.0), 0.0, 0.0, 1.0);
//    FragColor = vec4(abs(uv), 0.0, 1.0);
}