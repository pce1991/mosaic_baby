#version 330 core

in vec2 texcoord;

in vec4 colorA;
// in vec4 colorB;

uniform sampler2D texture0;

out vec4 color;

void main() {
    vec2 uv = texcoord;
    vec4 textureContrib = texture(texture0, uv).rgba;

    // @TODO: I think we want to use alpha from the mask so that blacks are still black and aren't
    // blended with background. 
    // using alpha is good option for using arbitrary sprites and not specific masks
    color = colorA * textureContrib.r;
    //color.a = textureContrib.a * 0.5f;
    //color = vec4(1, 0, 0, 1);
}
