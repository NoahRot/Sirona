#version 330 core
out vec4 FragColor;

in vec2 v_uv;                    // UV from vertex shader

uniform vec3 u_star_color;

void main()
{    
    // Distance from center (in UV space)
    float dist = length(v_uv);
    float radius = 1.0;
    
    // Smooth circle with anti-aliasing
    float edge = 0.01;                    // Adjust for sharper/softer edge
    float alpha = 1.0 - smoothstep(radius - edge, radius + edge, dist);

    // Bright core
    vec3 white = vec3(1.0, 1.0, 1.0);
    float bright_coeff = -pow(dist,2) + 1;
    vec3 color = bright_coeff*white + u_star_color;
    
    FragColor = vec4(color, alpha);
}