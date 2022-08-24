#include <iostream>

#include "Prop.hpp"

using namespace glm;

void GuiDrawTriangle(float packedcolor, vec2 v1, vec2 v2, vec2 v3, vec2 uv1, vec2 uv2, vec2 uv3, vertexbuf *vertex, int totalvert){
    vertex[totalvert].vertexpos = vec4(v1.x, v1.y, packedcolor, 1000);
    vertex[totalvert+1].vertexpos = vec4(v2.x, v2.y, packedcolor, 1000);
    vertex[totalvert+2].vertexpos = vec4(v3.x, v3.y, packedcolor, 1000);
    vertex[totalvert].uv = vec2(uv1.x, uv1.y);
    vertex[totalvert+1].uv = vec2(uv2.x, uv2.y);
    vertex[totalvert+2].uv = vec2(uv3.x, uv3.y);
}
void GuiDrawRectangle(float packedcolor, vec2 v1, vec2 v2, vec2 v3, vec2 v4, vec2 uv1, vec2 uv2, vec2 uv3, vec2 uv4, vertexbuf *vertex, int totalvert){
    GuiDrawTriangle(packedcolor, v1, v2, v3, uv1, uv2, uv3, vertex, totalvert);
    GuiDrawTriangle(packedcolor, v1, v3, v4, uv1, uv3, uv4, vertex, totalvert+3);
}

class Button{
    private:
    int begpos;
    float mcolor = 255.0;
    float scolor = 0.255000;
    float pcolor = 0.000255;
    public:
    bool pressed = false;
    vec2 v1 = vec2(-0.5, 0.5);
    vec2 v2 = vec2(0, 0.5);
    vec2 v3 = vec2(0, 0);
    vec2 v4 = vec2(-0.5, 0);
    vec2 uv1 = vec2(-0.5, 0.5);
    vec2 uv2 = vec2(0, 0.5);
    vec2 uv3 = vec2(0, 0);
    vec2 uv4 = vec2(-0.5, 0);
    void Setup(int &totalvert, float maincolor, float selectcolor, float presscolor){
        begpos = totalvert;
        totalvert += 6;
        mcolor = maincolor;
        scolor = selectcolor;
        pcolor = presscolor;
    }
    void Update(vertexbuf *vertex, dvec2 rawm, ivec2 resolution, int state){
        vec2 mousepos;
        mousepos.x = rawm.x / (resolution.x / 2.f) - 1;
        mousepos.y = 1 - rawm.y / (resolution.y / 2.f);
        if(mousepos.x <= v3.x && mousepos.y <= v3.y && mousepos.x >= v1.x && mousepos.y >= v1.y && state != GLFW_PRESS){
            GuiDrawRectangle(scolor, v1, v2, v3, v4, uv1, uv2, uv3, uv4, vertex, begpos);
            pressed = false;
        }else if(mousepos.x <= v3.x && mousepos.y <= v3.y && mousepos.x >= v1.x && mousepos.y >= v1.y && state == GLFW_PRESS){
            GuiDrawRectangle(pcolor, v1, v2, v3, v4, uv1, uv2, uv3, uv4, vertex, begpos);
            pressed = true;
        }else{
            GuiDrawRectangle(mcolor, v1, v2, v3, v4, uv1, uv2, uv3, uv4, vertex, begpos);
            pressed = false;
        }
    }
};

class ProgressBar{
    private:
    int begpos;
    public:
    vec2 v1 = vec2(-1, -0.9);
    vec2 v2 = vec2(-1, -1);
    vec2 uv1 = vec2(-1, -0.9);
    vec2 uv2 = vec2(-1, -1);
    float color;
    void setup(int &totalvert, float maincolor){
        begpos = totalvert;
        totalvert += 6;
        color = maincolor;
    }
    void Update(vertexbuf *vertex, float value, float maxvalue){
        GuiDrawRectangle(color, v1, vec2((value/maxvalue)+v1.x, v1.y), vec2((value/maxvalue)+v2.x, v2.y), v2, v1, vec2((value/maxvalue)+v1.x, v1.y), vec2((value/maxvalue)+v2.x, v2.y), v2, vertex, begpos);
    }
};
