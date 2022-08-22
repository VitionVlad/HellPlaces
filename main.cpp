#include <iostream>

#include <NuclearTech/EngineVK.hpp>

NuclearTechVk Engine;

float usrfov = 120;

bool linklight = false;

int level = 1;

bool loaded = false;

Prop Exit;

Prop door[2];

Prop KeyCard[2];

Prop enemy[10];

int bulletstep = 0;

vec2 bulletrot = vec2(0);

bool keyactive[2] = {
    false, 
    false
};

bool doorused[2] = {
    false, 
    false
};

int hp = 100;

int ehp = 100;

ProgressBar hpbar;

ProgressBar ehpbar;

bool pause = true;

int keybind[6] = {
    GLFW_KEY_W,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D,
    GLFW_KEY_F6,
    GLFW_KEY_F5,
};

Button playbtn;

Button settingsbtn;

Button quitbtn;

//2160x3840 resolution
Button reskbtn;
//2560x1440 resolution
Button respbtn;
//2048x1080 resolution
Button reshalfkbtn;
//1920x1080
Button resfullhd;
//1600x900
Button reshdpbtn;
//1280x720
Button reshdbtn;
//1/1 render resolution 
Button fullrhd;
//shadows on/off
Button shadowbtn;
//fullscreen on/off
Button fullscreenbtn;

int st = 0;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && pause == false){
        bulletrot = render.rot;
        render.ubo.massive[12] = vec4(render.pos.x, render.pos.y, render.pos.z, 1);
        bulletstep =  0;
        Engine.rawm.y -= render.resolution.y /2;
        glfwSetCursorPos(render.window, Engine.rawm.x, Engine.rawm.y);
    }
    st = action;
}

void propAi(Prop &prop){
    switch(level){
        case 6:
        if(-render.pos.x >= prop.pos.x){
            prop.pos.x += 0.05;
        }
        if(-render.pos.x <= prop.pos.x){
            prop.pos.x -= 0.05;
        }
        if(-render.pos.z >= prop.pos.z){
            prop.pos.z += 0.05;
        }
        if(-render.pos.z <= prop.pos.z){
            prop.pos.z -= 0.05;
        }
        break;
        default:
        if(-render.pos.x >= prop.pos.x){
            prop.pos.x += 0.5;
        }
        if(-render.pos.x <= prop.pos.x){
            prop.pos.x -= 0.5;
        }
        if(-render.pos.z >= prop.pos.z){
            prop.pos.z += 0.5;
        }
        if(-render.pos.z <= prop.pos.z){
            prop.pos.z -= 0.5;
        }
        break;
    }
}

void calcbullet(){
    if(bulletstep <= 10){
        render.ubo.massive[12].z += cos(bulletrot.y) * cos(bulletrot.x) * 1;
        render.ubo.massive[12].y += sin(bulletrot.y) * 1;
        render.ubo.massive[12].x += cos(bulletrot.y) * sin(bulletrot.x) * -1;
        bulletstep++;
    }
    if(bulletstep > 10){
        render.ubo.massive[12] = vec4(900);
    }
}

void saveLuciferFile(){
    string strnum = to_string(level);
    string path = "App/SPlist/Level"+strnum+".lucifer";
    ofstream lucifer;
    lucifer.open(path);
    for(int i = 1;i!=11;i++){
        lucifer << "lt " << i << " " << render.ubo.massive[i].x << " " << render.ubo.massive[i].z << endl;
    }
    for(int i = 0;i!=2;i++){
        lucifer << "dr " << i << " " << door[i].pos.x << " " << door[i].pos.z << endl;
        lucifer << "kc " << i << " " << KeyCard[i].pos.x << " " << KeyCard[i].pos.z << endl;
    }
    for(int i = 0;i!=10;i++){
        lucifer << "en " << i << " " << enemy[i].pos.x << " " << enemy[i].pos.z << endl;
    }
    lucifer << "ex 0 " << Exit.pos.x << " " << Exit.pos.z << endl;
    cout << "HellPlacesDev:Lucifer File Saved" << endl;
}

void loadLuciferFile(){
    string strnum = to_string(level);
    string path = "App/SPlist/Level"+strnum+".lucifer";
    ifstream lucifer;
    lucifer.open(path);
    string type;
    int num;
    float x;
    float y;
    while(lucifer >> type >> num >> x >> y){
        if(type == "lt"){
            Engine.light(vec3(x, 5, y), 241.235156, num);
        }
        if(type == "dr"){
            door[num].pos = vec3(x, 0, y);
        }
        if(type == "kc"){
           KeyCard[num].pos = vec3(x, 0, y);
        }
        if(type == "en"){
            enemy[num].pos = vec3(x, 2, y);
        }
        if(type == "ex"){
            Exit.pos = vec3(x, 0, y);
        }
    }
    for(int i = 0; i!=2; i++){
        door[i].changepos(vertex);
    }
    for(int i = 0; i!=2; i++){
        KeyCard[i].changepos(vertex);
    }
    for(int i = 0; i!=10; i++){
        enemy[i].changepos(vertex);
    }
    Exit.changepos(vertex);
    cout << "HellPlaces:Lucifer File Loaded" << endl;
}

