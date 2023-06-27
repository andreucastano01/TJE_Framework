#version 330 core
out vec4 FragColor;
uniform float progress;
void main()
{
    vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    if (gl_FragCoord.x < progress)
        FragColor = green;
    else
        FragColor = black;
}
