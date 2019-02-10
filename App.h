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
    GLuint VAO;
    GLuint VBO;
    GLuint FBO;
    glm::mat4 model, view, proj, MVP;
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

    //METHODS
    int init();
    int setupGL();
    void setupShaders();
    void setupModel();
    void setupMesh();
    void setupCanvas();
    int setupFBO();
    void setupAppParams();

    //For run
    void handleEvents();
    void render();
};

#endif
