/*
Lab3start
Creates a cube and a sphere
Includes rotation, scaling, translation, view and perspective transformations,
and is setup for you to add lighting because the objects have normals and colours defined
for all vertices.
Feel free to either use this example as a start to lab3 or extract bits and add to
an example of your own to practice implementing per-vertex lighting.
Iain Martin October 2014
*/


/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glloadD.lib")
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>

/* Include GLM core and matrix extensions*/

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>
//#include <ft2build.h>
//#include FT_FREETYPE_H


/* Define buffer object indices */
GLuint positionBufferObject, colourObject, normalsBufferObject;
GLuint sphereBufferObject, sphereNormals, sphereColours;
GLuint elementbuffer;

GLuint program;		/* Identifier for the shader program */
GLuint vao;			/* Vertex array (Container) object. This is the index of the VAO that will be the container for
					our buffer objects */

GLuint colourmode;	/* Index of a uniform to switch the colour mode in the vertex shader
					I've included this to show you how to pass in an unsigned integer into
					your vertex shader. */

/* Position and view globals */
GLfloat angle_x, variable_x, x, scale, z, y;
GLfloat angle_y, variable_y, angle_z, variable_z;
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;	//Define the resolution of the sphere object

/* Uniforms*/
GLuint modelID, viewID, projectionID;
GLuint colourmodeID;

GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
GLuint numspherevertices;

//Values used for the camera movement 
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//value used to store the keys pressed for multi-touch algorithm.
bool keys[1024];


/* Function prototypes */
/* Note that a better design would be to make a sphere class. I've suggested that as one of the
extras to do in the lab for this week. */
void makeUnitSphere(GLfloat *pVertices, GLuint numlats, GLuint numlongs);
GLuint makeSphereVBO(GLuint numlats, GLuint numlongs);
void drawSphere();

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/


