#ifndef APP_H
#define APP_H

#include "stdafx.h"
#include "ShaderProgram.hpp"
#include "objloader.hpp"
#include "camera.h"

using namespace std;

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
    const uint voxelDim = 56;
    const glm::ivec3 gridDim = glm::ivec3(voxelDim,voxelDim,voxelDim);
    GLuint VAO;
    GLuint VBO;
    GLuint FBO;
    glm::mat4 model, view, proj, MVPx, MVPy, MVPz;
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
    glm::mat4 viewFront = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
		glm::mat4 viewRight = glm::lookAt(glm::vec3(-5,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));
		glm::mat4 viewTop = glm::lookAt(glm::vec3(0,5,0), glm::vec3(0,0,0), glm::vec3(0,0,-1));
    glm::mat4 orthoViewMatArray[3] = {viewFront, viewRight, viewTop};

    //METHODS
    int init();
    int setupGL();
    void setupShaders();
    void setupModel();
    void setupMesh();
    void setup3DTexture();
    void setupCanvas();
    int setupFBO();
    void setupAppParams();

    //For run
    void handleEvents();
    void render();
};

#endif
