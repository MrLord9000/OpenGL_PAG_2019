#include "Camera.h"

glm::mat4 Camera::viewMatrix = glm::mat4(1.0f);

// ===== Setters ====================================================

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
	this->position = pos;
	this->front = front;
	this->up = up;
}

void Camera::SetAll(glm::vec3 &pos, glm::vec3 &front, glm::vec3 &up)
{
	this->position = pos;
	this->front = front;
	this->up = up;
}

void Camera::SetPosition(glm::vec3 &pos)
{
	this->position = pos;
}

void Camera::SetSpeed(float speed)
{
	this->speed = speed;
}

// ===== Getters ====================================================

float Camera::GetSpeed()
{
	return speed;
}

// ===== Update ====================================================

void Camera::Update()
{
	viewMatrix = glm::lookAt(position, position + front, up);
}
