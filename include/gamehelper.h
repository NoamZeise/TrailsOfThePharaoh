#ifndef GAME_HELPER_H
#define GAME_HELPER_H

#ifdef  GFX_ENV_VULKAN
	#ifndef  GLM_FORCE_DEPTH_ZERO_TO_ONE
		#define GLM_FORCE_DEPTH_ZERO_TO_ONE
	#endif
#endif
#include <glm/glm.hpp>
#include <string>
#include <stdio.h>
#include <random>
#include <ctime>

namespace gh
{
	inline static bool colliding(glm::vec4 a, glm::vec4 b)
	{
		return  a.x < b.x + b.z &&
				a.x + a.z > b.x &&
				a.y < b.y + b.w &&
				a.y + a.w > b.y;
	}

	inline static bool aInB(glm::vec4 a, glm::vec4 b)
	{
		return a.x > b.x && a.x < b.x + b.z &&
				a.y > b.y && a.y < b.y + b.z;
	}

	inline static bool contains(glm::vec2 p, glm::vec4 r)
	{
		return r.x < p.x && p.x < r.x + r.z  && r.y < p.y && p.y < r.y + r.w;
	}

	inline static bool exists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
	}

	inline static float distance(glm::vec2 p1, glm::vec2 p2)
	{
		return sqrt(((p1.x - p2.x) * (p1.x - p2.x)) + ((p1.y - p2.y) * (p1.y - p2.y)));
	}

	inline static int threePointOrientation(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
	{
		int slope = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
		if(slope == 0)
			return 0;
		return slope > 0 ? 1 : -1;
	}

	inline float max(float a, float b)
	{
		return a > b ? a : b;
	}

	inline float min(float a, float b)
	{
		return a < b ? a : b;
	}

	inline bool onSegemnt(glm::vec2 p1, glm::vec2 p2,  glm::vec2 p3)
	{
		return p2.x <= max(p1.x, p3.x) && p2.x >= min(p1.x, p3.x) && p2.y <= max(p1.y, p3.y) && p2.y >= min(p1.y, p3.y);
	}

//TODO check for colinear
	inline static bool linesCross(glm::vec2 p1, glm::vec2 q1, glm::vec2 p2, glm::vec2 q2)
	{
		int orientation1 = threePointOrientation(p1, q1, p2);
		int orientation2 = threePointOrientation(p1, q1, q2);
		int orientation3 = threePointOrientation(p2, q2, p1);
		int orientation4 = threePointOrientation(p2, q2, q1);

		if(orientation1 != orientation2 && orientation3 != orientation4)
		{
			return true;
		}

		if(orientation1 == 0 && onSegemnt(p1, p2, q1))
			return true;

		if(orientation2 == 0 && onSegemnt(p1, q2, q1))
			return true;

		if(orientation3 == 0 && onSegemnt(p2, p1, q2))
			return true;

		if(orientation4 == 0 && onSegemnt(p2, q1, q2))
			return true;

		return false;
	}


class Random
{
public:
	Random()
	{
		randomGen = std::mt19937(time(0));
		posReal = std::uniform_real_distribution<float>(0, 1);
		real = std::uniform_real_distribution<float>(-1, 1);
	}

	float Real()
	{
		return real(randomGen);
	}
	float PositiveReal()
	{
		return posReal(randomGen);
	}
private:
	std::mt19937 randomGen;
	std::uniform_real_distribution<float> posReal;
	std::uniform_real_distribution<float> real;
};



} //end namespace



#endif
