#ifndef FREE_CAMERA_H
#define FREE_CAMERA_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <vector>

#include <input.h>
#include <timer.h>
#include <config.h>
#include <gamehelper.h>

namespace Camera
{
	class FirstPerson
	{

	public:
		FirstPerson() { _position = glm::vec3(0.0f, 0.0f, 0.0f); };
		FirstPerson(glm::vec3 position);
		glm::mat4 getViewMatrix();
		float getZoom();
		void update(Input &input, Input &prevInput, Timer &timer);


	private:
		glm::vec3 _position;
		glm::vec3 _front;
		glm::vec3 _up;
		glm::vec3 _right;
		glm::vec3 _worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		bool viewUpdated = true;

		float _yaw = 200.0f;
		float _pitch = -20.0f;

		float _speed = 0.01f;
		float _sensitivity = 0.05f;
		float _zoom = 45.0f;


		void calculateVectors();
	};

	const unsigned int CAM2D_FLOAT = 100;
	class RoomFollow2D
	{
	public:
		void SetCameraOffset(glm::vec2 pos)
		{
			cameraArea = glm::vec4(pos.x - settings::TARGET_WIDTH/2.0f, pos.y - settings::TARGET_HEIGHT/2.0f,
			 settings::TARGET_WIDTH, settings::TARGET_HEIGHT);
			offset = glm::translate(glm::mat4(1.0f),
				 glm::vec3(-pos.x + settings::TARGET_WIDTH/2.0f, -pos.y + settings::TARGET_HEIGHT/2.0f, 0));
			previousOff.x = getOffset(settings::TARGET_WIDTH, pos.x, currentRect.x, currentRect.z);
			previousOff.y = getOffset(settings::TARGET_HEIGHT, pos.y, currentRect.y, currentRect.w);
		}

		void Target(glm::vec2 focus, Timer &timer);

		void setScale(float val) { scale = val; }
		float getScale() { return scale; }

		glm::mat4 getViewMat()
		{
			return offset;
		}
		void setCameraRects(std::vector<glm::vec4> cameraRects)
		{
			this->cameraRects = cameraRects;
		}
		void setCameraMapRect(glm::vec4 mapRect)
		{
			this->mapRect = mapRect;
		}
		void clearCameraRects()
		{
			cameraRects.clear();
			mapRect = glm::vec4(0);
		}

		glm::vec4 getCameraArea()
		{
			return cameraArea;
		}

		glm::vec2 getCameraOffset()
		{
			return glm::vec2(cameraArea.x, cameraArea.y);
		}

		glm::vec4 currentRoom = glm::vec4(0);
	private:

		float getOffset(float target, float focus, float roomPos, float roomSize);

		glm::mat4 offset = glm::mat4(1.0f);
		std::vector<glm::vec4> cameraRects;
		glm::vec4 mapRect =  glm::vec4(0);

		glm::vec4 currentRect = glm::vec4(0, 0, 0, 0);
		glm::vec2 previousOff = glm::vec2(0);

		glm::vec4 cameraArea = glm::vec4(0);
		float scale = 1.0f;
	};

}	//namesapce end



#endif
