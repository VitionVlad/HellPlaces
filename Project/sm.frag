
#version 450

layout(binding = 0) uniform umvp {
    mat4 proj;
    mat4 view;
    mat4 model;
    mat4 lightmat;
    vec4 massive[100];
    vec3 pPos;
} ubo;

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0)in vec3 vertcolor;

layout(location = 1) in vec2 uvs;

layout(location = 2) in vec3 normal;

layout(location = 3) in vec3 crntPos;

layout(location = 4) in float param;

layout(location = 5) in vec4 mat;

//out col

layout(location = 0) out vec4 outColor;

void main() {
}