#version 330

in vec3 Normal;
out vec4 out_Diffuse;

uniform vec3 in_Color;

void main(void)
{
    out_Diffuse = vec4(in_Color.xyz, 1.0);
}