void reset_props(){
    for(int i = 0; i!=2; i++){
        door[i].pos = vec3(-200*i);
        door[i].changepos(vertex);
    }
    for(int i = 0; i!=2; i++){
        KeyCard[i].pos = vec3(-300*i);
        KeyCard[i].changepos(vertex);
    }
    for(int i = 0; i!=10; i++){
        enemy[i].pos = vec3(-400*i);
        enemy[i].changepos(vertex);
    }
    Exit.pos = vec3(-1000);
    Exit.changepos(vertex);
}

int writetype;

void savegame(){
    string path = "App/User/GameSave.Hell";
    ofstream save;
    save.open(path);
    save << "level " << level << endl;
    save.close();
    cout << "HellPlaces:Progress Saved" << endl;
}

void loadsave(){
    loaded = false;
    string path = "App/User/GameSave.Hell";
    ifstream save;
    save.open(path);
    string param;
    save >> param >> level;
    save.close();
    render.pos = vec3(0, 5, 0);
    cout << "HellPlaces:Progress loaded" << endl;
}

void HellCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == keybind[0] && action == GLFW_REPEAT && pause == false){
        render.pos.z += cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.x += cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == keybind[1] && action == GLFW_REPEAT && pause == false){
        render.pos.x += cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.z -= cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == keybind[2] && action == GLFW_REPEAT && pause == false){
        render.pos.z -= cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.x -= cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == keybind[3] && action == GLFW_REPEAT && pause == false){
        render.pos.x -= cos(render.rot.y) * cos(render.rot.x) * pSpeed;
        render.pos.z += cos(render.rot.y) * sin(render.rot.x) * -pSpeed;
    }
    if (key == keybind[4] && action == GLFW_PRESS){
        savegame();
    }
    if (key == keybind[5] && action == GLFW_PRESS){
        loadsave();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        switch(pause){
            case false:
            pause = true;
            Engine.showmouse = true;
            Engine.mouselook = false;
            break;
            case true:
            pause = false;
            Engine.showmouse = false;
            Engine.mouselook = true;
            break;
        }
    }
}

void HellCallbackDev(GLFWwindow* window, int key, int scancode, int action, int mods){
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

    if (key == GLFW_KEY_F12 && action == GLFW_PRESS){
        saveLuciferFile();
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        writetype = 0;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        writetype = 1;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS){
        writetype = 2;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS){
        writetype = 3;
    }
    if (key == GLFW_KEY_Y && action == GLFW_PRESS){
        writetype = 4;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS){
        writetype = 5;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS){
        reset_props();
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 1);
            break;
            case 1:
            door[0].pos = vec3(-render.pos.x+1, 0, -render.pos.z);
            break;
            case 2:
            KeyCard[0].pos = vec3(-render.pos.x+1, 1, -render.pos.z);
            break;
            case 3:
            enemy[0].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
            case 4:
            Exit.pos = vec3(-render.pos.x-1, 1, -render.pos.z);
            Exit.changepos(vertex);
            break;
            case 5:
            level = 1;
            loaded = false;
            break;
        }
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 2);
            break;
            case 1:
            door[1].pos = vec3(-render.pos.x+1, 0, -render.pos.z);
            break;
            case 2:
            KeyCard[1].pos = vec3(-render.pos.x+1, 1, -render.pos.z);
            break;
            case 3:
            enemy[1].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
            case 5:
            level = 2;
            loaded = false;
            break;
        }
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 3);
            break;
            case 3:
            enemy[2].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
            case 5:
            level = 3;
            loaded = false;
            break;
        }
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 4);
            break;
            case 3:
            enemy[3].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
            case 5:
            level = 4;
            loaded = false;
            break;
        }
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 5);
            break;
            case 3:
            enemy[4].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
            case 5:
            level = 5;
            loaded = false;
            break;
        }
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 6);
            break;
            case 3:
            enemy[5].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
            case 5:
            level = 6;
            loaded = false;
            break;
        }
    }
    if (key == GLFW_KEY_7 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 7);
            break;
            case 3:
            enemy[6].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
        }
    }
    if (key == GLFW_KEY_8 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 8);
            break;
            case 3:
            enemy[7].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
        }
    }
    if (key == GLFW_KEY_9 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 9);
            break;
            case 3:
            enemy[8].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
        }
    }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS){
        switch(writetype){
            case 0:
            Engine.light(vec3(-render.pos.x, 5, -render.pos.z), 241.235156, 10);
            break;
            case 3:
            enemy[9].pos = vec3(-render.pos.x, 3, -render.pos.z);
            break;
        }
    }
    for(int i = 0; i!=2; i++){
        door[i].changepos(vertex);
    }
    for(int i = 0; i!=2; i++){
        KeyCard[i].changepos(vertex);
    }
    for(int i = 0; i!=10; i++){
        enemy[i].changepos(vertex);
    }
    Exit.changepos(vertex);
}

