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

layout(location = 6) out vec3 vpos;

vec3 unpackColor(float f) {
    vec3 color;
    color.r = floor(f);
    color.g = floor((f-color.r)*1000);
    color.b = floor((((f-color.r)*1000)-color.g)*1000);
    return color / 255;
}

vec2 unpackTexNum(float packed){
    vec2 result;
    result.x = floor(packed);
    result.y = (packed - result.x) * 10;
    return result;
}

void main() {
    if(vertpos.w == 1000){
        gl_Position = vec4(vertpos.x, -vertpos.y, 0, 1);
        param = vertpos.z;
    }else if(vertpos.w == 2000){
        vec4 pos = ubo.proj * ubo.model * vec4(vertpos.x, -vertpos.y, vertpos.z, 1.0);
        pos.z = pos.z/100;
        gl_Position = pos;
        vertcolor = vertcol;
        param = vertpos.w;
    }else{
        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertpos.x, -vertpos.y, vertpos.z, 1.0);
        mat = ubo.lightmat * vec4(vertpos.x, -vertpos.y, vertpos.z, 1.0);
        vpos = vec3(vertpos.x, vertpos.y, vertpos.z);
        vertcolor = vertcol;
        param = vertpos.w;
    }
    uvs = vec2(uv.x, -uv.y);
    crntPos = vec3(ubo.model*vec4(vertpos.xyz, 1.0f));
    normal = normals;
}
