#version 330

in vec4 Position;
in vec2 TexCoord;
in vec4 Color;

out vec4 out_Diffuse;

uniform sampler2D FontTexture;

void main(void)
{
    out_Diffuse = vec4(Color.rgb, texture(FontTexture, TexCoord.xy).r * Color.a);
}
