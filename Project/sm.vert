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

void main() {
    gl_Position = ubo.lightmat * vec4(vertpos.x, -vertpos.y, vertpos.z, 1.0);
}
