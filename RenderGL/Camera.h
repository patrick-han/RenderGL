#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};


class Camera
{
public:
	Camera(glm::vec3 init_pos) : 
		cameraPos(init_pos), 
		cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), 
		cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		yaw(-90.0f),
		pitch(0.0f),
		fov(45.0f),
		lookAt_mat(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp))
	{
	}

	~Camera()
	{
	}

	// Generate the new lookAt matrix
	void update()
	{
		// Spherical coordinates, basically
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
		lookAt_mat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	void updateYawAndPitch(float yaw_in, float pitch_in)
	{
		yaw = yaw_in;
		pitch = pitch_in;
	}

	void updateFOV(float fov_in)
	{
		fov = fov_in;
	}

	// Update camera position based on keyboard movements
	void processMovement(Camera_Movement movement, float deltaTime)
	{
		const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
		if (movement == FORWARD)
			cameraPos += cameraSpeed * cameraFront;
		if (movement == BACKWARD)
			cameraPos -= cameraSpeed * cameraFront;
		if (movement == LEFT)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; // Create right vector using cross product
		if (movement == RIGHT)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	float getFOV()
	{
		return fov;
	}

	glm::mat4 getLookAt_mat()
	{
		return lookAt_mat;
	}

private:
	glm::vec3 cameraPos;   // Position of the camera
	glm::vec3 cameraFront; // Direction the camera is looking at
	glm::vec3 cameraUp;    // Up vector, mainly needed for lookat matrix calculation

	float yaw; // -90 degrees by default, point camera towards - Z
	float pitch; // 0 by default
	float fov;

	glm::mat4 lookAt_mat;
};
#endif