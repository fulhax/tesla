#version 330

in vec2 in_Position;
in vec2 in_TexCoord;

out vec4 Position;
out vec2 TexCoord;

uniform vec2 in_UiPos;
uniform mat4 in_OrthoMatrix;

void main(void)
{
    TexCoord = in_TexCoord;
    Position = in_OrthoMatrix * vec4(in_Position + in_UiPos, 1.0, 1.0);
    gl_Position = Position;
}
