#version 450

layout(binding = 0) uniform umvp {
    mat4 proj;
    mat4 view;
    mat4 model;
    mat4 lightmat;
    vec4 massive[100];
    vec3 pPos;
} ubo;

layout(location = 0) in vec4 vertpos;

layout(location = 1) in ivec3 vertcol;

layout(location = 2) in vec3 normals;

layout(location = 3) in vec2 uv;

//to frag

layout(location = 0) out vec3 vertcolor;

layout(location = 1) out vec2 uvs;

layout(location = 2) out vec3 normal;

layout(location = 3) out vec3 crntPos;

layout(location = 4) out float param;

layout(location = 5) out vec4 mat;

vec2 screenplane[6] = vec2[](
    vec2(-1, -1),
    vec2(-1, 1),
    vec2(1, 1),
    vec2(-1, -1),
    vec2(1, -1),
    vec2(1, 1)
);

void main() {
    gl_Position = vec4(screenplane[gl_VertexIndex], 0, 1);
    uvs = (screenplane[gl_VertexIndex]+1)/2;
}