void uisetup(){
    playbtn.v1 = vec2(-1, -0.3);
    playbtn.v2 = vec2(-1, 0);
    playbtn.v3 = vec2(-0.7, 0);
    playbtn.v4 = vec2(-0.7, -0.3);
    playbtn.uv1 = vec2(0.021, 0.698);
    playbtn.uv2 = vec2(0.021, 0.783);
    playbtn.uv3 = vec2(0.2, 0.783);
    playbtn.uv4 = vec2(0.2, 0.698);
    playbtn.Setup(render.totalv, 1000, 1000, 1000);

    settingsbtn.v1 = vec2(-1, -0.6);
    settingsbtn.v2 = vec2(-1, -0.3);
    settingsbtn.v3 = vec2(-0.5, -0.3);
    settingsbtn.v4 = vec2(-0.5, -0.6);
    settingsbtn.uv1 = vec2(0.2, 0.698);
    settingsbtn.uv2 = vec2(0.2, 0.783);
    settingsbtn.uv3 = vec2(0.48, 0.783);
    settingsbtn.uv4 = vec2(0.48, 0.698);
    settingsbtn.Setup(render.totalv, 1000, 1000, 1000);

    quitbtn.v1 = vec2(-1, -0.85);
    quitbtn.v2 = vec2(-1, -0.6);
    quitbtn.v3 = vec2(-0.7, -0.6);
    quitbtn.v4 = vec2(-0.7, -0.85);
    quitbtn.uv1 = vec2(0.49, 0.698);
    quitbtn.uv2 = vec2(0.49, 0.783);
    quitbtn.uv3 = vec2(0.68, 0.783);
    quitbtn.uv4 = vec2(0.68, 0.698);
    quitbtn.Setup(render.totalv, 1000, 1000, 1000);

    reskbtn.v1 = vec2(0.2, 0.8);
    reskbtn.v2 = vec2(0.2, 1);
    reskbtn.v3 = vec2(1);
    reskbtn.v4 = vec2(1, 0.8);
    reskbtn.uv1 = vec2(0.026, 0.623);
    reskbtn.uv2 = vec2(0.026, 0.708);
    reskbtn.uv3 = vec2(0.78, 0.708);
    reskbtn.uv4 = vec2(0.78, 0.623);
    reskbtn.Setup(render.totalv, 1000, 1000, 1000);

    respbtn.v1 = vec2(0.5, 0.6);
    respbtn.v2 = vec2(0.5, 0.8);
    respbtn.v3 = vec2(1, 0.8);
    respbtn.v4 = vec2(1, 0.6);
    respbtn.uv1 = vec2(0.023, 0.551);
    respbtn.uv2 = vec2(0.023, 0.63);
    respbtn.uv3 = vec2(0.38, 0.63);
    respbtn.uv4 = vec2(0.38, 0.551);
    respbtn.Setup(render.totalv, 1000, 1000, 1000);

    reshalfkbtn.v1 = vec2(0.5, 0.4);
    reshalfkbtn.v2 = vec2(0.5, 0.6);
    reshalfkbtn.v3 = vec2(1, 0.6);
    reshalfkbtn.v4 = vec2(1, 0.4);
    reshalfkbtn.uv1 = vec2(0.39, 0.551);
    reshalfkbtn.uv2 = vec2(0.39, 0.63);
    reshalfkbtn.uv3 = vec2(0.75, 0.63);
    reshalfkbtn.uv4 = vec2(0.75, 0.551);
    reshalfkbtn.Setup(render.totalv, 1000, 1000, 1000);

    resfullhd.v1 = vec2(0.5, 0.2);
    resfullhd.v2 = vec2(0.5, 0.4);
    resfullhd.v3 = vec2(1, 0.4);
    resfullhd.v4 = vec2(1, 0.2);
    resfullhd.uv1 = vec2(0.021, 0.476);
    resfullhd.uv2 = vec2(0.021, 0.56);
    resfullhd.uv3 = vec2(0.37, 0.56);
    resfullhd.uv4 = vec2(0.37, 0.476);
    resfullhd.Setup(render.totalv, 1000, 1000, 1000);

    reshdpbtn.v1 = vec2(0.5, 0);
    reshdpbtn.v2 = vec2(0.5, 0.2);
    reshdpbtn.v3 = vec2(1, 0.2);
    reshdpbtn.v4 = vec2(1, 0);
    reshdpbtn.uv1 = vec2(0.391, 0.476);
    reshdpbtn.uv2 = vec2(0.391, 0.56);
    reshdpbtn.uv3 = vec2(0.713, 0.56);
    reshdpbtn.uv4 = vec2(0.713, 0.476);
    reshdpbtn.Setup(render.totalv, 1000, 1000, 1000);

    reshdbtn.v1 = vec2(0.5, -0.2);
    reshdbtn.v2 = vec2(0.5, 0);
    reshdbtn.v3 = vec2(1, 0);
    reshdbtn.v4 = vec2(1, -0.2);
    reshdbtn.uv1 = vec2(0.021, 0.409);
    reshdbtn.uv2 = vec2(0.021, 0.48);
    reshdbtn.uv3 = vec2(0.343, 0.48);
    reshdbtn.uv4 = vec2(0.343, 0.409);
    reshdbtn.Setup(render.totalv, 1000, 1000, 1000);

    fullrhd.v1 = vec2(0.3, -0.4);
    fullrhd.v2 = vec2(0.3, -0.2);
    fullrhd.v3 = vec2(1, -0.2);
    fullrhd.v4 = vec2(1, -0.4);
    fullrhd.uv1 = vec2(0.026, 0.256);
    fullrhd.uv2 = vec2(0.026, 0.416);
    fullrhd.uv3 = vec2(0.718, 0.416);
    fullrhd.uv4 = vec2(0.718, 0.256);
    fullrhd.Setup(render.totalv, 1000, 1000, 1000);

    shadowbtn.v1 = vec2(0.5, -0.6);
    shadowbtn.v2 = vec2(0.5, -0.4);
    shadowbtn.v3 = vec2(1, -0.4);
    shadowbtn.v4 = vec2(1, -0.6);
    shadowbtn.uv1 = vec2(0.026, 0.189);
    shadowbtn.uv2 = vec2(0.026, 0.266);
    shadowbtn.uv3 = vec2(0.421, 0.266);
    shadowbtn.uv4 = vec2(0.421, 0.186);
    shadowbtn.Setup(render.totalv, 1000, 1000, 1000);

    fullscreenbtn.v1 = vec2(0.5, -0.8);
    fullscreenbtn.v2 = vec2(0.5, -0.6);
    fullscreenbtn.v3 = vec2(1, -0.6);
    fullscreenbtn.v4 = vec2(1, -0.8);
    fullscreenbtn.uv1 = vec2(0.026, 0.114);
    fullscreenbtn.uv2 = vec2(0.026, 0.192);
    fullscreenbtn.uv3 = vec2(0.307, 0.191);
    fullscreenbtn.uv4 = vec2(0.307, 0.114);
    fullscreenbtn.Setup(render.totalv, 1000, 1000, 1000);
    render.totalv+=6;
}

