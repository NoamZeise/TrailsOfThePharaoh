#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont)
{
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);

	for(const auto &ray  : logical.raySources)
	{
		rays.push_back(LightRay(render->LoadTexture("textures/pixel.png"), ray.rect, ray.angle));
	}

	Resource::Texture mirror = render->LoadTexture("textures/level/mirror.png");
	for(const auto &tilter : logical.tilters)
	{
		Resource::Texture baseTexture;
		glm::vec4 baseOffset;
		visual.getTilesetTexAndOffset(tilter.tile, &baseTexture, &baseOffset);
		tilters.push_back(Tilter(Sprite(baseTexture, tilter.rect, 1.0f), baseOffset, Sprite(mirror, glm::vec4(0), 5.0f), tilter.pivot, tilter.initialAngle));
	}
}


void Level::Update(glm::vec4 cameraRect, Timer &timer, Input::Controls &controls)
{
	visual.Update(cameraRect, timer, &activeColliders);
	for(auto &tilter : tilters)
	{
		tilter.Update(cameraRect, controls);
	}
	for(auto &ray : rays)
	{
		ray.Update(logical.mirrors, logical.colliders, tilters, cameraRect);
	}
}

void Level::Draw(Render *render)
{
	#ifdef SEE_COLLIDERS
	for(const auto &rect: activeColliders)
	{
		render->DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(rect, 0, 5.0f), glm::vec4(1.0f));
	}
	#endif

	visual.Draw(render);
	for(auto &ray : rays)
	{
		ray.Draw(render);
	}
	for(auto& tilter : tilters)
	{
		tilter.Draw(render);
	}
}
