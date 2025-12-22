#version 460 core

in vec2 v_UV;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_Tex;

vec3 ACES(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;

    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(u_Tex, vec2(v_UV.x, 1.0 - v_UV.y)).rgb;

    color = ACES(color);
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