bool fullscreen = false;

bool settings = false;

void readsettings(){
    ifstream cfg;
    cfg.open("App/User/GameSettings.cfg");
    string param;
    int value;
    int resx = 0;
    int resy = 0;
    while(cfg >> param >> value){
        if(param == "resx"){
            resx = value;
            render.resolution.x = value;
        }
        if(param == "resy"){
            resy = value;
            render.resolution.y = value;
        }
        if(param == "fullscreen"){
            switch(value){
                case 0:
                fullscreen = false;
                glfwSetWindowMonitor(render.window, NULL, 100, 100, resx, resy, 60);
                break;
                case 1:
                fullscreen = true;
                glfwSetWindowMonitor(render.window, glfwGetPrimaryMonitor(), 0, 0, resx, resy, 60);
                break;
            }
        }
        if(param == "shadows"){
            switch(value){
                case 0:
                Engine.enableshadows = false;
                break;
                case 1:
                Engine.enableshadows = true;
                break;
            }
        }
        if(param == "shadowmapres"){
            render.shadowmapresolution = value;
        }
        if(param == "renderresfactor"){
            Engine.scalingFactor = value;
        }
    }
    glfwSetWindowSize(render.window, resx, resy);
}

void writesettings(){
    ofstream cfg;
    cfg.open("App/User/GameSettings.cfg");
    cfg << "resx " << render.resolution.x << endl;
    cfg << "resy " << render.resolution.y << endl;
    cfg << "fullscreen " << fullscreen << endl;
    cfg << "shadows " << Engine.enableshadows << endl;
    cfg << "shadowmapres " << render.shadowmapresolution << endl;
    cfg << "renderresfactor " << Engine.scalingFactor << endl;
}

