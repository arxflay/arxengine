#version 330 core
in vec2 TexPos;
out vec4 FragColor;
uniform sampler2D tex;

void main()
{
    vec4 coloredVec = texture(tex, TexPos);
    FragColor = coloredVec.rrrr;
}
