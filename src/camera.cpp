#include "camera.h"

namespace Camera
{
	FirstPerson::FirstPerson(glm::vec3 position)
	{
		_position = position;
		calculateVectors();
	}

	glm::mat4 FirstPerson::getViewMatrix()
	{
		if(viewUpdated)
		{
			view = glm::lookAt(_position, _position + _front, _up);
			viewUpdated = false;
			return view;
		}
		return view;
	}

	float FirstPerson::getZoom()
	{
		return _zoom;
	}


	void FirstPerson::update(Input::Raw &input, Input::Raw &prevInput, Timer &timer)
	{
		viewUpdated = true;
		//keyboard
		float velocity = _speed * timer.FrameElapsed();
		if(input.Keys[GLFW_KEY_W])
			_position += _front * velocity;
		if(input.Keys[GLFW_KEY_A])
			_position -= _right * velocity;
		if(input.Keys[GLFW_KEY_S])
			_position -= _front * velocity;
		if(input.Keys[GLFW_KEY_D])
			_position += _right * velocity;
		if(input.Keys[GLFW_KEY_SPACE])
			_position += _worldUp * velocity;
		if(input.Keys[GLFW_KEY_LEFT_SHIFT])
			_position -= _worldUp * velocity;

		//mouse
		_pitch   += (float)(prevInput.Y - input.Y) * _sensitivity;
		_yaw 	 += (float)(prevInput.X - input.X) * _sensitivity;

		if(_pitch > 89.0f)
			_pitch = 89.0f;
		if(_pitch < -89.0f)
		 _pitch = -89.0f;

		//scroll
		_zoom -= (float)input.offset * timer.FrameElapsed();
		if(_zoom < 1.0f)
			_zoom = 1.0f;
		if(_zoom > 100.0f)
			_zoom = 100.0f;

		calculateVectors();
		//std::cout << "X:" << _position.x << " Y:" << _position.y << " Z:" << _position.z << std::endl;
	}

	void FirstPerson::calculateVectors()
	{
		_front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_front.y = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_front.z = sin(glm::radians(_pitch));
		_front = glm::normalize(_front);

		_right = glm::normalize(glm::cross(_front, _worldUp));
		_up = glm::normalize(glm::cross(_right, _front));
	}

	float RoomFollow2D::getOffset(float target, float focus, float roomPos, float roomSize)
	{
		if(target > roomSize)
				return -roomSize/2 -roomPos;
		float min = roomPos + target/2;
		float max = roomPos + roomSize - target/2;
		if(focus > min && focus < max)
			return -focus;
		if(focus < min)
			return -min;
		if(focus > max)
			return -max;
		return -focus;
	}


	void RoomFollow2D::Target(glm::vec2 focus, Timer &timer)
		{

			int correctedWidth = settings::TARGET_WIDTH*scale;
			int correctedHeight = settings::TARGET_HEIGHT*scale;
			glm::vec2 transform = glm::vec2(0);

			if(mapRect.z == 0 && cameraRects.size() == 0)
			{
				transform.x = -focus.x;
				transform.y = -focus.y;
			}
			else if(cameraRects.size() == 0)
			{
				transform.x = getOffset(correctedWidth, focus.x, mapRect.x, mapRect.z);
				transform.y = getOffset(correctedHeight, focus.y, mapRect.y, mapRect.w);
			}
			else
			{
				glm::vec4 newRect = glm::vec4(0);
				currentRoom = glm::vec4(0);
				for(const auto& rect: cameraRects)
				{
					if(gh::contains(focus, rect))
					{
						newRect = rect;
						currentRoom = rect;
						break;
					}
				}
				if(newRect == glm::vec4(0))
				{
					transform.x = -focus.x;
					transform.y = -focus.y;
				}
				else
				{
					if (newRect != currentRect)
						currentRect = newRect;
					transform.x = getOffset(correctedWidth, focus.x, currentRect.x, currentRect.z);
					transform.y = getOffset(correctedHeight, focus.y, currentRect.y, currentRect.w);
				}

				if(mapRect != glm::vec4(0))
				{
					transform.x = getOffset(correctedWidth, -transform.x, mapRect.x, mapRect.z);
					transform.y = getOffset(correctedHeight, -transform.y, mapRect.y, mapRect.w);
				}
			}

			if(previousOff.x != 0 || previousOff.y != 0)
			{
			previousOff += glm::vec2(((transform.x - previousOff.x)/CAM2D_FLOAT) * timer.FrameElapsed(),
				((transform.y - previousOff.y)/CAM2D_FLOAT)* timer.FrameElapsed());
			transform = previousOff;
			}

			cameraArea = glm::vec4(-transform.x - correctedWidth/2, -transform.y - correctedHeight/2,
									(correctedWidth), (correctedHeight));
			offset = glm::translate(glm::mat4(1.0f), glm::vec3((int)(transform.x + correctedWidth/2.0f),
				 					(int)(transform.y + correctedHeight/2.0f), 0));
			previousOff = transform;
		}



} //namespace end
