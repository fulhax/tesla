#version 330

in vec4 Position;
in vec2 TexCoord;

out vec4 out_Diffuse;

uniform sampler2D Texture;

void main(void)
{
    out_Diffuse = texture(Texture, vec2(TexCoord.x, -TexCoord.y)); //vec4(Color);
    if(out_Diffuse.w < 0.01f) {
        discard;
    }
}
