// Samuel Walters
// 12-6-2023
// CS-330 Wabara

// Movement is with WASD
// Q is for down
// E is for UP
// Key F will reset the camera position
// Mouse Scrolling is for slowing down the movement speed
// 


#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Camera.h"


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL2/SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;
bool wireFrame = false;
bool ortho = false;

// Light source Position
glm::vec3 lightPosition(1.0f, 10.f, 10.f);
glm::vec3 lightPosition1(-1.0f, 15.0f, 10.f);


GLuint shaderProgram; // for compiled shaders
// Declare Input Callback Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);

// Declare View Matrix
glm::mat4 viewMatrix;

// Camera Field of View
GLfloat fov = 45.0f;

Camera myCamera(glm::vec3(-5.0f, 5.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.f, -23.f);

glm::float32 triRotations[] = { 0.0f, 60.0f, 120.0f, 180.0f, 240.0f, 300.0f };

// Camera Transformation Prototype
void TransformCamera();

// Boolean array for keys and mouse buttons
bool keys[1024], mouseButtons[3];

// Pitch and Yaw
GLfloat radius = 15.0f, rawYaw = 0.0f, rawPitch = 0.0f, degYaw, degPitch;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = width, lastY = height, xChange, yChange; // Center mouse cursor
bool firstMouseMove = true;


// Define vertex data for Triangle (GLfloat array)
GLfloat triangleVertices[] = {
	// positon attributes (x, y, z)
	0.0f, 0.0f, 0.0f, // vert 1
	1.0f, 0.66f, 0.0f,
	0.5f, 1.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	0.5f, 0.866f, 0.0f, // vert 2
	0.0f, 1.0f, 0.66f,
	0.0f, 0.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	1.0f, 0.0f, 0.0f, // vert 3
	0.92f, 0.0f, 1.0f,
	1.0f, 0.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	0.5f, 0.866f, 0.0f, // vert 4
	0.1f, 1.0f, 0.87f,
	0.5f, 1.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	0.5f, 0.866f, -2.0f, // vert 5
	0.25f, 1.0f, 0.6f,
	0.0f, 0.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	1.0f, 0.0f, 0.0f, // vert 6
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	1.0f, 0.0f, 0.0f, // vert 7
	0.20f, 0.0f, 1.0f,
	0.5f, 1.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	1.0f, 0.0f, -2.0f, // vert 8
	0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z

	0.5f, 0.866f, -2.0f, // vert 9
	0.33f, 1.0f, 0.0f,
	1.0f, 0.0f, // texture coordinates
	0.0f, 0.0f, 1.0f, // Normal positive Z
};


GLfloat lampVertices[] = {
	-0.5, -0.5, 0.0, // index 0
	-0.5, 0.5, 0.0, // index 1
	0.5, -0.5, 0.0,  // index 2	
	0.5, 0.5, 0.0,  // index 3	
};


GLfloat counterVertices[] = {

	// Triangle 1
	-0.5, -0.5, 0.0, // index 0
	1.0, 0.0, 0.0, // red
	0.0, 0.0,		// UV (bl)
	0.0f, 0.0f, 1.0f, // normal positive z

	-0.5, 0.5, 0.0, // index 1
	0.0, 1.0, 0.0, // green
	0.0, 1.0,		// UV (br)
	0.0f, 0.0f, 1.0f, // normal positive z

	0.5, -0.5, 0.0,  // index 2	
	0.0, 0.0, 1.0, // blue
	1.0, 0.0,		// UV (tl)
	0.0f, 0.0f, 1.0f, // normal positive z

	// Triangle 2	
	0.5, 0.5, 0.0,  // index 3	
	1.0, 0.0, 1.0, // purple
	1.0, 1.0,		// UV (tr)
	0.0f, 0.0f, 1.0f // normal positive z
};

// Define element indices
GLubyte counterIndices[] = {
	0, 1, 2,
	1, 2, 3
};

// for Pyramid AKA candy
// Change this to have the  rectangle in middle of triangles
GLfloat vertices[] = {
	// positon attributes (x,y,z)
	-2.0f, 0.0f, 0.0f, // vert 0 A
	0.0f, 1.0f, 0.0f, // red
	0.0f, 1.0f,			// texture coordinates
	0.0f, 0.0f, -1.0f, // Normal positive Z

	-2.0f, 0.0f, -1.0f, // vert 1 B
	1.0f, 0.0f, 0.0f, // green
	0.0f, 0.0f,			// texture coordinates
	0.0f, 0.0f, -1.0f, // Normal positive Z

	2.0f, 0.0f, 0.0f,  // vert 2 C
	0.0f, 1.0f, 1.0f, // blue
	1.0f, 1.0f,			// texture coordinates
	0.0f, 0.0f, -1.0f, // Normal positive Z

	2.0f, 0.0f, -1.0f, // vert 3 D
	1.0f, 1.0f, 0.0f, // green
	1.0f, 0.0f,			// texture coordinates
	0.0f, 0.0f, -1.0f, // Normal positive Z

	0.0f, 2.0f, 0.0f, // vert 4 F
	0.0f, 0.0f, 1.0f, // green 
	0.5f, 0.5f,			// texture coordinates
	0.0f, 0.0f, -1.0f, // Normal positive Z

	0.0f, 2.0f, -1.0f, // vert 5 G
	0.0f, 0.0f, 1.0f, // green 
	0.5f, 0.5f,			// texture coordinates
	0.0f, 0.0f, -1.0f, // Normal positive Z
};

//Plane Transforms
glm::vec3 lampPlanePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.5f),
	glm::vec3(0.5f,  0.0f,  0.0f),
	glm::vec3(0.0f,  0.0f,  -0.5f),
	glm::vec3(-0.5f, 0.0f,  0.0f),
	glm::vec3(0.0f, 0.5f,  0.0f),
	glm::vec3(0.0f, -0.5f,  0.0f)
};

