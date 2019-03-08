#ifndef APP_H
#define APP_H

#include "stdafx.h"
#include "ShaderProgram.hpp"
#include "objloader.hpp"
#include "camera.h"

using namespace std;
using glm::vec3;
using glm::vec2;
using glm::mat4;
using glm::mat3;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

class App {
  public:

    App();
    ~App();
    void run();
  private:
    //DATA
    unique_ptr<ShaderProgram> passthrough;
    unique_ptr<ShaderProgram> shaderProgram;
    //render data
    GLuint suzanne;
    GLuint canvas;
    GLuint volumeTexture;
    const uint voxelDim = 512;
    const glm::ivec3 gridDim = glm::ivec3(voxelDim,voxelDim,voxelDim);
    GLuint VAO;
    GLuint VBO;
    GLuint FBO;
    glm::mat4 model, view, proj, MVPx, MVPy, MVPz, viewFront, viewRight, viewTop;
    //atomic counter
    GLuint numVoxelsHandle;
    GLuint numVoxelFragments = 0;
    //mesh containers
    vector<glm::vec3> verts;
    vector<glm::vec2> uvs;
    vector<glm::vec3> normals;

    GLuint renderTexture;
    GLuint positionAttribute, colAttrib, uniColor;
    SDL_Window* window = NULL;
    SDL_GLContext context;
    Camera cam;
    bool quit = false;
    //ortho matrices
    glm::mat4 orthoViewMatArray[3] = {viewFront, viewRight, viewTop};

    //METHODS
    void init();
    void setupGL();
    void setupShaders();
    void setupModel();
    void setupMesh();
    void setup3DTexture();
    void setupCanvas();
    bool setupFBO();
    void setupAppParams();

    //For run
    void handleEvents();
    void render();
};

#endif
