#pragma once

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera() {}
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);

	static glm::mat4 viewMatrix;
	static float pitch, yaw;
	static bool enabled;

	void SetAll(glm::vec3 &pos, glm::vec3 &front, glm::vec3 &up);
	void SetPosition(glm::vec3 &pos);
	void SetFront(glm::vec3& front);
	void SetSpeed(float speed);

	float GetSpeed();
	glm::vec3 GetPosition() { return position; }
	glm::vec3* GetPositionPointer() { return &position; }
	glm::vec3 GetFront() { return front; }
	glm::vec3 GetUp() { return up; }

	void Update();

private:
	float speed = 2.0f;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 front = glm::vec3(0.0f);
	glm::vec3 up = glm::vec3(0.0f);
};