glm::float32 lampPlaneRotations[] = {
	0.0f, 90.0f, 180.0f, -90.0f, -90.f, 90.f
};


// for Pyramid AKA candy
GLubyte indices[] = {
	0, 1, 2,    // BASE
	2, 3, 1,    // BASE
	0, 1, 4,    // Left square
	4, 5, 1,	// Left square
	1, 5, 3,	// Back face
	2, 4, 3,    // C, E, D
	4, 3, 5,    // A, D, E
	2, 4, 0,    // Front face

};


// Draw Primitive(s)
void draw()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei count = sizeof(indices) / sizeof(indices[0]); // Calculate the number of indices
	glDrawElements(mode, count, GL_UNSIGNED_BYTE, nullptr);
}


// Draw Primitive(s)
void drawPlane()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 6;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}


// Create and Compile Shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	// Create Shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	// Attach source code to Shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	// Compile Shader
	glCompileShader(shaderID);

	// Return ID of Compiled shader
	return shaderID;

}


// Create Program Object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	// Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	// Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create program object
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	// Link shaders to create executable
	glLinkProgram(shaderProgram);

	// Delete compiled vertex and fragment shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	// Return Shader Program
	return shaderProgram;

}

// ERROR CHECKING
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "OpenGL Debug Message: %s\n", message);
}


