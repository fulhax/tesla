#version 330

in vec2 in_Position;
in vec2 in_TexCoord;

out vec4 Position;
out vec2 TexCoord;
out vec4 Color;

uniform vec2 in_UiPos;
uniform vec4 in_Color;
uniform mat4 in_OrthoMatrix;

void main(void)
{
    Color = in_Color;
    TexCoord = in_TexCoord;
    Position = in_OrthoMatrix * vec4(in_Position + in_UiPos, 1.0, 1.0);
    gl_Position = Position;
}