void init(GLWrapper *glw)
{
	/* Set the object transformation controls to their initial values */
	x = 0.05f;
	y = 0;
	z = 0;
	angle_x = angle_y = angle_z = 0;
	variable_x = variable_y = variable_z = 0;
	scale = 1.f;
	aspect_ratio = 1024.f / 768.f;	// Initial aspect ratio from window size (variables would be better!)
	colourmode = 0;
	numlats = 20;		// Number of latitudes in our sphere
	numlongs = 20;		// Number of longitudes in our sphere

	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	/* Define vertices for a cube in 12 triangles */
	GLfloat vertexPositions[] =
	{
		-0.25f, 0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f, 0.25f, -0.25f,
		-0.25f, 0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f, -0.25f, 0.25f,
		0.25f, 0.25f, -0.25f,

		0.25f, -0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, -0.25f,

		0.25f, -0.25f, 0.25f,
		-0.25f, -0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, 0.25f,
		-0.25f, 0.25f, 0.25f,
		0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, 0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, -0.25f,
		-0.25f, 0.25f, -0.25f,
		-0.25f, 0.25f, 0.25f,

		-0.25f, -0.25f, 0.25f,
		0.25f, -0.25f, 0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, 0.25f,

		-0.25f, 0.25f, -0.25f,
		0.25f, 0.25f, -0.25f,
		0.25f, 0.25f, 0.25f,

		0.25f, 0.25f, 0.25f,
		-0.25f, 0.25f, 0.25f,
		-0.25f, 0.25f, -0.25f,
	};

	/* Manually specified colours for our cube */
	float vertexColours[] = {
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
	};



	/* Manually specified normals for our cube */
	GLfloat normals[] =
	{
		0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
		0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
		1.f, 0, 0, 1.f, 0, 0, 1.f, 0, 0,
		1.f, 0, 0, 1.f, 0, 0, 1.f, 0, 0,
		0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
		0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
		-1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
		-1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
		0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
		0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
		0, 1.f, 0, 0, 1.f, 0, 0, 1.f, 0,
		0, 1.f, 0, 0, 1.f, 0, 0, 1.f, 0,
	};

	/* Create the vertex buffer for the cube */
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the colours buffer for the cube */
	glGenBuffers(1, &colourObject);
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the normals  buffer for the cube */
	glGenBuffers(1, &normalsBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec3), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* create the sphere object */
	numspherevertices = makeSphereVBO(numlats, numlongs);




	/* Load and build the vertex and fragment shaders */
	try
	{
		program = glw->LoadShader("lab3start.vert", "lab3start.frag");
	}
	catch (std::exception &e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	/* Define uniforms to send to vertex shader */
	modelID = glGetUniformLocation(program, "model");
	colourmodeID = glGetUniformLocation(program, "colourmode");
	viewID = glGetUniformLocation(program, "view");
	projectionID = glGetUniformLocation(program, "projection");

	//display controls in the cosole
	std::cout << "Move camera with W,A,S,D" << std::endl;
	std::cout << "Move whole arm with R for right direction, T for left direction, Y to stop" << std::endl;
	std::cout << "Move forearm with F for up direction, G for down direction, H to stop" << std::endl;
	std::cout << "Move wrist with V for up direction, B for down direction, N to stop" << std::endl;
	std::cout << "Press M to put color on the objects ( not recommended, looks better without ) " << std::endl;
	std::cout << "Press K to show the vertecies on the objects " << std::endl;

	

}


/* Called to update the display. Note that this function is called in the event loop in the wrapper
class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	/* Make the compiled shader program current */
	glUseProgram(program);

	/* Bind cube vertices. Note that this is in attribute index 0 */
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind cube colours. Note that this is in attribute index 1 */
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind cube normals. Note that this is in attribute index 2 */
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//cube
	glm::mat4 model = glm::mat4(1.0f);

	//Changed the projection matrix
	glm::mat4 Projection = glm::perspective(30.0f, aspect_ratio, 0.1f, 300.0f);

	// Camera matrix
	// Code reference http://learnopengl.com/#!Getting-started/Camera 
	glm::mat4 View = glm::lookAt(
		cameraPos, cameraPos + cameraFront, cameraUp
		);

	//Biceps cube 1
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, 0, 0));
	model = glm::scale(model, glm::vec3(scale , scale , scale ));
	
	// Send our transformations to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	//Forearm cube
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1.55f, 0 , 0));

	if (angle_y > -45 || angle_y < 45){
		model = glm::rotate(model, angle_y * 2, glm::vec3(0, 0, 1));
	}
	if (angle_y < -45){
		for(int i = 0; i < 45; i++){
		angle_y = angle_y;
		variable_y = -variable_y;
	}
	}
	if (angle_y > 45){
		for (int i = 45; i > 0; i--)
			angle_y = angle_y;
		variable_y = -variable_y;

	}

	model = glm::translate(model, glm::vec3((-1.15), 0, 0));
	model = glm::scale(model, glm::vec3(scale*3, scale, scale));

	// Send our transformations to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//Biceps cube 2
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0)); //rotating in clockwise direction around x-axis

	model = glm::translate(model, glm::vec3(-0.5, 0, 0));
	model = glm::scale(model, glm::vec3(scale , scale , scale ));//scale equally in all axis
	
	// Send our transformations to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	//Biceps cube 3
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1, 0, 0));
	model = glm::scale(model, glm::vec3(scale, scale, scale));

	// Send our transformations to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	//Wrist cube 
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-1.60, 0, 0));
	model = glm::rotate(model, angle_y*2, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(-1.55, 0, 0));

			if (angle_z > -45 || angle_z < 45){
				model = glm::rotate(model, angle_z * 2, glm::vec3(0, 0, 1));
			}
			if (angle_z < -45){
				angle_z = angle_z;
				variable_z = -variable_z;
			}
			if (angle_z > 45){
				angle_z = angle_z;
				variable_z = -variable_z;
			
			}

	model = glm::translate(model, glm::vec3(-0.7, 0, 0));
	model = glm::scale(model, glm::vec3(scale, scale, scale));//scale equally in all axis
	//rotating in clockwise direction around x-axis

	// Send our transformations to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	 model = glm::mat4(1.0f);
	 model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	 model = glm::translate(model, glm::vec3(0.5, 0, 0));
	 model = glm::scale(model, glm::vec3(scale, scale, scale));//scale equally in all axis

	// Send our uniforms variables to the currently bound shader
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform1ui(colourmodeID, colourmode);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);

	//Arm sphere
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x-0.1, y+0.6, 0));
	model = glm::scale(model, glm::vec3((scale + 0.0001f) / 2, (scale + 0.0001f) / 2 , (scale + 0.0001f)/ 2));//scale equally in all axis
	model = glm::rotate(model, 0.0f, glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model = glm::rotate(model, 0.0f, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model = glm::rotate(model, 0.0f, glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);

	//Forearm sphere
	drawSphere();
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model = glm::translate(model, glm::vec3((-1.6), 0, 0));
	model = glm::scale(model, glm::vec3((scale + 0.0001f) / 2, (scale + 0.0001f) / 2, (scale + 0.0001f) / 2));//scale equally in all axis
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	drawSphere();


	//Wrist sphere
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -angle_x, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model = glm::translate(model, glm::vec3(-1.6, 0, 0));
	model = glm::rotate(model, angle_y*2, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(-1.6, 0, 0));
	model = glm::scale(model, glm::vec3((scale + 0.0001f) / 2, (scale + 0.0001f) / 2, (scale + 0.0001f) / 2));//scale equally in all axis
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	drawSphere();

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	/* Modify our animation variables */
	angle_x += variable_x;
	angle_y += variable_y;
	angle_z += variable_z;
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = float(w) / float(h);
}


//Function for multi touch comands on the camera
void do_movement(){
	// Camera controls
	GLfloat cameraSpeed = 0.01f;
if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/
	//if (action != GLFW_PRESS) return;

if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == 'R') variable_x -= 0.03f;
	if (key == 'T') variable_x += 0.03f;
	if (key == 'Y') variable_x = 0.00f;
	if (key == 'F') variable_y -= 0.03f;
	if (key == 'G') variable_y += 0.03f;
	if (key == 'H') variable_y = 0.00f;
	if (key == 'V') variable_z -= 0.03f;
	if (key == 'N') variable_z = 0.00f;
	if (key == 'B') variable_z += 0.03f;

	if (key == 'M' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
		std::cout << "colourmode=" << colourmode << std::endl;
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == 'K' && action != GLFW_PRESS)
	{
		drawmode++;
		if (drawmode > 2) drawmode = 0;
	}

}



