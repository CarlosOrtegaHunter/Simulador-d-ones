#version 330 core
out vec4 FragColor;
in float colorDensity;
void main()
{
    FragColor = vec4(colorDensity, 0.0f, 0.5f - colorDensity/3.0f, 1.0f);
} 