int main(void)
{
	width = 1920; height = 1080;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "CS-330 Final Project", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set input callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;

	// Enable Debugging
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint pyramidVBO, pyramidVAO, pyramidEBO;
	GLuint counterVBO, counterEBO, counterVAO;
	GLuint lampVBO, lampEBO, lampVAO;
	GLuint triangleVBO, triangleVAO; // triangle VAO variables
	GLuint chargerVBO, chargerEBO, chargerVAO;

	// Generate all buffers
	glGenBuffers(1, &pyramidVBO); // Create VBO
	glGenBuffers(1, &pyramidEBO); // Create EBO

	// Countertop
	glGenBuffers(1, &counterVBO); // Create VBO
	glGenBuffers(1, &counterEBO); // Create EBO

	// For Cylinder
	glGenBuffers(1, &triangleVBO); // Create VBO
	glGenVertexArrays(1, &triangleVAO); // Create VAO

	// Lamp
	glGenBuffers(1, &lampVBO); // Create VBO
	glGenBuffers(1, &lampEBO); // Create EBO

	// Charger
	glGenBuffers(1, &chargerVBO); // Create VBO
	glGenBuffers(1, &chargerEBO); // Create EBO

	glGenVertexArrays(1, &pyramidVAO); // Candy/Pyramid VAO
	glGenVertexArrays(1, &counterVAO); // Create Countertop VOA
	glGenVertexArrays(1, &lampVAO); // Create Lamp VOA
	glGenVertexArrays(1, &chargerVAO); // create lamp VAO

	// Pyramid STUFF
	glBindVertexArray(pyramidVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO); // Select EBO

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

		// Specify attribute location and layout to GPU
		// Location
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Texture
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// Light Normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)

	// Floor VAO
	glBindVertexArray(counterVAO);

		glBindBuffer(GL_ARRAY_BUFFER, counterVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, counterEBO); // Select EBO

		glBufferData(GL_ARRAY_BUFFER, sizeof(counterVertices), counterVertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(counterIndices), counterIndices, GL_STATIC_DRAW); // Load indices 

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

	glBindVertexArray(0);


	// Triangle for Cylinder
	glBindVertexArray(triangleVAO); // Activate VAO for VBO association
		
		glBindBuffer(GL_ARRAY_BUFFER, triangleVBO); // Enable VBO	
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); // Copy Vertex data to VBO

		// Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0); // Associate VBO with VA (Vertex Attribute)
		glEnableVertexAttribArray(0); // Enable VA
	
		// Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Associate VBO with VA
		glEnableVertexAttribArray(1); // Enable VA
	
		// Texture
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// Normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
	glBindVertexArray(0);


	glBindVertexArray(lampVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lampVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO); // Select EBO

		glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(counterIndices), counterIndices, GL_STATIC_DRAW); // Load indices 

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glBindVertexArray(chargerVAO);

		// VBO and EBO Placed in User-Defined VAO
		glBindBuffer(GL_ARRAY_BUFFER, chargerVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chargerEBO); // Select EBO


		glBufferData(GL_ARRAY_BUFFER, sizeof(counterVertices), counterVertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(counterIndices), counterIndices, GL_STATIC_DRAW); // Load indices 

		// Specify attribute location and layout to GPU
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)


	// Load texture maps
	int candyTexWidth, candyTexHeight, counterTexWidth, counterTexHeight, slinkyTexWidth, slinkyTexHeight, chargerTexHeight, chargerTexWidth;
	int monsterTexWidth, monsterTexHeight, monsterEyesTexHeight, monsterEyesTexWidth;

	unsigned char* candyImage = SOIL_load_image("candySmaller.png", &candyTexWidth, &candyTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* counterImage = SOIL_load_image("counter.png", &counterTexWidth, &counterTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* slinkyImage = SOIL_load_image("slinky.png", &slinkyTexWidth, &slinkyTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* chargerImage = SOIL_load_image("charger.png", &chargerTexWidth, &chargerTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* monsterImage = SOIL_load_image("monster.png", &monsterTexWidth, &monsterTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* monsterEyesImage = SOIL_load_image("monsterEye.png", &monsterEyesTexWidth, &monsterEyesTexHeight, 0, SOIL_LOAD_RGB);

	// Generate Textures  
	GLuint candyTexture; // for texture ID
		glGenTextures(1, &candyTexture);// Generate texture id
		glBindTexture(GL_TEXTURE_2D, candyTexture); // Activate texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, candyTexWidth, candyTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, candyImage); // Generate texture
		glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
		SOIL_free_image_data(candyImage); // Free image from memory
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object

	GLuint counterTexture;
		glGenTextures(1, &counterTexture);
		glBindTexture(GL_TEXTURE_2D, counterTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, counterTexWidth, counterTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, counterImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(counterImage);
		glBindTexture(GL_TEXTURE_2D, 0);

	GLuint slinkyTexture;
		glGenTextures(1, &slinkyTexture);
		glBindTexture(GL_TEXTURE_2D, slinkyTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, slinkyTexWidth, slinkyTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, slinkyImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(slinkyImage);
		glBindTexture(GL_TEXTURE_2D, 0);

	GLuint chargerTexture;
		glGenTextures(1, &chargerTexture);
		glBindTexture(GL_TEXTURE_2D, chargerTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, chargerTexWidth, chargerTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, chargerImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(chargerImage);
		glBindTexture(GL_TEXTURE_2D, 0);

	GLuint monsterTexture;
		glGenTextures(1, &monsterTexture);
		glBindTexture(GL_TEXTURE_2D, monsterTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, monsterTexWidth, monsterTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, monsterImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(monsterImage);
		glBindTexture(GL_TEXTURE_2D, 0);

	GLuint monsterEyesTexture;
		glGenTextures(1, &monsterEyesTexture);
		glBindTexture(GL_TEXTURE_2D, monsterEyesTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, monsterEyesTexWidth, monsterEyesTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, monsterEyesImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(monsterEyesImage);
		glBindTexture(GL_TEXTURE_2D, 0);

	// Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 lightColor1;"
		"uniform vec3 lightPos1;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"//Ambient\n"
		"float ambientStrength = 0.8f;"
		"vec3 ambient = ambientStrength * lightColor * lightColor1;"
		"//Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"vec3 lightDir1 = normalize(lightPos1 - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"float diff1 = max(dot(norm, lightDir1), 0.0);"
		"vec3 diffuse = diff * lightColor * 0.2;"
		"vec3 diffuse1 = diff1 * lightColor1 * 10.0;"
		"vec3 fullDiffuse = diffuse1 + diffuse;"
		"//Specularity\n"
		"float specularStrength = 0.5f;"
		"float specularStrength1 = 0.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"vec3 reflectDir1 = reflect(-lightDir1, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
		"float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 specular1 = specularStrength1 * spec1 * lightColor1;"
		"vec3 fullSpecular = specular + specular1;"
		"vec3 result = (fullDiffuse + ambient + fullSpecular) * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	// lamp vertex shader source
	string lampVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// Fragment shader source code
	string lampFragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(1.0f);"
		"}\n";

	// Creating Shader Program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	GLuint lampShaderProgram = CreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Toggle Wireframe mode
		if (wireFrame) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use Shader Program exe and select VAO before drawing 
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes

		// Declare transformations (can be initialized outside loop)		
		glm::mat4 projectionMatrix;

		// Define LookAt Matrix
		viewMatrix = myCamera.GetViewMatrix();

		// Define projection matrix
		projectionMatrix = glm::perspective(myCamera.Zoom, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		
		
		if (ortho) {
			GLfloat oWidth = (GLfloat)width * 0.01f; // 10% of width
			GLfloat oHeight = (GLfloat)height * 0.01f; // 10% of height

			viewMatrix = myCamera.GetViewMatrix();
			projectionMatrix = glm::ortho(-oWidth, oWidth, oHeight, -oHeight, 0.1f, 100.0f);
		}
		else {
			viewMatrix = myCamera.GetViewMatrix();
			projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		}
		// Get matrix's uniform location and set matrix
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		// Get light and object Color, and light position location
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint lightColorLoc1 = glGetUniformLocation(shaderProgram, "lightColor1");
		GLint lightPosLoc1 = glGetUniformLocation(shaderProgram, "lightPos1");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

		// Assign light colors and obj colors
		glUniform3f(objectColorLoc, 0.56f, 0.52f, 0.47f); // Color is 

		// Light colors here 
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightColorLoc1, 1.0f, 1.0f, 1.0f);

		// Set light position
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(lightPosLoc1, lightPosition1.x, lightPosition1.y, lightPosition1.z);

		// Specify camera position for shader
		glUniform3f(viewPosLoc, myCamera.Position.x, myCamera.Position.y, myCamera.Position.z);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glm::mat4 modelMatrix;
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes

		/*
		Loop to draw our all pyramid shaped objects
		*/
		for (int i = 0; i <= 9; i++) {
			// Draw Candy first
			if (i == 0) {
				//PYRAMID
				
				// Bind tex first then to array
				glBindTexture(GL_TEXTURE_2D, candyTexture); // Apply grid texture
				glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				glBindVertexArray(pyramidVAO);
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.f, 0.f));
				modelMatrix = glm::rotate(modelMatrix, 0.f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glEnable(GL_DEPTH_TEST);
				draw(); // DRAW CANDY
			}
			// Monster Eyes
			if (i == 1 || i == 2) {
				
				//glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				// Bind tex first then to array
				glBindTexture(GL_TEXTURE_2D, monsterEyesTexture); // Apply grid texture
				glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				glBindVertexArray(pyramidVAO);
				glm::mat4 modelMatrix;

				if (i == 1) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(1.8f, 2.f, 3.f));
				}
				else {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(2.2f, 2.f, 3.f));
				}
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.05f, .05f, .05f));
				modelMatrix = glm::rotate(modelMatrix, 0.f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glEnable(GL_DEPTH_TEST);
				draw(); 
			}
			// Monster Mouth
			if (i == 3) {

				//glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				// Bind tex first then to array
				glBindTexture(GL_TEXTURE_2D, slinkyTexture); // Apply grid texture
				glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				glBindVertexArray(pyramidVAO);
				glm::mat4 modelMatrix;

				modelMatrix = glm::translate(modelMatrix, glm::vec3(2.f, 1.5f, 3.f));

				modelMatrix = glm::scale(modelMatrix, glm::vec3(.3f, .1f, .3f));
				modelMatrix = glm::rotate(modelMatrix, 180.f * toRadians, glm::vec3(0.0f, 0.0f, 3.0f));
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glEnable(GL_DEPTH_TEST);
				draw(); 
			}
			// Monster Teeth
			if (i == 4 || i == 5) {

				//glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				// Bind tex first then to array
				glBindTexture(GL_TEXTURE_2D, chargerTexture); // Apply grid texture
				glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				glBindVertexArray(pyramidVAO);
				glm::mat4 modelMatrix;
				if (i == 4) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(1.8f, 1.5f, 3.1f));
				}
				else {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(2.2f, 1.5f, 3.1f));
				}
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.05f, .1f, .05f));
				modelMatrix = glm::rotate(modelMatrix, 180.f * toRadians, glm::vec3(0.0f, 0.0f, 3.0f));
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glEnable(GL_DEPTH_TEST);
				draw(); // DRAW CANDY
			}
			// Monster Arms
			if (i == 6 || i == 7) {

				//glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				// Bind tex first then to array
				glBindTexture(GL_TEXTURE_2D, monsterTexture); // Apply grid texture
				glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				glBindVertexArray(pyramidVAO);
				glm::mat4 modelMatrix;
				if (i == 4) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(1.25f, 1.5f, 2.55f));
				}
				else {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(2.75f, 1.5f, 2.55f));
				}
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.15f, .15f, .15f));
				modelMatrix = glm::rotate(modelMatrix, 180.f * toRadians, glm::vec3(0.0f, 0.0f, 2.0f));
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glEnable(GL_DEPTH_TEST);
				draw(); // DRAW CANDY
			}
			// Monster Ears
			if (i == 8 || i == 9) {

				//glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				// Bind tex first then to array
				glBindTexture(GL_TEXTURE_2D, monsterTexture); // Apply grid texture
				glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes
				glBindVertexArray(pyramidVAO);
				glm::mat4 modelMatrix;
				if (i == 8) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(1.35f, 2.5f, 2.55f));
				}
				else {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(2.35f, 2.5f, 2.55f));
				}
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.15f, .15f, .15f));
				modelMatrix = glm::rotate(modelMatrix, 270.f * toRadians, glm::vec3(0.0f, 0.0f, 2.0f));
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1.f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				glEnable(GL_DEPTH_TEST);
				draw(); // DRAW CANDY
			}
			glBindVertexArray(0); //Incase different VAO will be used after
			//glUseProgram(0); // Incase different shader will be used after
		}
		// Error checking after pyramids loops
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (after drawing Pyramids): " << error << std::endl;
		}

		// START COUNTERTOP
		//glBindTexture(GL_TEXTURE_2D, counterTexture); // Apply grid texture
		glBindVertexArray(counterVAO);
			//glUseProgram(shaderProgram);
			glBindTexture(GL_TEXTURE_2D, counterTexture);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.f, 0.f));
			modelMatrix = glm::rotate(modelMatrix, 90.f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(20.f, 20.f, 20.f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glEnable(GL_DEPTH_TEST);
			drawPlane();		// DRAW COUNTER
		glBindVertexArray(0); //Incase different VAO will be used after
		//glUseProgram(0);

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (after drawing counter): " << error << std::endl;
		}

		// Create Cylinders for Slinky, glasses case, and Monster
		for (int j = 0; j <= 3; j++) {

			glBindVertexArray(triangleVAO);

			glm::float32 triRotations[] = { 0.0f, 60.0f, 120.0f, 180.0f, 240.0f, 300.0f };
			// Loop to draw the cylinders
			for (int i = 0; i < 6; i++) {
				// Apply Transform to model
				glm::mat4 modelMatrix = glm::mat4(1.0f);
				// This is the first cylinder (Slinky)
				if (j == 0) {
					glBindTexture(GL_TEXTURE_2D, slinkyTexture);
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, .75f, 0.75f));
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-8.f, 0.f, 3.f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				// Second Cylinder, top of Monster
				if (j == 1) {
					glBindTexture(GL_TEXTURE_2D, monsterTexture);
					modelMatrix = glm::scale(modelMatrix, glm::vec3(.8f, .8f, .8f));
					modelMatrix = glm::translate(modelMatrix, glm::vec3(2.5f, 1.f, 2.75f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				// Third Cylinder, bottom of Monster
				if (j == 2) {
					glBindTexture(GL_TEXTURE_2D, monsterTexture);
					modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, .5f, 0.5f));
					modelMatrix = glm::translate(modelMatrix, glm::vec3(3.85f, 0.f, 4.35f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.0f, 1.0f));
				}
				if (j == 3) {
					glBindTexture(GL_TEXTURE_2D, monsterEyesTexture);
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.f, 0.f, 3.5f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(180.f), glm::vec3(0.0f, 0.0f, 1.0f));
					modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.0f, 1.0f));
					modelMatrix = glm::scale(modelMatrix, glm::vec3(1.75f, 1.75f, 1.75f));
				}

				// Copy perspective and MV matrices to uniforms
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Use GL_FILL for filled polygons
				glDrawArrays(GL_TRIANGLES, 0, 9);
			}
			glBindVertexArray(0);// Optional unbinding but recommended
		}

		// Cylinder Errors
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (after drawing Cylinders): " << error << std::endl;
		}


		// START CHARGER
		glBindTexture(GL_TEXTURE_2D, chargerTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(chargerVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form cube
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, lampPlanePositions[i] + glm::vec3(0.0f, 0.25f, 4.0f));
			modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			drawPlane();
		}
		glBindVertexArray(chargerVAO); // User-defined VAO must be called before draw. 
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, lampPlanePositions[i] / glm::vec3(1.25, 1.25, 1.25) + glm::vec3(0.0f, 0.20f, 3.2f));
			modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.80f, .80f, .80f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			drawPlane();
		}
		glBindVertexArray(0); //Incase different VAO wii be used after
		glUseProgram(0);		// Unbind Shader exe and VOA after drawing per frame
		// Slinky Errors
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (after drawing Charger): " << error << std::endl;
		}
		// Unbind Shader exe and VOA after drawing per frame


		/*************************
		ONLY LIGHT SOURCE BELOW HERE
		*************************/


		glUseProgram(lampShaderProgram);

		// Get matrix's uniform location and set matrix
		GLint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
		GLint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
		GLint lampProjLoc = glGetUniformLocation(lampShaderProgram, "projection");

		glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


		glBindVertexArray(lampVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form cube for light source.
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, lampPlanePositions[i] / glm::vec3(8., 8., 8.) + lightPosition);
			modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			drawPlane();
		}
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (after Drawing light One): " << error << std::endl;
		}

		//Transform planes to form cube for light source
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(modelMatrix, lampPlanePositions[i] / glm::vec3(8., 8., 8.) + lightPosition1);
			modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, lampPlaneRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			drawPlane();
		}

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error (after drawing Light Two): " << error << std::endl;
		}

		//Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO will be used after
		////Build View matrix
		// Use loop to build Model matrix for triangle
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		// Poll Camera Transformations
		TransformCamera();

	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &pyramidVAO);
	glDeleteBuffers(1, &pyramidVBO);
	glDeleteBuffers(1, &pyramidEBO);
	glDeleteVertexArrays(1, &lampVAO);
	glDeleteBuffers(1, &lampVBO);
	glDeleteBuffers(1, &lampEBO);
	glDeleteVertexArrays(1, &chargerVAO);
	glDeleteBuffers(1, &chargerVBO);
	glDeleteBuffers(1, &chargerEBO);
	glDeleteVertexArrays(1, &triangleVAO);
	glDeleteBuffers(1, &triangleVBO);

	glfwTerminate();
	return 0;
}

