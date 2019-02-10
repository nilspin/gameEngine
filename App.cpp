#include "App.h"


App::App()  {
  init();
}

App::~App() {
	SDL_GL_DeleteContext(context);
	SDL_Quit();
}

void App::run()  {
  while(!quit)  {
    handleEvents();
    render();
  }
}
int App::init()  {
  setupGL();
  setupShaders();
  setupModel();
  setupFBO();
  setupAppParams();
}

void App::setupShaders()  {
  shaderProgram = unique_ptr<ShaderProgram>(new ShaderProgram());
	shaderProgram->initFromFiles("shaders/MainShader.vert", "shaders/MainShader.geom", "shaders/MainShader.frag");

	shaderProgram->addAttribute("position");
	shaderProgram->addAttribute("color");
	shaderProgram->addUniform("MVP");
	shaderProgram->use();

  passthrough = unique_ptr<ShaderProgram>(new ShaderProgram());
	passthrough->initFromFiles("shaders/passthrough.vert", "shaders/passthrough.frag");
	passthrough->addAttribute("position");
	passthrough->addUniform("sampler");

}

int App::setupGL()  {

	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	window = SDL_CreateWindow("SDL_project", 200, 30, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
	context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}

#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                     GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
#endif


	glEnable(GL_DEPTH_TEST); //wierd behaviour happens if we don't do this
	glEnable(GL_BLEND);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	//clear screen

}

void App::setupAppParams()  {
	cam.setPosition(glm::vec3(0, 0, 5));
	proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 100.0f);	//projection matrix
}
int App::setupFBO(){
	//Framebuffer shit
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//now create a texture
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 300, 300) ;
	//filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1,drawBuffers);//this is finally where we tell the driver to draw to this paricular framebuffer

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER,0);

}

void App::render()  {
  	//First things first
		cam.calcMatrices();

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glViewport(0, 0, 300, 300);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram->use();

		view = cam.getViewMatrix();
		GLfloat time = SDL_GetTicks();
    model = glm::mat4(1);
		//model = glm::rotate(glm::mat4(1), time*0.002f, glm::vec3(0, -1, 0));//	//calculate on the fly
		MVP = proj*view*model;
		glUniformMatrix4fv(shaderProgram->uniform("MVP"), 1, false, glm::value_ptr(MVP));


		glBindVertexArray(suzanne);
		glDrawArrays(GL_TRIANGLES, 0, verts.size());
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		//2nd pass : render everything on screen -- uncomment only for debug purposes
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		passthrough->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTexture);
		glUniform1i(passthrough->uniform("sampler"), 0);
		glBindVertexArray(canvas);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		SDL_GL_SwapWindow(window);
		//		if (1000 / FPS > SDL_GetTicks() - start)
		//			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));

}

void App::setupModel()  {
  setupMesh();
  setupCanvas();
}

void App::setupCanvas() {
    //=========================Canvas for texture sampling====================
    // The quad's FBO. Used only for visualizing the shadowmap.
    static const GLfloat g_quad_vertex_buffer_data[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
    };

    //Create Vertex Array Object
    glGenVertexArrays(1, &canvas);
    glBindVertexArray(canvas);

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    //Assign attribs
    glEnableVertexAttribArray(passthrough->attribute("position"));
    glVertexAttribPointer(passthrough->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);	//unbind VAO


}

void App::setupMesh() {
    bool res = loadOBJ("assets/suzanne.obj", verts, uvs, normals);

    glGenVertexArrays(1, &suzanne);
    glBindVertexArray(suzanne);

    GLuint suzanneVertVBO;//VBO for suzanne verts
    glGenBuffers(1, &suzanneVertVBO);
    glBindBuffer(GL_ARRAY_BUFFER, suzanneVertVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(glm::vec3), &verts[0], GL_STATIC_DRAW);
    //Assign attribs
    glVertexAttribPointer(shaderProgram->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderProgram->attribute("position"));

    //vertex color(we don't have separate color so just take normals for now)
    GLuint suzanneNormalVBO;//VBO for suzanne verts
    glGenBuffers(1, &suzanneNormalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, suzanneNormalVBO);

    /* UPLOAD NORMAL DATA
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);	//EDIT THIS LATER!!!
    //Assign attribs
    glVertexAttribPointer(shaderProgram->attribute("normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderProgram->attribute("normal"));
    */

    GLuint suzanneColorVBO;
    glGenBuffers(1, &suzanneColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, suzanneColorVBO);
    vector<glm::vec3> temp(verts.size(), glm::vec3(1, 0.5, 0));	//red color
    glBufferData(GL_ARRAY_BUFFER, temp.size()*sizeof(glm::vec3), &temp[0], GL_STATIC_DRAW);	//EDIT THIS LATER!!!
    //Assign attribs
    glVertexAttribPointer(shaderProgram->attribute("color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderProgram->attribute("color"));
    /*TEMP stuff*/
    glBindVertexArray(0);

}

void App::handleEvents() {

		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_QUIT:	//if X windowkey is pressed then quit
				quit = true;

			case SDL_KEYDOWN:	//if ESC is pressed then quit

				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;

				case SDLK_q:
					quit = true;
					break;

				case SDLK_w:
					cam.move(FORWARD);
					break;

				case SDLK_s:
					cam.move(BACK);
					break;

				case SDLK_a:
					cam.move(LEFT);
					break;

				case SDLK_d:
					cam.move(RIGHT);
					break;

				case SDLK_UP:
					cam.move(UP);
					break;

				case SDLK_DOWN:
					cam.move(DOWN);
					break;

				case SDLK_LEFT:
					cam.move(ROT_LEFT);
					break;

				case SDLK_RIGHT:
					cam.move(ROT_RIGHT);
					break;


				case SDLK_r:
					cam.reset();
					std::cout << "Camera reset \n";
					break;
				}
				break;

			case SDL_MOUSEMOTION:
				cam.rotate();
				break;
			}

		}
}

