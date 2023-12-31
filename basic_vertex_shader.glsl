#version 330 core

layout(location = 0) in vec3 inPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;

void main()
{
    FragPos = inPos;
    gl_Position = projection * view * vec4(inPos, 1.0);
}