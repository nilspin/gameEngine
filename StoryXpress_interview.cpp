#include "stdafx.h"
#include "camera.h"
#include "ShaderProgram.hpp"
#include "Mesh.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint VAO;
GLuint VBO;
GLuint positionAttribute, colAttrib, uniColor;

void initGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

}

int main(int argc, char *argv[])
{
	/* Don't use camera for now*/
	Camera cam;
	cam.SetMode(FREE);
	cam.SetPosition(glm::vec3(0, 0, -10));
	cam.SetLookAt(glm::vec3(0, 0, 0));
	cam.SetClipping(.01, 50);
	cam.SetFOV(45);

	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 200, 30, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}

#pragma region SHADER_FUNCTIONS

	//=============================================================================================

	ShaderProgram *shaderProgram = new ShaderProgram();
	shaderProgram->initFromFiles("MainShader.vert", "MainShader.frag");

	shaderProgram->addAttribute("position");
	shaderProgram->addAttribute("color");

	shaderProgram->addUniform("MVP");

	shaderProgram->use();


#pragma endregion SHADER_FUNCTIONS

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
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f,
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

	GLfloat canvas[] = {		//DATA
		-1.0f,-1.0f, -3.0f,
		-1.0f, 1.0f, -3.0f,
		1.0f, -1.0f, -3.0f,
		1.0f, -1.0f, -3.0f,
		1.0f, 1.0f, -3.0f,
		-1.0f, 1.0f, -3.0f
	};	//Don't need index data for this peasant mesh!

	GLuint WallVBO;//VBO for fluid wall
	glGenBuffers(1, &WallVBO);
	glBindBuffer(GL_ARRAY_BUFFER, WallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvas), &canvas, GL_STATIC_DRAW);
	//Assign attribs
	glVertexAttribPointer(shaderProgram->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderProgram->attribute("position"));

	GLfloat WallColor[] =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
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
	glBindVertexArray(0);	//unbind VAO


	/*Framebuffer shit
	GLuint FBO;
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//now create a texture
	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	*/

	/*Object loading shit*/
//	Mesh *suzanne = new Mesh("suzanne.obj");
	//	suzanne->upload();
	//	vector<glm::vec4> suzanne_verts;
	//	vector<glm::vec3> suzanne_normals;
	//	vector<char16_t> suzanne_elements;


	glm::mat4 model; //define a transformation matrix for model in local coords

	glm::mat4 view = glm::lookAt(
		glm::vec3(4.2f, 4.2f, 4.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);	//view matrix

	glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 100.0f);	//projection matrix

	glm::mat4 MVP;

	glEnable(GL_DEPTH_TEST); //wierd shit happens if you don't do this
	glEnable(GL_BLEND);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	//clear screen

	//here comes event handling part
	SDL_Event e;
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
					cam.Move(FORWARD);
					std::cout << "W pressed \n";
					break;

				case SDLK_s:
					cam.Move(BACK);
					std::cout << "S pressed \n";
					break;

				case SDLK_a:
					cam.Move(LEFT);
					std::cout << "A pressed \n";
					break;

				case SDLK_d:
					cam.Move(RIGHT);
					std::cout << "D pressed \n";
					break;

				case SDLK_q:
					cam.Move(DOWN);
					std::cout << "Q pressed \n";
					break;

				case SDLK_e:
					cam.Move(UP);
					std::cout << "E pressed \n";
					break;

				}
				break;

			case SDL_MOUSEMOTION:

				cam.Move2D(e.motion.x, e.motion.y);
				std::cout << "mouse moved by x=" << e.motion.x << " y=" << e.motion.y << "\n";
				break;

			case SDL_MOUSEBUTTONDOWN:
				cam.SetPos(e.button.button, e.button.state, e.button.x, e.button.y);
				break;

			case SDL_MOUSEBUTTONUP:
				cam.SetPos(e.button.button, e.button.state, e.button.x, e.button.y);
				break;
			}
		}
#pragma endregion EVENT_HANDLING

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw wall
		glBindVertexArray(Wall);
		model = glm::mat4(1);
		MVP = proj*view*model;
		glUniformMatrix4fv(shaderProgram->uniform("MVP"), 1, FALSE, glm::value_ptr(MVP));

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//draw cube
		glBindVertexArray(VAO);
		//following 2 lines define "intensity" of color, i.e ranging from 0 to highest
		GLfloat time = SDL_GetTicks();
		glUniform1f(uniColor, 1.0f);// (sin(time*0.01f) + 1.0f) / 2.0f);

		model = glm::rotate(glm::mat4(1), time*0.005f, glm::vec3(0, 0, 1));	//calculate on the fly
		MVP = proj*view*model;
		glUniformMatrix4fv(shaderProgram->uniform("MVP"), 1, FALSE, glm::value_ptr(MVP));

		//		suzanne->draw();
		glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		SDL_GL_SwapWindow(window);
		//		if (1000 / FPS > SDL_GetTicks() - start)
		//			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}

	shaderProgram->disable();
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}