bool resolutionset = true;

void uiloop(){
    if(resolutionset == false){
        switch(fullscreen){
            case 0:
            glfwSetWindowMonitor(render.window, NULL, 100, 100, render.resolution.x, render.resolution.y, 60);
            break;
            case 1:
            glfwSetWindowMonitor(render.window, glfwGetPrimaryMonitor(), 0, 0, render.resolution.x, render.resolution.y, 60);
            break;
        }
        resolutionset = true;
    }
    if(pause == true){
        playbtn.Update(vertex, Engine.rawm, render.resolution, st);
        settingsbtn.Update(vertex, Engine.rawm, render.resolution, st);
        quitbtn.Update(vertex, Engine.rawm, render.resolution, st);
        if(settingsbtn.pressed == true){
            switch(settings){
                case 0:
                settings = true;
                break;
                case 1:
                settings = false;
                break;
            }
        }   
        if(playbtn.pressed == true){
            pause = false;
            Engine.showmouse = false;
            Engine.mouselook = true;
        }
        if(quitbtn.pressed == true){
            glfwSetWindowShouldClose(render.window, 1);
        }
        if(settings == true){
            reskbtn.Update(vertex, Engine.rawm, render.resolution, st);
            respbtn.Update(vertex, Engine.rawm, render.resolution, st);
            reshalfkbtn.Update(vertex, Engine.rawm, render.resolution, st);
            resfullhd.Update(vertex, Engine.rawm, render.resolution, st);
            reshdpbtn.Update(vertex, Engine.rawm, render.resolution, st);
            reshdbtn.Update(vertex, Engine.rawm, render.resolution, st);
            fullrhd.Update(vertex, Engine.rawm, render.resolution, st);
            shadowbtn.Update(vertex, Engine.rawm, render.resolution, st);
            fullscreenbtn.Update(vertex, Engine.rawm, render.resolution, st);
            if(reskbtn.pressed == true){
                glfwSetWindowSize(render.window, 3840, 2160);
                writesettings();
            }
            if(respbtn.pressed == true){
                glfwSetWindowSize(render.window, 2560, 1440);
                writesettings();
            }
            if(reshalfkbtn.pressed == true){
                glfwSetWindowSize(render.window, 2048, 1080);
                writesettings();
            }
            if(resfullhd.pressed == true){
                glfwSetWindowSize(render.window, 1920, 1080);
                writesettings();
            }
            if(reshdpbtn.pressed == true){
                glfwSetWindowSize(render.window, 1600, 900);
                writesettings();
            }
            if(reshdbtn.pressed == true){
                glfwSetWindowSize(render.window, 1280, 720);
                writesettings();
            }
            if(fullrhd.pressed == true){
                switch(Engine.scalingFactor){
                    case 1:
                    Engine.scalingFactor = 2;
                    break;
                    case 2:
                    Engine.scalingFactor = 1;
                    break;
                }
                render.RenderResolution.x = render.resolution.x / Engine.scalingFactor;
                render.RenderResolution.y = render.resolution.y / Engine.scalingFactor;
                render.Renewswap();
                writesettings();
            }
            if(fullscreenbtn.pressed == true){
                switch(Engine.enableshadows){
                    case 0:
                    Engine.enableshadows = true;
                    break;
                    case 1:
                    Engine.enableshadows = false;
                    break;
                }
                writesettings();
            }
            if(shadowbtn.pressed == true){
                resolutionset = false;
                switch(fullscreen){
                    case 0:
                    fullscreen = true;
                    break;
                    case 1:
                    fullscreen = false;
                    break;
                }
                writesettings();
            }
            st = 0;
        }else{
            for(int i = render.totalv-60; i!=render.totalv;i++){
                vertex[i].vertexpos = vec4(0);
            }
        }
        GuiDrawRectangle(1000, vec2(-1, 0.5), vec2(-1, 1), vec2(-0.2, 1), vec2(-0.2, 0.5), vec2(0, 0.8), vec2(0, 1), vec2(0.53, 1), vec2(0.53, 0.8), vertex, render.totalv-6);
    }else{
        for(int i = render.totalv-78; i!=render.totalv;i++){
            vertex[i].vertexpos = vec4(0);
        }
    }
}

