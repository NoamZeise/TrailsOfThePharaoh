#ifndef TILED_MAP_H
#define TILED_MAP_H

#include <string>
#include <vector>
#include <string>
#include <iostream>

#include "tiled.h"
#include <timer.h>
#include <glmhelper.h>
#include "../../renderer-include.h"
#include <gamehelper.h>

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#endif

//#define SEE_COLLIDERS

class Map
{
public:
Map() {}
Map(std::string filename, Render* render, float scale, Resource::Font mapFont);
void Update(glm::vec4 cameraRect, Timer &timer);
void Draw(Render *render);

glm::vec4 getMapRect() { return mapRect; }
std::vector<glm::vec4> getColliders()
{
	return currentFrameColliders;
}

private:

	struct Tile
	{
		glm::vec4 tileRect;
		Resource::Texture texture;
	};

	struct MapText
	{
		MapText(glm::vec4 rect, glm::vec4 colour, std::string text, float pixelSize)
		{
			this->rect = rect;
			this->colour = colour;
			this->text = text;
			this->pixelSize = pixelSize;
		}
		glm::vec4 rect;
		glm::vec4 colour;
		std::string text;
		float pixelSize;
		bool toDraw = false;
	};

	struct TileDraw
	{
		TileDraw() {}
		TileDraw(Resource::Texture tex, glm::mat4 tileMat, glm::vec4 texOffset)
		{
			this->tex = tex;
			this->tileMat = tileMat;
			this->texOffset = texOffset;
		}
		Resource::Texture tex;
		glm::mat4 tileMat;
		glm::vec4 texOffset;
	};

	struct Switch
	{
		Switch(int index, glm::vec4 rect,glm::mat4 mat, bool active)
		{
			this->tileIndex = index;
			this->rect = rect;
			this->mat = mat;
			this->active = active;
		}
		int tileIndex;
		glm::vec4 rect;
		glm::mat4 mat;
		bool active = true;
	};

	tiled::Map map;
	std::vector<std::vector<glm::mat4>> tileMats;
	std::vector<MapText> mapTexts;
	std::vector<glm::vec4> tileRects;
	std::vector<Tile> tiles;
	//std::vector<glm::vec4> cameraRects;
	glm::vec4 mapRect;
	Resource::Font mapFont;

	std::vector<glm::vec4> currentFrameColliders;
	std::vector<glm::vec4> colliders;

	std::vector<TileDraw> toDraw;

	glm::vec4 lastCamRect;
};


#endif
