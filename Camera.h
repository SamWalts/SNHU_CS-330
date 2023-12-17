// Contains all camera functions and movement functions

#pragma once
#ifndef CAMERA_H
#define CAMERA_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.5f;
const float SENSITIVITY = 0.001f;
const float ZOOM = 45.0f;

enum Keyboard_Movement {
	FORWARD,		// W
	BACK,			// S
	RIGHT,			// D
	LEFT,			// A
	DOWN,			// Q
	UP,				// E
	RESET			// F
};


class Camera {
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 1.0f, 1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		Front = glm::vec3(0.f, 0.f, 0.f);
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVariables();
	}

	glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void MouseMovementProcess(float lastX, float lastY) {
		// Must have 2 true in Source.cpp
		// isOrbiting firstMouseMove

		Yaw += lastX;
		Pitch += lastY;
		//updateCameraVariables();
		updateCameraVariables();
	}

	void MouseScroll(double yoffset) {
		//Zoom -= (float)yoffset;
		//if (Zoom < 1.0f)
		//	Zoom = 1.0f;
		//if (Zoom > 45.0f)
		//	Zoom = 45.0f;

		MovementSpeed -= static_cast<float>(yoffset);
		if (MovementSpeed < 0.5f) {
			MovementSpeed = 0.5f;
		}
		if (MovementSpeed > 10.0f) {
			MovementSpeed = 10.0f;
		}
	}

		void KeyboardProcess(Keyboard_Movement direction, float deltaTime) {
			float movementVelocity = MovementSpeed * deltaTime;

			switch (direction) {
			case FORWARD:
				Position += Front * movementVelocity;
				break;
			case BACK:
				Position -= Front * movementVelocity;
				break;
			case LEFT:
				Position -= Right * movementVelocity;
				break;
			case RIGHT:
				Position += Right * movementVelocity;
				break;
			case UP:
				Position += Up * movementVelocity;
				break;
			case DOWN:
				Position -= Up * movementVelocity;
				break;
			case RESET:
				ResetToDefaults();
				break;
			default:
				// Handle the case where direction is not one of the specified values
				break;
			}
		}



private:
	void ResetToDefaults() {
		Position = glm::vec3(0.0f, 5.0f, 15.0f);
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		Yaw = -90.0f;
		Pitch = 0.0f;
		MovementSpeed = 0.5f;
		Zoom = 45.0f;

		updateCameraVariables();  // Ensure to update other dependent variables
	}

	// calculates the front vector from the Camera's updating angles
	void updateCameraVariables()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors
		Up = glm::normalize(glm::cross(Right, Front));
	}
	};


#endif // !