/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper *glw = new GLWrapper(1024, 768, "Dragomir Kolev");;

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);
	glw->setDoMovement(do_movement);
	init(glw);
	glw->eventLoop();

	//while (!glfwWindowShouldClose(glw->getWindow())){
	//	glfwPollEvents();
		//do_movement();

	//}
	

	delete(glw);
	return 0;
}

/* Make a sphere from two triangle fans (one at each pole) and triangle strips along latitudes */
/* Make a sphere from two triangle fans (one at each pole) and triangle strips along latitudes */
/* This version uses indexed vertex buffers for both the fans at the poles and the latitude strips */
/* Please not that a better structure would be to make a seperate sphere class and create an instance
of this in our lab3start.cpp program. */
GLuint makeSphereVBO(GLuint numlats, GLuint numlongs)
{
	GLuint i, j;
	/* Calculate the number of vertices required in sphere */
	GLuint numvertices = 2 + ((numlats - 1) * numlongs);
	GLfloat* pVertices = new GLfloat[numvertices * 3];
	GLfloat* pColours = new GLfloat[numvertices * 4];
	makeUnitSphere(pVertices, numlats, numlongs);

	/* Define colours as the x,y,z components of the sphere vertices */
	for (i = 0; i < numvertices; i++)
	{
		pColours[i * 4] = pVertices[i * 3];
		pColours[i * 4 + 1] = pVertices[i * 3 + 1];
		pColours[i * 4 + 2] = pVertices[i * 3 + 2];
		pColours[i * 4 + 3] = 1.f;
	}

	/* Generate the vertex buffer object */
	glGenBuffers(1, &sphereBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 3, pVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the normals in a buffer object */
	glGenBuffers(1, &sphereNormals);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 3, pVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the colours in a buffer object */
	glGenBuffers(1, &sphereColours);
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 4, pColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Calculate the number of indices in our index array and allocate memory for it */
	GLuint numindices = ((numlongs * 2) + 2) * (numlats - 1) + ((numlongs + 2) * 2);
	GLuint* pindices = new GLuint[numindices];

	// fill "indices" to define triangle strips
	GLuint index = 0;		// Current index

	// Define indices for the first triangle fan for one pole
	for (i = 0; i < numlongs + 1; i++)
	{
		pindices[index++] = i;
	}
	pindices[index++] = 1;	// Join last triangle in the triangle fan

	GLuint start = 1;		// Start index for each latitude row
	for (j = 0; j < numlats - 2; j++)
	{
		for (i = 0; i < numlongs; i++)
		{
			pindices[index++] = start + i;
			pindices[index++] = start + i + numlongs;
		}
		pindices[index++] = start; // close the triangle strip loop by going back to the first vertex in the loop
		pindices[index++] = start + numlongs; // close the triangle strip loop by going back to the first vertex in the loop

		start += numlongs;
	}

	// Define indices for the last triangle fan for the south pole region
	for (i = numvertices - 1; i > (numvertices - numlongs - 2); i--)
	{
		pindices[index++] = i;
	}
	pindices[index] = numvertices - 2;	// Tie up last triangle in fan

	// Generate a buffer for the indices
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numindices * sizeof(GLuint), pindices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete pindices;
	delete pColours;
	delete pVertices;
	return numvertices;
}


/* Define the vertex positions for a sphere. The array of vertices must have previosuly
been created.
*/
void makeUnitSphere(GLfloat *pVertices, GLuint numlats, GLuint numlongs)
{
	GLfloat DEG_TO_RADIANS = 3.141592f / 180.f;
	GLuint vnum = 0;
	GLfloat x, y, z, lat_radians, lon_radians;

	/* Define north pole */
	pVertices[0] = 0; pVertices[1] = 0; pVertices[2] = 1.f;
	vnum++;

	GLfloat latstep = 180.f / numlats;
	GLfloat longstep = 360.f / numlongs;

	/* Define vertices along latitude lines */
	for (GLfloat lat = 90.f - latstep; lat > -90.f; lat -= latstep)
	{
		lat_radians = lat * DEG_TO_RADIANS;
		for (GLfloat lon = -180.f; lon < 180.f; lon += longstep)
		{
			lon_radians = lon * DEG_TO_RADIANS;

			x = cos(lat_radians) * cos(lon_radians);
			y = cos(lat_radians) * sin(lon_radians);
			z = sin(lat_radians);

			/* Define the vertex */
			pVertices[vnum * 3] = x; pVertices[vnum * 3 + 1] = y; pVertices[vnum * 3 + 2] = z;
			vnum++;
		}
	}
	/* Define south pole */
	pVertices[vnum * 3] = 0; pVertices[vnum * 3 + 1] = 0; pVertices[vnum * 3 + 2] = -1.f;
}


/* Draws the sphere form the previously defined vertex and index buffers */
void drawSphere()
{
	GLuint i;

	/* Draw the vertices as GL_POINTS */
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	/* Bind the sphere normals */
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	/* Bind the sphere colours */
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glPointSize(3.f);

	// Enable this line to show model in wireframe
	if (drawmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (drawmode == 2)
	{
		glDrawArrays(GL_POINTS, 0, numspherevertices);
	}
	else
	{
		/* Bind the indexed vertex buffer */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		/* Draw the north pole regions as a triangle  */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 2, GL_UNSIGNED_INT, (GLvoid*)(0));

		/* Calculate offsets into the indexed array. Note that we multiply offsets by 4
		because it is a memory offset the indices are type GLuint which is 4-bytes */
		GLuint lat_offset_jump = (numlongs * 2) + 2;
		GLuint lat_offset_start = numlongs + 2;
		GLuint lat_offset_current = lat_offset_start * 4;

		/* Draw the triangle strips of latitudes */
		for (i = 0; i < numlats - 2; i++)
		{
			glDrawElements(GL_TRIANGLE_STRIP, numlongs * 2 + 2, GL_UNSIGNED_INT, (GLvoid*)(lat_offset_current));
			lat_offset_current += (lat_offset_jump * 4);
		}
		/* Draw the south pole as a triangle fan */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 2, GL_UNSIGNED_INT, (GLvoid*)(lat_offset_current));
	}

	
}

