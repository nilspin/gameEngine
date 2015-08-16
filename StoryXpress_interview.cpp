#include "stdafx.h"
#include "ShaderProgram.hpp"
#include "Mesh.h"
#include "objloader.hpp"
#include "camera.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint VAO;
GLuint VBO;
GLuint positionAttribute, colAttrib, uniColor;
SDL_Event e;
SDL_Window* window = NULL;

#pragma region CAMERA_CODE
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, -15);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

double lastTime = SDL_GetTicks();

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
//	static double lastTime = SDL_GetTicks();

	// Compute time difference between current and last frame
	double currentTime = SDL_GetTicks();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	int xpos = e.motion.x;
	int ypos = e.motion.y;

	// Reset mouse position for next frame
	SDL_WarpMouseInWindow(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (e.key.keysym.sym == SDLK_w && e.type == SDL_KEYDOWN){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (e.key.keysym.sym == SDLK_s && e.type == SDL_KEYDOWN){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (e.key.keysym.sym == SDLK_d && e.type == SDL_KEYDOWN){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (e.key.keysym.sym == SDLK_a && e.type == SDL_KEYDOWN){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
		);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

#pragma endregion CAMERA_CODE
int main(int argc, char *argv[])
{

#pragma region SDL_INIT
	
	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("SDL_project", 200, 30, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}

#pragma endregion SDL_INIT

#pragma region SHADER_FUNCTIONS

	//=============================================================================================

	unique_ptr<ShaderProgram> shaderProgram(new ShaderProgram());
	shaderProgram->initFromFiles("MainShader.vert", "MainShader.frag");

	shaderProgram->addAttribute("position");
	shaderProgram->addAttribute("color");
	shaderProgram->addAttribute("normal");

	shaderProgram->addUniform("MVP");
	shaderProgram->addUniform("V");
	shaderProgram->addUniform("M");
	shaderProgram->addUniform("LightPosition");
	shaderProgram->addUniform("depthBiasMVP");
	shaderProgram->addUniform("depthTexture");
	shaderProgram->use();

	unique_ptr<ShaderProgram> passthrough(new ShaderProgram());
	passthrough->initFromFiles("passthrough.vert", "passthrough.frag");
	passthrough->addAttribute("position");
	passthrough->addUniform("sampler");

	unique_ptr<ShaderProgram> depthWrite(new ShaderProgram());
	depthWrite->initFromFiles("depthWrite.vert","depthWrite.frag");
	depthWrite->addAttribute("position");
	depthWrite->addUniform("depthMVP");	//calculated from light's PoV

#pragma endregion SHADER_FUNCTIONS

#pragma region MODEL

	vector<glm::vec3> verts;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
	bool res = loadOBJ("suzanne.obj", verts, uvs, normals);

	GLuint suzanne;
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
	/*TEMP stuff*/
//	vector<glm::vec3> temp(verts.size(), glm::vec3(1, 0, 0));
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);	//EDIT THIS LATER!!!
	//Assign attribs
	glVertexAttribPointer(shaderProgram->attribute("normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("normal"));

	GLuint suzanneColorVBO;
	glGenBuffers(1, &suzanneColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, suzanneColorVBO);
	vector<glm::vec3> temp(verts.size(), glm::vec3(0.5, 0, 0));	//red color
	glBufferData(GL_ARRAY_BUFFER, temp.size()*sizeof(glm::vec3), &temp[0], GL_STATIC_DRAW);	//EDIT THIS LATER!!!
	//Assign attribs
	glVertexAttribPointer(shaderProgram->attribute("color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("color"));
	/*TEMP stuff*/
	glBindVertexArray(0);
#pragma endregion MODEL

#pragma region MESH
	//Create Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//create a vertex buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//define vertices of our triangle
	GLfloat vertices[] = {
		// front
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0
	};

	GLfloat colors[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 3 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), 8 * 4 * sizeof(GLfloat), colors);
	//Note that we use GL_STATIC_DRAW because we want to send data to GPU only once thoughout the
	//program

	/*Now specify the layout of the Vertex data */

	// The following line tells the CPU program that "vertexData" stuff goes into "posision"
	//parameter of the vertex shader.
	glVertexAttribPointer(shaderProgram->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("position"));

	glVertexAttribPointer(shaderProgram->attribute("color"), 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(shaderProgram->attribute("color"));

	GLushort indices[] = { 0, 1, 2, 2, 3, 0, 3, 2, 6, 6, 7, 3, 7, 6, 5, 5, 4, 7, 4, 5, 1, 1, 0, 4, 4, 0, 3, 3, 7, 4, 1, 5, 6, 6, 2, 1 };
	//define an index buffer
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glBindVertexArray(0);

	//==========================THE WALL========================================
	GLuint Wall;
	glGenVertexArrays(1, &Wall);
	glBindVertexArray(Wall);

	GLfloat Wall_vert[] = {		//DATA
		-10.0f,-3.0f, -10.0f,
		-10.0f, -3.0f, 10.0f,
		10.0f, -3.0f, -10.0f,
		10.0f, -3.0f, -10.0f,
		10.0f, -3.0f, 10.0f,
		-10.0f, -3.0f, 10.0f
	};	//Don't need index data for this peasant mesh!

	GLuint WallVBO;//VBO for fluid wall
	glGenBuffers(1, &WallVBO);
	glBindBuffer(GL_ARRAY_BUFFER, WallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Wall_vert), &Wall_vert, GL_STATIC_DRAW);
	//Assign attribs
	glVertexAttribPointer(shaderProgram->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("position"));

	GLfloat WallColor[] =
	{
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	GLuint WallColorVBO;//VBO for fluid wall
	glGenBuffers(1, &WallColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, WallColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WallColor), &WallColor, GL_STATIC_DRAW);
	//Assign attribs
	glVertexAttribPointer(shaderProgram->attribute("color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("color"));

	//wall normal
	GLfloat WallNormal[] = {		//DATA
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	GLuint WallNormalVBO;//VBO for fluid wall
	glGenBuffers(1, &WallNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, WallNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WallNormal), &WallNormal, GL_STATIC_DRAW);
	//Assign attribs
	glVertexAttribPointer(shaderProgram->attribute("normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("normal"));
	glBindVertexArray(0);	//unbind VAO

#pragma endregion MESH

#pragma region CANVAS
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

	GLuint canvas;
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

#pragma endregion CANVAS


#pragma region FBO_SHIT

	//Framebuffer shit
	GLuint FBO;
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//now create a texture
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER,0);

#pragma endregion FBO_SHIT

	glm::mat4 model; //define a transformation matrix for model in local coords

	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 3.0f, -20.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
		);	//view matrix(pos, target, up)

	glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 100.0f);	//projection matrix

	glm::mat4 MVP;
	
	glm::vec3 LightPos(0, 4, 3);//(2.5, 3, -5);
	
	glEnable(GL_DEPTH_TEST); //wierd shit happens if you don't do this
	glEnable(GL_BLEND);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	//clear screen

	//here comes event handling part
	bool quit = false;

	while (!quit)
	{
#pragma region EVENT_HANDLING

		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_QUIT:	//if X windowkey is pressed then quit
				quit = true;

			case SDL_KEYDOWN:	//if ESC is pressed then quit

				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;

				case SDLK_w:
					std::cout << "W pressed \n";
					break;

				case SDLK_s:
					std::cout << "S pressed \n";
					break;

				case SDLK_a:
					std::cout << "A pressed \n";
					break;

				case SDLK_d:
					std::cout << "D pressed \n";
					break;

				case SDLK_q:
					std::cout << "Q pressed \n";
					break;

				case SDLK_e:
					std::cout << "E pressed \n";
					break;

				}
				break;

			case SDL_MOUSEMOTION:

				std::cout << "mouse moved by x=" << e.motion.x << " y=" << e.motion.y << "\n";
				break;

/*			case SDL_MOUSEBUTTONDOWN:
				break;

			case SDL_MOUSEBUTTONUP:
				break;
*/
			}
		}
#pragma endregion EVENT_HANDLING

		//First things first
		computeMatricesFromInputs();
		//1st pass - write to depthTexture (from light's PoV)
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat time = SDL_GetTicks();
		glm::mat4 depthProj = glm::ortho<float>(-10,10,-10,10,-10,20);	//ortho projection matrix
		glm::mat4 depthView = glm::lookAt(LightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 depthModel = glm::rotate(glm::mat4(1), time*0.002f, glm::vec3(0, 1, 0));	//calculate on the fly glm::mat4(1);	//identity for now
		glm::mat4 depthMVP = depthProj*depthView*depthModel;

		depthWrite->use();
		glUniformMatrix4fv(depthWrite->uniform("depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));

		glBindVertexArray(suzanne);
		glDrawArrays(GL_TRIANGLES, 0, verts.size());
		glBindVertexArray(0);	//unbind suzanne
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//2nd pass : sample from depthBuffer and test occlusion
		glViewport(0, 0, 1024, 768);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);
		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
		shaderProgram->use();
		//add view matrix code here
		view = getViewMatrix();
		model = glm::translate(glm::mat4(1), glm::vec3(0, 0, -10));//glm::rotate(glm::mat4(1), time*0.002f, glm::vec3(0, 1, 0));	//calculate on the fly
		MVP = proj*view*model;
		glUniformMatrix4fv(shaderProgram->uniform("MVP"), 1, FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(shaderProgram->uniform("M"), 1, FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(shaderProgram->uniform("V"), 1, FALSE, glm::value_ptr(view));
		glUniform3f(shaderProgram->uniform("LightPosition"), LightPos.x, LightPos.y, LightPos.z);
		glUniformMatrix4fv(shaderProgram->uniform("depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(shaderProgram->uniform("depthTexture"), 0);
		
		glBindVertexArray(suzanne);
		glDrawArrays(GL_TRIANGLES, 0, verts.size());
		glBindVertexArray(0);

		model = glm::translate(glm::mat4(1),glm::vec3(0,0,-10));
		MVP = proj*view*model;
		glUniformMatrix4fv(shaderProgram->uniform("MVP"), 1, FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(shaderProgram->uniform("M"), 1, FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(shaderProgram->uniform("V"), 1, FALSE, glm::value_ptr(view));
		glBindVertexArray(Wall);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//3rd pass : render everything on screen -- uncomment only for debug purposes
/*		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		passthrough->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(passthrough->uniform("sampler"), 0);
		glBindVertexArray(canvas);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
*/

		SDL_GL_SwapWindow(window);
		//		if (1000 / FPS > SDL_GetTicks() - start)
		//			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}