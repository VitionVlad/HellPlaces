#include <iostream>

#include <thread>

#include "Gui.hpp"

float pSpeed = 0.1;

MagmaVK render;

CollWork colision;

void movecallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_W && action == GLFW_REPEAT){
        render.pos.z += cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.x += cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == GLFW_KEY_A && action == GLFW_REPEAT){
        render.pos.x += cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.z -= cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == GLFW_KEY_S && action == GLFW_REPEAT){
        render.pos.z -= cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.x -= cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == GLFW_KEY_D && action == GLFW_REPEAT){
        render.pos.x -= cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.z += cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
}

class NuclearTechVk{
    private:
    vec2 oldpos;
    public:
    dvec2 rawm;
    vec2 speed;
    float sensivity = 1000;
    bool showmouse = false;
    bool mouselook;
    bool collisionenable;
    bool enablephysics;
    vec2 mouseupdown = vec2(1.5, -1.5);
    bool enableshadows = true;
    float groundlevelshadow = 0.01;
    int scalingFactor = 1;
    string vshaderpath[99]{};
    string fshaderpath[99]{};
    VkPolygonMode polyMode[99]{};
    VkCullModeFlags CullMode[99]{};
    int aditionalPipelineCounts = 0;
    void Init(){
        glfwInit();
        glfwSwapInterval(0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        render.window = glfwCreateWindow(render.resolution.x, render.resolution.y, "", NULL, NULL);
        render.vshadowpath = "App/Raw/smvert.spv";
        render.fshadowpath = "App/Raw/smfrag.spv";
        render.enableshadowmaps = true;
        render.vshaderpath[0] = "App/Raw/vert.spv";
        render.fshaderpath[0] = "App/Raw/frag.spv";
        render.pippolymode[0] = VK_POLYGON_MODE_FILL;
        render.cullmode[0] = VK_CULL_MODE_NONE;
        render.enablepipeline[0] = true;
        for(int i = 0; i!=aditionalPipelineCounts; i++){
            render.vshaderpath[i+1] = vshaderpath[i];
            render.fshaderpath[i+1] = fshaderpath[i];
            render.pippolymode[i+1] = polyMode[i];
            render.cullmode[i+1] = CullMode[i];
            render.enablepipeline[i+1] = true;
        }
        render.pipelinecounts = 1+aditionalPipelineCounts;
        render.vPostPath = "App/Raw/pvert.spv";
        render.fPostPath = "App/Raw/pfrag.spv";
        render.RenderResolution.x = render.resolution.x / scalingFactor;
        render.RenderResolution.y = render.resolution.y / scalingFactor;
        render.Init();
    }
    void TurnPipeline(int cnt, bool value){
        render.enablepipeline[cnt] = value;
    }
    void objwork(const char* path, int begpos, vec3 pos, float AditionalParameter){
        loadobj(path, vertex, render.totalv, begpos, pos, AditionalParameter);
    }
    void plywork(const char* path, int begpos, bool color, vec3 pos, float AditionalParameter){
        switch(color){
            case 0:
            loadply(path, vertex, render.totalv, begpos, pos, AditionalParameter);
            break;
            case 1:
            loadplycolor(path, vertex, render.totalv, begpos, pos, AditionalParameter);
            break;
        }
    }
    void CreateProp(Prop &prop, const char* pathtomodel, int modeltype, vec2 updown, vec2 border, float allwdown, float AditionalParameter){
        prop.begpos = render.totalv;
        switch (modeltype){
        case 0:
        objwork(pathtomodel, render.totalv, vec3(0, 0, 0), AditionalParameter);
            break;
         case 1:
        plywork(pathtomodel, render.totalv, false, vec3(0, 0, 0), AditionalParameter);
            break;
         case 2:
        plywork(pathtomodel, render.totalv, true, vec3(0, 0, 0), AditionalParameter);
            break;
        default:
        throw runtime_error("Error: Unknown model type");
            break;
        }
        prop.pos = vec3(0, 0, 0);
        prop.finpos = render.totalv;
        prop.saveprop(vertex);
        prop.setsize(updown, border, allwdown);
    }
    float packColor(vec3 color) {
        if(color.r < 5){
            color.r = 5;
        }
        if(color.g < 5){
            color.g = 5;
        }
        if(color.b < 5){
            color.b = 5;
        }
        return color.r+(color.g*0.001)+(color.b*0.000001);
    }
    void light(vec3 pos, float color, uint lightcnt){
        if(lightcnt > 100){
            throw runtime_error("Fatal light error");
        }else{
            render.ubo.massive[lightcnt] = vec4(pos.x, pos.y, pos.z, color);
        }
    }
    void Update(GLFWkeyfun keyfun){
        glfwSetKeyCallback(render.window, keyfun);
        glfwGetCursorPos(render.window, &rawm.x, &rawm.y);
        if(oldpos.x != render.pos.x && oldpos.y != render.pos.z){
            speed.x = render.pos.x - oldpos.x;
            speed.y = render.pos.z - oldpos.y; 
            oldpos.x = render.pos.x;
            oldpos.y = render.pos.z;
        }
        if(oldpos.x == render.pos.x && oldpos.y == render.pos.y){
            speed.x = 0;
            speed.y = 0;
        }
        if(collisionenable == true){
            colision.calculateCollision(vertex, render.totalv, render.pos);
        }
        if(mouselook == true){
            render.rot.x = rawm.x / sensivity;
            render.rot.y = -rawm.y / sensivity;
            if(render.rot.y < mouseupdown.y){
                render.rot.y = mouseupdown.y;
            }
            if(render.rot.y > mouseupdown.x){
                render.rot.y = mouseupdown.x;
            }
        }
        render.RenderResolution.x = render.resolution.x / scalingFactor;
        render.RenderResolution.y = render.resolution.y / scalingFactor;
        render.Draw(vertex);
        if(collisionenable == true){
            colision.updateLastCoord(render.pos);
        }
        if(enablephysics == true){
            colision.physwork(render.pos);
        }
        switch(enableshadows){
            case 1:
            render.useshadowmaps = true;
            break;
            case 0:
            render.useshadowmaps = false;
            break;
        }
        switch(showmouse){
            case 1:
            glfwSetInputMode(render.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
            default:
            glfwSetInputMode(render.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        }
    }
    void End(){
        render.Destroy();
    }
};
