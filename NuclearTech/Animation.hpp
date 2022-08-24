#include <iostream>

#include <glm/glm.hpp>

#include "Loader.hpp"

class Animation{
    private:
    int totalvertex = 0;
    bool calctotalv = false;
    public:
    int begpos;
    int framecount = 0;
    float AditionalParameterToModels;
    string animfiles[60] = {
        "App/Models/prop.ply",
        "App/Models/propa1.ply"
    };
    int format = 2;
    int totalframes = 1;
    void Play(vec3 pos, vertexbuf *vertex, int &totalv){
        switch(format){
            case 0:
            loadobj(animfiles[framecount].c_str(), vertex, totalvertex, begpos, pos, AditionalParameterToModels);
            framecount++;
            break;
            case 1:
            loadply(animfiles[framecount].c_str(), vertex, totalvertex, begpos, pos, AditionalParameterToModels);
            framecount++;
            break;
            case 2:
            loadplycolor(animfiles[framecount].c_str(), vertex, totalvertex, begpos, pos, AditionalParameterToModels);
            framecount++;
            break;
            default:
            throw runtime_error("Unknown format");
            break;
        }
        if(framecount == totalframes){
            framecount = 0;
        }
        if(calctotalv == false){
            totalv += totalvertex;
            calctotalv = true;
        }
    }
};

class camAnimation{
    private:
    int currfr;
    public:
    vec3 campos[60];
    vec2 rotf[60];
    int finfr;
    void Play(vec3 &pos, vec2 &rot){
        pos = campos[currfr];
        rot = rotf[currfr];
        if(currfr == finfr){
            currfr = 0;
        }else{
            currfr++;
        }
    }
};
