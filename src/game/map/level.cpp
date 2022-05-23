#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont)
{
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);

	Resource::Texture mirror = render->LoadTexture("textures/level/mirror.png");
	for(const auto &tilter : logical.tilters)
	{
		Resource::Texture baseTexture;
		glm::vec4 baseOffset;
		visual.getTilesetTexAndOffset(tilter.tile, &baseTexture, &baseOffset);
		tilters.push_back(Tilter(Sprite(baseTexture, tilter.rect, 1.0f), baseOffset, Sprite(mirror, glm::vec4(0), 5.0f), tilter.pivot, tilter.initialAngle));
	}

	//set Lines

	for(const auto &rect: logical.colliders)
	{
		if(rect.z == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x, rect.y + rect.w), 1.0f, false));
		if(rect.w == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x + rect.z, rect.y), 1.0f, false));
	}
	for(const auto &rect: logical.mirrors)
	{
		if(rect.z  == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x, rect.y + rect.w), 1.0f, true));
		if(rect.w == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x + rect.z, rect.y), 1.0f, true));
	}
	staticLinesOffset = lines.size();

	for(auto &tilter: tilters)
	{
		auto rect = tilter.getMirrorPoints();
		lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.z, rect.w), tilter.getThickness(), true));
	}

	//set light sources

	for(const auto &ray  : logical.raySources)
	{
		rays.push_back(LightRay(render->LoadTexture("textures/pixel.png"), ray.rect, ray.angle, staticLinesOffset));
	}
}


void Level::Update(glm::vec4 cameraRect, Timer &timer, Input::Controls &controls)
{
	visual.Update(cameraRect, timer, &activeColliders);

	for(int i = 0; i < tilters.size(); i++)
	{
		tilters[i].Update(cameraRect, controls);
		auto rect = tilters[i].getMirrorPoints();
		lines[staticLinesOffset + i].Update(glm::vec2(rect.x, rect.y), glm::vec2(rect.z, rect.w));
	}
	for(auto &ray : rays)
	{
		ray.Update(lines);
	}
}

void Level::Draw(Render *render)
{
	#ifdef SEE_COLLIDERS
	for(const auto &rect: logical.colliders)
	{
		render->DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(glm::vec4(rect.x, rect.y, rect.z + 10, rect.w + 10), 0, 5.0f), glm::vec4(1.0f));
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
