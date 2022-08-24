#include <iostream>

#include "Animation.hpp"

#include <glm/gtx/rotate_vector.hpp>

class CollWork{
    public:
    vec3 lastpos;
    bool colide;
    bool allowmove = true;
    float height = 1.8;
    float uph = 0.2;
    float allowdown = 0.2;
    vec2 borders = vec2(0.1);
    void physwork(vec3 &pos){
        if(colide == false){
            pos.y = pos.y - 0.1;
        }
    }
    void updateLastCoord(vec3 pos){
        lastpos = pos;
    }
    bool inRange(int low, int high, int mx){
        return ((mx-high)*(mx-low) <= 0);
    }
    void vertexCollision(vec4 vertpos, vec3 &pos){
        if(inRange(pos.x-borders.x,pos.x+borders.x, -vertpos.x)&&inRange(pos.z-borders.y,pos.z+borders.y, -vertpos.z)&&inRange(pos.y-height,pos.y+uph, vertpos.y)){
            pos.y = lastpos.y;
            colide = true;
            if(vertpos.y > pos.y - height + allowdown){
                pos.x = lastpos.x;
                pos.z = lastpos.z;
            }
        }else{
            colide = false;
        }
    }
    void vertexCollisionProp(vec4 vertpos, vec3 &pos){
        if(inRange(pos.x-borders.x,pos.x+borders.x, vertpos.x)&&inRange(pos.z-borders.y,pos.z+borders.y, vertpos.z)&&inRange(pos.y-height,pos.y+uph, vertpos.y)){
            pos.y = lastpos.y;
            colide = true;
            if(vertpos.y > pos.y - height + allowdown){
                pos.x = lastpos.x;
                pos.z = lastpos.z;
            }
        }else{
            colide = false;
        }
    }
    void calculateCollision(vertexbuf *vertex, int totalvertex, vec3 &pos){
        for(int i = 0; i != totalvertex; i++){
            vertexCollision(vertex[i].vertexpos, pos);
        }
    }
    void calculateCollisionProp(vertexbuf *vertex, int totalvertex, vec3 &pos){
        for(int i = 0; i != totalvertex; i++){
            vertexCollisionProp(vertex[i].vertexpos, pos);
        }
    }
};