void levelProcces(){
    switch(level){
        case 1:
        if(loaded == false){
            render.pos = vec3(0, 5, 0);
            hp = 100;
            ehp = 100;
            render.totalv = 0;
            Engine.objwork("App/Models/Level1.obj", 0, vec3(0, 0, 0), 0);
            cout << "HellPlaces:App/Models/Level1.obj loaded" << endl;
            Engine.objwork("App/Models/gun.obj", render.totalv, vec3(0, 0, 0), 2000);
            cout << "HellPlaces:App/Models/gun.obj loaded" << endl;
            loaded = true;
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(door[i], "App/Models/door.obj", 0, vec2(1), vec2(1), 0, 0);
                door[i].enablec = false;
                door[i].enablep = false;
                door[i].enablepip = false;
            }
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(KeyCard[i], "App/Models/keycard.obj", 0, vec2(1), vec2(1), 0, 0);
                KeyCard[i].enablec = false;
                KeyCard[i].enablep = false;
                KeyCard[i].enablepip = false;
            }
            for(int i = 0; i!=10; i++){
                Engine.CreateProp(enemy[i], "App/Models/Enemy.obj", 0, vec2(0.2), vec2(1), 0, 0);
            }
            Engine.CreateProp(Exit, "App/Models/Portal.obj", 0, vec2(0.2, 0), vec2(1), 0, 0);
            for(int i = 0; i!=2;i++){
                keyactive[i] = false;
                doorused[i] = false;
            }
            hpbar.setup(render.totalv, 6000);
            loadLuciferFile();
            uisetup();
        }
        break;
        case 2:
        if(loaded == false){
            render.pos = vec3(0, 5, 0);
            hp = 100;
            ehp = 100;
            render.totalv = 0;
            Engine.objwork("App/Models/Level2.obj", 0, vec3(0, 0, 0), 0);
            cout << "HellPlaces:App/Models/Level2.obj loaded" << endl;
            Engine.objwork("App/Models/gun.obj", render.totalv, vec3(0, 0, 0), 2000);
            cout << "HellPlaces:App/Models/gun.obj loaded" << endl;
            loaded = true;
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(door[i], "App/Models/door.obj", 0, vec2(0.2), vec2(1), 0, 0);
                door[i].enablec = false;
                door[i].enablep = false;
                door[i].enablepip = false;
            }
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(KeyCard[i], "App/Models/keycard.obj", 0, vec2(0.2), vec2(1), 0, 0);
                KeyCard[i].enablec = false;
                KeyCard[i].enablep = false;
                KeyCard[i].enablepip = false;
            }
            for(int i = 0; i!=10; i++){
                Engine.CreateProp(enemy[i], "App/Models/Enemy.obj", 0, vec2(0.2), vec2(1), 0, 0);
            }
            Engine.CreateProp(Exit, "App/Models/Portal.obj", 0, vec2(0.2, 0), vec2(1), 0, 0);
            for(int i = 0; i!=2;i++){
                keyactive[i] = false;
                doorused[i] = false;
            }
            hpbar.setup(render.totalv, 6000);
            loadLuciferFile();
            uisetup();
        }
        break;
        case 3:
        if(loaded == false){
            render.pos = vec3(0, 5, 0);
            hp = 100;
            ehp = 100;
            render.totalv = 0;
            Engine.objwork("App/Models/Level3.obj", 0, vec3(0, 0, 0), 0);
            cout << "HellPlaces:App/Models/Level3.obj loaded" << endl;
            Engine.objwork("App/Models/gun.obj", render.totalv, vec3(0, 0, 0), 2000);
            cout << "HellPlaces:App/Models/gun.obj loaded" << endl;
            loaded = true;
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(door[i], "App/Models/door.obj", 0, vec2(0.2), vec2(1), 0, 0);
                door[i].enablec = false;
                door[i].enablep = false;
                door[i].enablepip = false;
            }
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(KeyCard[i], "App/Models/keycard.obj", 0, vec2(0.2), vec2(1), 0, 0);
                KeyCard[i].enablec = false;
                KeyCard[i].enablep = false;
                KeyCard[i].enablepip = false;
            }
            for(int i = 0; i!=10; i++){
                Engine.CreateProp(enemy[i], "App/Models/Enemy.obj", 0, vec2(0.2), vec2(1), 0, 0);
            }
            Engine.CreateProp(Exit, "App/Models/Portal.obj", 0, vec2(0.2, 0), vec2(1), 0, 0);
            for(int i = 0; i!=2;i++){
                keyactive[i] = false;
                doorused[i] = false;
            }
            hpbar.setup(render.totalv, 6000);
            loadLuciferFile();
            uisetup();
        }
        break;
        case 4:
        if(loaded == false){
            render.pos = vec3(0, 5, 0);
            hp = 100;
            ehp = 100;
            render.totalv = 0;
            Engine.objwork("App/Models/Level4.obj", 0, vec3(0, 0, 0), 0);
            cout << "HellPlaces:App/Models/Level4.obj loaded" << endl;
            Engine.objwork("App/Models/gun.obj", render.totalv, vec3(0, 0, 0), 2000);
            cout << "HellPlaces:App/Models/gun.obj loaded" << endl;
            loaded = true;
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(door[i], "App/Models/door.obj", 0, vec2(0.2), vec2(1), 0, 0);
                door[i].enablec = false;
                door[i].enablep = false;
                door[i].enablepip = false;
            }
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(KeyCard[i], "App/Models/keycard.obj", 0, vec2(0.2), vec2(1), 0, 0);
                KeyCard[i].enablec = false;
                KeyCard[i].enablep = false;
                KeyCard[i].enablepip = false;
            }
            for(int i = 0; i!=10; i++){
                Engine.CreateProp(enemy[i], "App/Models/Enemy.obj", 0, vec2(0.2), vec2(1), 0, 0);
            }
            Engine.CreateProp(Exit, "App/Models/Portal.obj", 0, vec2(0.2, 0), vec2(1), 0, 0);
            for(int i = 0; i!=2;i++){
                keyactive[i] = false;
                doorused[i] = false;
            }
            hpbar.setup(render.totalv, 6000);
            loadLuciferFile();
            uisetup();
        }
        break;
        case 5:
        if(loaded == false){
            render.pos = vec3(0, 5, 0);
            hp = 100;
            ehp = 100;
            render.totalv = 0;
            Engine.objwork("App/Models/Level5.obj", 0, vec3(0, 0, 0), 0);
            cout << "HellPlaces:App/Models/Level5.obj loaded" << endl;
            Engine.objwork("App/Models/gun.obj", render.totalv, vec3(0, 0, 0), 2000);
            cout << "HellPlaces:App/Models/gun.obj loaded" << endl;
            loaded = true;
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(door[i], "App/Models/door.obj", 0, vec2(0.2), vec2(1), 0, 0);
                door[i].enablec = false;
                door[i].enablep = false;
                door[i].enablepip = false;
            }
            for(int i = 0; i!=2; i++){
                Engine.CreateProp(KeyCard[i], "App/Models/keycard.obj", 0, vec2(0.2), vec2(1), 0, 0);
                KeyCard[i].enablec = false;
                KeyCard[i].enablep = false;
                KeyCard[i].enablepip = false;
            }
            for(int i = 0; i!=10; i++){
                Engine.CreateProp(enemy[i], "App/Models/Enemy.obj", 0, vec2(0.2), vec2(1), 0, 0);
            }
            Engine.CreateProp(Exit, "App/Models/Portal.obj", 0, vec2(0.2, 0), vec2(1), 0, 0);
            for(int i = 0; i!=2;i++){
                keyactive[i] = false;
                doorused[i] = false;
            }
            hpbar.setup(render.totalv, 6000);
            loadLuciferFile();
            uisetup();
        }
        break;
        case 6:
        if(loaded == false){
            render.pos = vec3(0, 5, 0);
            hp = 100;
            render.totalv = 0;
            Engine.objwork("App/Models/Level6.obj", 0, vec3(0, 0, 0), 0);
            cout << "HellPlaces:App/Models/Level6.obj loaded" << endl;
            Engine.objwork("App/Models/gun.obj", render.totalv, vec3(0, 0, 0), 2000);
            cout << "HellPlaces:App/Models/gun.obj loaded" << endl;
            Engine.CreateProp(enemy[0], "App/Models/Enemy.obj", 0, vec2(0.2), vec2(1), 0, 0);
            enemy[0].pos =  vec3(0, 3, -10);
            enemy[0].changepos(vertex);
            loaded = true;
            hpbar.setup(render.totalv, 6000);
            ehpbar.v1 = vec2(-1, -0.9);
            ehpbar.v2 = vec2(-1, -0.8);
            ehpbar.setup(render.totalv, 6000);
            ehp = 100;
            uisetup();
        }
        break;
    }
}

