
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

layout(binding = 2) uniform sampler2D shadowMap;

layout(binding = 4) uniform samplerCube cubeMap;

layout(location = 0) in vec3 vpos;

//out col

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(cubeMap, vpos).bgra;
}