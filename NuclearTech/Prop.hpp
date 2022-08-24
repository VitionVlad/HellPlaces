#include <iostream>

#include "CollWork.hpp"

vertexbuf propvertex[9999999];

class Prop{
    private:
    bool inRange(int low, int high, int mx){
        return ((mx-high)*(mx-low) <= 0);
    }
    vec2 yr;
    vec2 xzr;
    public:
    int begpos;
    int finpos;
    vec3 pos;
    CollWork propcolision;
    bool enablep = true;
    bool enablec = true;
    bool enablepi = true;
    bool enablepip = true;
    bool playerinteracted;
    void setsize(vec2 updown, vec2 border, float allwdown){
        propcolision.borders = border;
        propcolision.height = updown.y;
        propcolision.uph = updown.x;
        propcolision.allowdown = allwdown;
    }
    void saveprop(vertexbuf *vertex){
        for(int i = begpos; i != finpos; i++){
            propvertex[i].vertexpos.x = vertex[i].vertexpos.x;
            propvertex[i].vertexpos.y = vertex[i].vertexpos.y;
            propvertex[i].vertexpos.z = vertex[i].vertexpos.z;
        }
    }
    void changepos(vertexbuf *vertex){
        for(int i = begpos; i != finpos; i++){
            vertex[i].vertexpos.x = propvertex[i].vertexpos.x + pos.x;
            vertex[i].vertexpos.y = propvertex[i].vertexpos.y + pos.y;
            vertex[i].vertexpos.z = propvertex[i].vertexpos.z + pos.z;
        }
    }
    void PropPosInteract(vec3 pPos, vec2 speed){
        if(inRange(pPos.z - propcolision.borders.y-1, pPos.z + propcolision.borders.y+1, -pos.z)&&inRange(pPos.x - propcolision.borders.x-1, pPos.x + propcolision.borders.x+1, -pos.x)&&enablepi==true){
            if(enablepip == true){
                pos.z-=speed.y;
                pos.x-=speed.x;
            }
            playerinteracted = true;
        }else{
            playerinteracted = false;
        }
    }
    void updateProp(vertexbuf *vertex, vec3 pPos, vec2 speed){
        if(enablec == true){
            propcolision.calculateCollisionProp(vertex, begpos, pos);
            propcolision.updateLastCoord(pos);
        }
        if(enablep == true){
            propcolision.physwork(pos);
        }
        PropPosInteract(pPos, speed);
        changepos(vertex);
    }
};