void logicProces(){
    calcbullet();
    for(int i = 0; i!=2; i++){
        door[i].PropPosInteract(render.pos, Engine.speed);
        if(door[i].playerinteracted == true && keyactive[i] == true){
            cout << "HellPlaces: You Opened Door No " << i << endl;
            doorused[i] = true;
            door[i].pos = vec3(1000, 0, 0);
            door[i].changepos(vertex);
        }
    }
    for(int i = 0; i!=2; i++){
        KeyCard[i].PropPosInteract(render.pos, Engine.speed);
        if(KeyCard[i].playerinteracted == true){
            keyactive[i] = true;
            cout << "HellPlaces: You Picked Up KeyCard No " << i << endl;
            KeyCard[i].pos = vec3(1000, 0, 0);
            KeyCard[i].changepos(vertex);
        }
    }
    for(int i = 0; i!=10; i++){
        if(pause == false){
            propAi(enemy[i]);
        }
        enemy[i].updateProp(vertex, vec3(render.ubo.massive[12].x, render.ubo.massive[12].y, render.ubo.massive[12].z), Engine.speed);
        if(enemy[i].playerinteracted == true){
            switch(level){
                case 6:
                ehp--;
                break;
                default:
                enemy[i].pos = vec3(-1000);
                break;
            }
        }
        enemy[i].PropPosInteract(render.pos, Engine.speed);
        if(enemy[i].playerinteracted == true && enemy[i].pos.y > 0){
            hp-=20;
        }
    }
    if(hp <= 0){
        loadsave();
    }
    Exit.PropPosInteract(render.pos, Engine.speed);
    if(Exit.playerinteracted == true){
        render.pos = vec3(0, 5, 0);
        loaded = false;
        level++;
    }
    if(render.pos.y < 0){
        hp = 0;
        ehp = 100;
    }
    if(enemy[0].pos.y < 0){
        ehp = 0;
    }
    hpbar.Update(vertex, hp, 100);
    if(level == 6){
        ehpbar.Update(vertex, ehp, 100);
    }
    uiloop();
    if(ehp <= 0 && level == 6){
        enemy[0].pos = vec3(-1000);
        GuiDrawRectangle(9000, vec2(-0.8), vec2(-0.8, 0.8), vec2(0.8), vec2(0.8, -0.8), vec2(0), vec2(0, 1), vec2(1), vec2(1, 0), vertex, render.totalv-6);
        if(pause == true){
            level = 1;
            ehp = 100;
            loaded = false;
        }
    }
}

