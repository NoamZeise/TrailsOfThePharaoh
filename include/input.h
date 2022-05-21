#ifndef INPUT_H
#define INPUT_H


#ifdef  GFX_ENV_VULKAN
	#ifndef  GLM_FORCE_DEPTH_ZERO_TO_ONE
		#define GLM_FORCE_DEPTH_ZERO_TO_ONE
	#endif
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef GFX_ENV_VULKAN
	#ifndef GLFW_INCLUDE_VULKAN
		#define GLFW_INCLUDE_VULKAN
	#endif
#endif
#include <GLFW/glfw3.h>

namespace Input
{
struct Raw
{
	Raw()
	{
		X = 0;
		Y = 0;
		offset = 0;
		for (unsigned int i = 0; i < 1024; i++)
		{
			Keys[i] = false;
		}
		for (unsigned int i = 0; i < 8; i++)
		{
			Buttons[i] = false;
		}
	}
	bool Keys[1024];
	bool Buttons[8];
	double X;
	double Y;
	double offset;
};


class Controls
{
public:
	void Update(Input::Raw &input, glm::vec2 correctedMousePos, glm::vec2 camOffset)
	{
		mousePos = glm::vec2(correctedMousePos.x + camOffset.x, correctedMousePos.y + camOffset.y);
		scrollDelta = input.offset;
		rightMouse = input.Buttons[GLFW_MOUSE_BUTTON_RIGHT];
		leftMouse = input.Buttons[GLFW_MOUSE_BUTTON_LEFT];

		up = input.Keys[GLFW_KEY_UP];
		down = input.Keys[GLFW_KEY_DOWN];
		left = input.Keys[GLFW_KEY_LEFT];
		right = input.Keys[GLFW_KEY_RIGHT];

		plus = input.Keys[GLFW_KEY_EQUAL];
		minus = input.Keys[GLFW_KEY_MINUS];
	}

	glm::vec2 MousePos() { return mousePos; }
	float ScrollDelta() { return scrollDelta; }

	bool LeftMouse() { return leftMouse; }
	bool RightMouse() { return rightMouse; }

	bool Up() { return up; }
	bool Down() { return down; }
	bool Left() { return left; }
	bool Right() { return right; }

	bool Plus() { return plus; }
	bool Minus() { return minus; }

private:
	glm::vec2 mousePos = glm::vec2(0.0f);
	float scrollDelta = 0.0f;
	bool leftMouse = false;
	bool rightMouse = false;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	bool plus = false;
	bool minus = false;
};



}


#endif
