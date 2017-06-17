#pragma once		
#include <include/glm.h>
#include <include/math.h>
class GameCamera
{
	public:
		GameCamera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
		{
			Set(position, center, up);
		}
		GameCamera()
		{
			position = glm::vec3(0, 2, 5);
			forward = glm::vec3(0, 0, -1);
			up		= glm::vec3(0, 1, 0);
			right	= glm::vec3(1, 0, 0);
			distanceToTarget = 3;
		}
		~GameCamera() {}

		void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
		{
			this->position = position;
			forward = normalize(center-position);
			right	= glm::cross(forward, up);
			this->up = glm::cross(right,forward);
		}

		void MoveForward(float distance)
		{
			glm::vec3 dir = glm::normalize(glm:: vec3(forward.x, 0, forward.z));
			position = position + dir * distance;				
		}

		void TranslateForward(float distance)
		{
			position = position + forward * distance;
		}

		void TranslateToTarget(float distance)
		{
			TranslateForward(distance);
		}

		void TranslateUpword(float distance)
		{
			position = position + up * distance;
		}

		void TranslateRight(float distance)
		{
			right = glm::cross(forward, up);
			position = position + right * distance;
		}

		void RotateFirstPerson_OX(float angle)
		{
			forward = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1.0f));
			forward = glm::normalize(forward);

			right = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1.0f));
			right = glm::normalize(right);

			up = glm::cross(right, forward);
			up = glm::normalize(up);
		}

		void RotateFirstPerson_OY(float angle)
		{
			forward = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1, 0, 0)) * glm::vec4(forward, 1.0f));
			forward = glm::normalize(forward);

			up = glm::cross(right, forward);
			up = glm::normalize(up);
		}

		void RotateFirstPerson_OZ(float angle)
		{
			right = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(right, 1.0f));
			right = glm::normalize(right);

			up = glm::cross(right, forward);
			up = glm::normalize(up);
		}

		void RotateThirdPerson_OX(float angle)
		{
			TranslateToTarget(distanceToTarget);
			RotateFirstPerson_OX(angle);
			TranslateToTarget(-distanceToTarget);
		}

		void RotateThirdPerson_OY(float angle)
		{
			TranslateToTarget(distanceToTarget);
			RotateFirstPerson_OY(angle);
			TranslateToTarget(-distanceToTarget);
		}

		void RotateThirdPerson_OZ(float angle)
		{
			TranslateToTarget(distanceToTarget);
			RotateFirstPerson_OZ(angle);
			TranslateToTarget(-distanceToTarget);
		}

		glm::mat4 GetViewMatrix()
		{
			// Returns the View Matrix
			return glm::lookAt(position, position + forward, up);
		}

		glm::vec3 GetTargetPosition()
		{
			return position + forward * distanceToTarget;
		}

	public:
		float distanceToTarget;
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		float fovx;
		float fovy;
};