int main(){
    render.TexToLoad[0] = "App/Textures/MainTexture.ppm";
    render.TexToLoad[1] = "App/Textures/MainSpecular.ppm";
    render.TexToLoad[2] = "App/Textures/text_menus.ppm";
    render.TexToLoad[3] = "App/Textures/final_text.ppm";
    render.TexToLoadCnt = 4;
    render.pos.y = 5;
    render.enablelayers = false;
    Engine.mouselook = false;
    Engine.showmouse = true;
    Engine.collisionenable = true;
    Engine.enablephysics = true;
    Engine.enableshadows = true;

    Engine.aditionalPipelineCounts = 1;
    Engine.fshaderpath[0] = "App/Raw/skyf.spv";
    Engine.vshaderpath[0] = "App/Raw/skyv.spv";
    Engine.CullMode[0] = VK_CULL_MODE_NONE;
    Engine.polyMode[0] = VK_POLYGON_MODE_FILL;

    Engine.Init();  

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    string tittle;
    double currentTime;
    pSpeed = 0.5;

    level = 1;

    begtexpos = 0;

    int resx = 0, resy = 0;

    readImage(pixels, resx, resy, "App/icon.ppm");

    cout << "HellPlaces:App/icon.ppm loaded" << endl;

    GLFWimage icon;

    icon.pixels = pixels;

    icon.height = resy;

    icon.width = resx;

    glfwSetWindowIcon(render.window, 1, &icon);

    cout << "HellPlaces:new icon applied" << endl;

    mat4 lightProj = ortho(-60.0f, 60.0f, -60.0f, 60.0f, 1.0f, 145.0f);
    mat4 lightView = lookAt(vec3(30.0f, -30.0f, 30.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    render.ubo.lightmat = lightProj * lightView;

    readsettings();

    while(!glfwWindowShouldClose(render.window)){
        glfwPollEvents();
        render.fov = usrfov;
        glfwSetMouseButtonCallback(render.window, mouse_button_callback);
        lightView = lookAt(vec3(65.0f-render.pos.x, -65.0f, 65.0f-render.pos.z), vec3(-render.pos.x, 0, -render.pos.z), vec3(0.0f, 1.0f, 0.0f));
        render.ubo.lightmat = lightProj * lightView;
        Engine.Update(HellCallback);
        levelProcces();
        logicProces();
        currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){
            tittle = to_string(nbFrames);
            glfwSetWindowTitle(render.window, tittle.c_str());
            nbFrames = 0;
            lastTime += 1.0;
        }
    }
    Engine.End();
    glfwDestroyWindow(render.window);
}
