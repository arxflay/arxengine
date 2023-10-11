#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexPos;

out vec2 TexPos;

void main()
{
    gl_Position = vec4(aPos.xy, 0.0f, 1.0f);
    TexPos = aTexPos;
}
