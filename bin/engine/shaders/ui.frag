#version 330

in vec4 Position;
in vec2 TexCoord;

out vec4 out_Diffuse;

uniform sampler2D FontTexture;

void main(void)
{
    out_Diffuse = texture(FontTexture, TexCoord.xy).rrrg;
}
