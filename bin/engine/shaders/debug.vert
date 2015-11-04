#version 330

in vec3 in_Position;
out vec4 Position;

uniform mat4 in_ModelMatrix;
uniform mat4 in_ViewMatrix;
uniform mat4 in_ProjMatrix;

void main(void)
{
    gl_Position = in_ProjMatrix * in_ViewMatrix * in_ModelMatrix * vec4(in_Position, 1.0);
}