// Define input functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_SPACE) {
		wireFrame = !wireFrame;
	}
	// Toggle orthographic projection
	if (key == GLFW_KEY_O) {
		ortho = !ortho;
	}

	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Assign true to Element ASCII if key pressed
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE) // Assign false to Element ASCII if key released
		keys[key] = false;

}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	myCamera.MouseScroll(yoffset);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouseMove)
		// Change this to send to camera MouseMovementProcess
	{

		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}
	//Calculate mouse offset (Easing effect)
	float xChange = xpos - lastX;
	float yChange = lastY - ypos; // Inverted cam

	// Get current mouse (always starts at 0)
	lastX = xpos;
	lastY = ypos;

	myCamera.MouseMovementProcess(xChange, yChange);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	// Assign boolean state to element Button code
	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}


// Define TransformCamera function
void TransformCamera()
{
	if (keys[GLFW_KEY_W]) {
		myCamera.KeyboardProcess(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		myCamera.KeyboardProcess(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		myCamera.KeyboardProcess(BACK, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		myCamera.KeyboardProcess(RIGHT, deltaTime);
	}
	if (keys[GLFW_KEY_Q]) {
		myCamera.KeyboardProcess(DOWN, deltaTime);
	}
	if (keys[GLFW_KEY_E]) {
		myCamera.KeyboardProcess(UP, deltaTime);
	}
	//Focus camera
	if (keys[GLFW_KEY_F]) {
		myCamera.KeyboardProcess(RESET, deltaTime);
	}

}

