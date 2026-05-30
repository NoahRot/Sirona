#version 330 core

in vec2 v_uv;

out vec4 FragColor;

uniform vec3 u_starColor;
uniform float u_time;

// --------------------------------------------------
// Hash
// --------------------------------------------------

float hash(vec2 p)
{
    p = fract(p * vec2(123.34, 345.45));
    p += dot(p, p + 34.345);

    return fract(p.x * p.y);
}

// --------------------------------------------------
// Value noise
// --------------------------------------------------

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(
        mix(a, b, u.x),
        mix(c, d, u.x),
        u.y
    );
}

// --------------------------------------------------
// FBM
// --------------------------------------------------

float fbm(vec2 p)
{
    float value = 0.0;
    float amplitude = 0.5;

    for(int i = 0; i < 4; ++i)
    {
        value += amplitude * noise(p);

        p *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

// --------------------------------------------------
// Main
// --------------------------------------------------

void main()
{
    //---------------------------------------------
    // Convert UV to [-1,+1]
    //---------------------------------------------

    vec2 p = v_uv * 2.0 - 1.0;

    float r = length(p);

    //---------------------------------------------
    // Soft glow outside star
    //---------------------------------------------

    float glow =
        exp(-r * 3.5);

    //---------------------------------------------
    // Discard far away pixels
    //---------------------------------------------

    if(r > 1.5)
        discard;

    //---------------------------------------------
    // Star disk
    //---------------------------------------------

    float disk =
        1.0 - smoothstep(0.98, 1.0, r);

    //---------------------------------------------
    // Sphere lighting
    //---------------------------------------------

    float sphere =
        sqrt(max(0.0, 1.0 - r * r));

    //---------------------------------------------
    // Limb darkening
    //---------------------------------------------

    float limb =
        mix(0.45, 1.0, sphere);

    //---------------------------------------------
    // Surface animation
    //---------------------------------------------

    vec2 noiseUV =
        p * 5.0 +
        vec2(u_time * 0.03, u_time * 0.02);

    float surface =
        fbm(noiseUV);

    float granulation =
        mix(0.85, 1.15, surface);

    //---------------------------------------------
    // Star color
    //---------------------------------------------

    vec3 color =
        u_starColor *
        limb *
        granulation;

    //---------------------------------------------
    // Corona
    //---------------------------------------------

    vec3 coronaColor =
        u_starColor * glow * 0.5;

    //---------------------------------------------
    // Final mix
    //---------------------------------------------

    vec3 finalColor =
        color * disk +
        coronaColor;

    //---------------------------------------------
    // Alpha
    //---------------------------------------------

    float alpha =
        max(
            disk,
            glow * 0.25
        );

    FragColor =
        vec4(finalColor, alpha);
}