#version 400 core

out vec4 outCol;
uniform sampler2D mainTexture;
uniform vec3 colour;

in vec2 uv;

void main()
{
	outCol.rgba = texture2D(mainTexture, uv).rgba;

    if (outCol.a < 0.5)
        discard;

    outCol.rgb *= colour;
}