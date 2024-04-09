#version 330 core
out vec4 FragColor;

in vec4 o_color;
in vec3 v_pos;
in float i_stroke;
in vec2 i_size;
uniform vec2 u_screen_size;

// https://iquilezles.org/articles/distfunctions2d/ - best source for sdfs EVER
float sdEllipse( in vec2 p, in vec2 ab )
{
    if (ab.x == ab.y) ab.x += 0.001; // otherwise division by zero, nobody will mind this "impurity"
    p = abs(p); if( p.x > p.y ) {p=p.yx;ab=ab.yx;}
    float l = ab.y*ab.y - ab.x*ab.x;
    float m = ab.x*p.x/l;      float m2 = m*m;
    float n = ab.y*p.y/l;      float n2 = n*n;
    float c = (m2+n2-1.0)/3.0; float c3 = c*c*c;
    float q = c3 + m2*n2*2.0;
    float d = c3 + m2*n2;
    float g = m + m*n2;
    float co;
    if( d<0.0 )
    {
        float h = acos(q/c3)/3.0;
        float s = cos(h);
        float t = sin(h)*sqrt(3.0);
        float rx = sqrt( -c*(s + t + 2.0) + m2 );
        float ry = sqrt( -c*(s - t + 2.0) + m2 );
        co = (ry+sign(l)*rx+abs(g)/(rx*ry)- m)/2.0;
    }
    else
    {
        float h = 2.0*m*n*sqrt( d );
        float s = sign(q+h)*pow(abs(q+h), 1.0/3.0);
        float u = sign(q-h)*pow(abs(q-h), 1.0/3.0);
        float rx = -s - u - c*4.0 + 2.0*m2;
        float ry = (s - u)*sqrt(3.0);
        float rm = sqrt( rx*rx + ry*ry );
        co = (ry/sqrt(rm-rx)+2.0*g/rm-m)/2.0;
    }
    vec2 r = ab * vec2(co, sqrt(1.0-co*co));
    return length(r-p) * sign(p.y-r.y);
}

void main()
{
    float dist = distance(vec2(0,0), v_pos.xy);
    vec2 es = i_size;
    float factor = max(es.x, es.y) * 2.0; // not exactly sure why this factor but i established it using experimental observation (i guessed it)
    float stroke = i_stroke / factor;
    float d = sdEllipse(v_pos.xy, es / factor);
    if (abs(d) > stroke) {
        discard;
    }
    else {
        FragColor = vec4(o_color);
    }
}