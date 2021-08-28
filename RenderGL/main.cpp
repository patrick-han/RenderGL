#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp" // For printing matrix values

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processKeyboardInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Initial mouse position (center of the screen)
float lastX = DEFAULT_WINDOW_WIDTH / 2;
float lastY = DEFAULT_WINDOW_HEIGHT / 2;
bool firstMouse = true;

// Delta time setup
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Camera initial setup
float yaw = -90.0f; // By default, point camera towards -Z, since yaw is measured from  the +X (0 deg). CW -> -degrees
float pitch = 0.0f;
float fov = 45.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Initialize camera at [0, 0, 3] in world coordinates

// Light variables
glm::vec3 lightPos(1.2f, 1.0f, 1.0f);

// Generic global variables
float arrow_key_value = 0.0;

int main()
{
	glfwInit();
	// Specify OpenGL v3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Explicitly use core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window object
	GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "RenderGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// wrt to the window
	glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

	// On resize window, resize framebuffer/viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Enable depth-testing
	glEnable(GL_DEPTH_TEST);

	// Cursor/mouse	stuff, register callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Capture + hide cursor when application in focus
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Cube vertices + normals
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// Set up Vertex Buffer Object
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind VBO to target GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy data from vertices to buffer memory	

	// Lit objects setup
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO); // Create
	glBindVertexArray(cubeVAO); // Bind
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Set attributes (vertex positions)
	glEnableVertexAttribArray(0); // Enable attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Set attributes (normals)
	glEnableVertexAttribArray(1); // Enable attribute

	// Light source vertex attributes setup
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Shader setup
	Shader lightingShader("shaders\\vertexShaderCubes.vs", "shaders\\lighting.fs"); // For objects to be lit (cubes)
	lightingShader.use();
	lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f); // Coral color
	lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f); // Pure white light
	lightingShader.setVec3("lightPos", lightPos); // Light's position, needed for calculating lighting

	Shader lightingSourceShader("shaders\\vertexShaderLights.vs", "shaders\\light_source.fs"); // For light objects
	lightingSourceShader.use();

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Input
		processKeyboardInput(window);

		// Delta time calculation
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Clear screen with a nice color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// View matrix
		camera.update(); // Update internal state of the camera each frame
		glm::mat4 view_mat = camera.getLookAt_mat();
		
		// Projection matrix (Doesn't change every frame, usually. Here we change the FOV with scroll though)
		glm::mat4 projection_mat;
		projection_mat = glm::perspective(glm::radians(camera.getFOV()), (float)DEFAULT_WINDOW_WIDTH / (float)DEFAULT_WINDOW_WIDTH, 0.1f, 100.0f);

		/*
		* Draw lights
		*/
		lightingSourceShader.use();
		// Light source model matrix
		glm::mat4 light_source_model_mat;
		light_source_model_mat = glm::mat4(1.0f);
		light_source_model_mat = glm::rotate(light_source_model_mat, glm::radians((float)glfwGetTime() * 100), glm::vec3(1.0f, 0.0f, 1.0f));
		light_source_model_mat = glm::translate(light_source_model_mat, lightPos);
		light_source_model_mat = glm::scale(light_source_model_mat, glm::vec3(0.2f));
		lightingSourceShader.setMat4("model_mat", light_source_model_mat);
		lightingSourceShader.setMat4("view_mat", view_mat);
		lightingSourceShader.setMat4("projection_mat", projection_mat);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		/*
		* Draw non-light cube objects
		*/
		// Activate Shader
		lightingShader.use();
		glm::vec3 cubePos(0.5f, 0.0f, 1.0f);
		glm::mat4 cube_model_mat;
		cube_model_mat = glm::mat4(1.0f);
		cube_model_mat = glm::translate(cube_model_mat, cubePos);
		lightingShader.setMat4("model_mat", cube_model_mat);
		lightingShader.setMat4("view_mat", view_mat);
		lightingShader.setMat4("projection_mat", projection_mat);
		glm::vec3 newLightPos = light_source_model_mat * glm::vec4(lightPos, 1.0f);
		lightingShader.setVec3("lightPos", glm::vec3(newLightPos.x, newLightPos.y, newLightPos.z));
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36); // Render the cube

		// Check and all events and then swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Cleanup OpenGL stuff
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// Cleanup glfw
	glfwTerminate();
	return 0;
}

// Window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Process keyboard
void processKeyboardInput(GLFWwindow* window)
{
	/*
	* Generic keybindings
	*/
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		arrow_key_value += 1;
		std::cout << arrow_key_value << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		arrow_key_value -= 1;
		std::cout << arrow_key_value << std::endl;
	}

	/*
	* Camera-related keybindings
	*/
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processMovement(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processMovement(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processMovement(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processMovement(RIGHT, deltaTime);
	}
}

// Based on the mouse position, calculate the new pitch and yaw and update the camera's internal state accordingly
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMouse) // Initially set to true, prevent mouse from jumping on entering
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Restrict movement of pitch, since weird stuff happens with the lookat matrix
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	// Update camera pitch and yaw
	camera.updateYawAndPitch(yaw, pitch);
}

// Adjust FOV with scroll wheel by updating the camera's internal state
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
	{
		fov = 1.0f;
	}
	if (fov > 45.0f)
	{
		fov = 45.0f;
	}

	camera.updateFOV(fov);
}