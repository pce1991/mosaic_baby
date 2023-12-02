#version 330 core

in vec2 texcoord;

uniform sampler2D texture0;
uniform float time;
uniform vec4 maskColor;

out vec4 color;

void main() {
    vec2 uv = texcoord;
    vec4 textureContrib = texture(texture0, uv).rgba;

    // using alpha is good option for using arbitrary sprites and not specific masks
    color = maskColor * textureContrib.a;
    //color.a = textureContrib.a * 0.5f;
}
