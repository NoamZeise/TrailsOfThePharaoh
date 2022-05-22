#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont)
{
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);

	for(auto &ray  : logical.raySources)
	{
		rays.push_back(LightRay(render->LoadTexture("textures/pixel.png"), ray.rect, ray.angle));
	}
}


void Level::Update(glm::vec4 cameraRect, Timer &timer, Input::Controls &controls)
{
	lastCamRect = cameraRect;
	visual.Update(cameraRect, timer, &activeColliders);
	for(auto &ray : rays)
	{
		ray.Update(logical.mirrors, logical.colliders, cameraRect, controls);
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
}
