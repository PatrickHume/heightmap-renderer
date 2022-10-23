// A simple passthough vertex shader.
// https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
// vertex shader
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
out vec2 TexCoord;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTex;
}
