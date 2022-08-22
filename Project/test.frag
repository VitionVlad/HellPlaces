
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

layout(location = 0) in vec3 vertcolor;

layout(location = 1) in vec2 uvs;

layout(location = 2) in vec3 normal;

layout(location = 3) in vec3 crntPos;

layout(location = 4) in float param;

layout(location = 5) in vec4 mat;

layout(location = 6) in vec3 vpos;

//out col

layout(location = 0) out vec4 outColor;

vec3 unpackColor(float f) {
    vec3 color;
    color.r = floor(f);
    color.g = floor((f-color.r)*1000);
    color.b = floor((((f-color.r)*1000)-color.g)*1000);
    return color / 255;
}

vec3 DirectionalLight(){
    float dist = length(vec3(1.0f, 1.0f, 0.0f));
    float a = 0.2f;
    float b = 0.06f;
    float inten = 1.0f / (a*dist*dist+b*dist+1.0f);
    float ambient = 0.01;
    vec3 finnorm = normalize(normal);
    vec3 lightdir = normalize(vec3(1.0f, 1.0f, 1.0f));
    float diffuse = max(dot(finnorm, lightdir), 0.0f);
    float specular = 0.5f;
    vec3 viewdir = normalize(vec3(-ubo.pPos.x, ubo.pPos.y, -ubo.pPos.z) - crntPos);
    vec3 reflectdir = reflect(-lightdir, finnorm);
    vec3 halfway = normalize(viewdir + lightdir);
    float specam = pow(max(dot(finnorm, halfway), 0.0f), 16);
    float fspecular = specam * specular;

    float shadow = 0.0f;

    vec3 lightcoord = mat.xyz/mat.w;
    if(lightcoord.z <= 1.0f){
        lightcoord.xy = (lightcoord.xy + 1.0f)/2.0f;
        float currdepth = lightcoord.z;
        float bias = 0.005*tan(acos(dot(finnorm, lightdir)));
        bias = clamp(bias, 0,0.01);
        int Sampleradion = 2;
        vec2 pixelsize = 1.0f / textureSize(shadowMap, 0);
        for(int y = -Sampleradion; y <= Sampleradion; y++){
            for(int x = -Sampleradion; x <= Sampleradion; x++){
                float closestdepth = texture(shadowMap, lightcoord.xy+vec2(x, y)*pixelsize).r;
                if(currdepth > closestdepth + bias){
                    shadow+=1.0f;
                }
            }
        }
        shadow/=pow((Sampleradion*2+1), 2);
    }
    
    vec3 fincol = texture(texSampler, vec3(uvs, 0)).bgr * (diffuse*(1.0f-shadow)*inten+ambient)* vec3(0.1, 0.1, 0.9) + texture(texSampler, vec3(uvs, 1)).r * (fspecular *(1.0f-shadow) * inten) * vec3(0.1, 0.1, 0.9);
    return fincol;
}

bool inRange(float low, float high, float mx){
    return ((mx-high)*(mx-low) <= 0);
}

vec3 PointLight(vec4 pos){
    vec3 lightvec = pos.xyz - crntPos;
    float dist = length(lightvec);
    float a = 0.1f;
    float b = 0.06f;
    float inten = 1.0f / (a*dist*dist+b*dist+1.0f);
    float ambient = 0.01;
    vec3 finnorm = normalize(normal);
    vec3 lightdir = normalize(lightvec);
    float diffuse = max(dot(finnorm, lightdir), 0.0f);
    float specular = 0.5f;
    vec3 viewdir = normalize(vec3(-ubo.pPos.x, ubo.pPos.y, -ubo.pPos.z) - crntPos);
    vec3 reflectdir = reflect(-lightdir, finnorm);
    vec3 halfway = normalize(viewdir + lightdir);
    float specam = pow(max(dot(finnorm, halfway), 0.0f), 16);
    float fspecular = specam * specular;
    
    vec3 fincol = texture(texSampler, vec3(uvs, 0)).bgr * (diffuse*inten+ambient) * unpackColor(pos.w) + texture(texSampler, vec3(uvs, 1)).r * (fspecular * inten) * unpackColor(pos.w);
    return fincol;
}

void main() {
    vec3 finlight = vec3(0.0);

    float transparency = 1.0f;

    finlight = DirectionalLight();

    for(int i = 1; i != 11; i++){
        if(ubo.massive[i].w != 0){
            finlight += PointLight(ubo.massive[i]);
        }
    }

    vec3 I = normalize(vec3(-crntPos.x, crntPos.y, -crntPos.z) - ubo.pPos);
    vec3 R = reflect(I, normalize(normal));

    //finlight = finlight*texture(cubeMap, R).bgr;

    if(param == 4000){
        finlight = texture(cubeMap, vpos).bgr;
    }
    if(param == 2000){
        finlight = PointLight(vec4(0, 0, 0, 255.255255));
    }
    if(param == 6000){
        finlight = vec3(0, 0, 1);
    }
    if(param == 1000){
        finlight = texture(texSampler, vec3(uvs, 2)).bgr;
    }
    if(param == 9000){
        finlight = texture(texSampler, vec3(uvs, 3)).bgr;
    }

    outColor = vec4(finlight, transparency);
}