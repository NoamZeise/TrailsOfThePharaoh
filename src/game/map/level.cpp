#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont)
{
	mirrorTex = render->LoadTexture("textures/pixel.png");
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);

	Resource::Texture mirror = render->LoadTexture("textures/level/mirror.png");
	for(const auto &tilterGroup : logical.tilters)
	{
		tilters.push_back(std::vector<Tilter>());
		for(const auto& tilter:  tilterGroup)
		{
			Resource::Texture baseTexture;
			glm::vec4 baseOffset;
			visual.getTilesetTexAndOffset(tilter.tile, &baseTexture, &baseOffset);
			tilters.back().push_back(Tilter(Sprite(baseTexture, tilter.rect, 1.0f), baseOffset, Sprite(mirror, glm::vec4(0), 5.0f), tilter.pivot, tilter.initialAngle));
		}
	}

	//set Lines

	for(const auto &rect: logical.colliders)
	{
		if(rect.z == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x, rect.y + rect.w), 0.0f, false));
		if(rect.w == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x + rect.z, rect.y), 0.0f, false));
	}
	for(const auto &rect: logical.mirrors)
	{
		if(rect.z  == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x, rect.y + rect.w), 0.0f, true));
		if(rect.w == 0)
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.x + rect.z, rect.y), 0.0f, true));
	}
	for(const auto &points: logical.polyColliders)
		for(int i = 1; i < points.size(); i++)
		{
			lines.push_back(LightRay::LightElements(points[i-1], points[i], 0.0f, false));
			toDrawLines.push_back(lines.back());
		}
	for(const auto &points: logical.polyMirrors)
		for(int i = 1; i < points.size(); i++)
		{
			lines.push_back(LightRay::LightElements(points[i-1], points[i], 0.0f, true));
			toDrawLines.push_back(lines.back());
		}

	for(const auto &switches: logical.switchRays)
	{
		int prevIndex = lines.size();
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					switches.box.x,
					switches.box.y
				),
				glm::vec2(
					switches.box.x + switches.box.z,
					switches.box.y
				),
				0.0f,
				false
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					switches.box.x,
					switches.box.y
				),
				glm::vec2(
					switches.box.x,
					switches.box.y + switches.box.w
				),
				0.0f,
				false
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					switches.box.x + switches.box.z,
					switches.box.y + switches.box.w
				),
				glm::vec2(
					switches.box.x + switches.box.z,
					switches.box.y
				),
				0.0f,
				false
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					switches.box.x + switches.box.z,
					switches.box.y + switches.box.w
				),
				glm::vec2(
					switches.box.x,
					switches.box.y + switches.box.w
				),
				0.0f,
				false
			)
		);
		raySwitches.push_back(RaySwitch(
				LightRay(render->LoadTexture("textures/pixel.png"), switches.ray.rect, switches.ray.angle, lines.size()),
				switches.box,
				switches.on,
				prevIndex
			)
		);
	}

	staticLinesOffset = lines.size();

	for(auto &tilterGroup: tilters)
	{
		for(auto &tilter:  tilterGroup)
		{
			auto rect = tilter.getMirrorPoints();
			lines.push_back(LightRay::LightElements(glm::vec2(rect.x, rect.y), glm::vec2(rect.z, rect.w), tilter.getThickness(), true));
		}
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

	int lineOffsetIndex = 0;
	for(auto &tilterGroup: tilters)
	{
		float change = 0.0f;
		glm::vec4 colour = glm::vec4(1.0f);
		for(int i = 0; i < tilterGroup.size(); i++)
		{
			tilterGroup[i].Update(cameraRect, controls);
			auto rect = tilterGroup[i].getMirrorPoints();
			lines[staticLinesOffset +  lineOffsetIndex].Update(glm::vec2(rect.x, rect.y), glm::vec2(rect.z, rect.w));
			if(tilterGroup[i].wasChanged())
			{
				change = tilterGroup[i].getChanged();
				colour = tilterGroup[i].getColour();
			}

			if(colour == glm::vec4(1.0f))
				colour = tilterGroup[i].getColour();

			lineOffsetIndex++;
		}
			for(int i = 0; i < tilterGroup.size(); i++)
			{
				tilterGroup[i].setColour(colour);
				if(!tilterGroup[i].wasChanged() && change != 0.0f)
				{
					tilterGroup[i].offsetAngle(change);
				}
			}
	}
	if(updatesSinceNotChanged < 2)
	{
		for(auto &l: lines)
		{
			l.hitSource.clear();
			l.hitDest.clear();
			l.lightHit = false;
		}
		for(auto &ray : rays)
		{
			ray.Update(lines);
		}
		for(auto& raySwitch : raySwitches)
		{
			raySwitch.Update(lines);
		}
	}
	if(!linesChanged)
		updatesSinceNotChanged++;
	else
		updatesSinceNotChanged = 0;
	linesChanged = false;
	for(auto &l: lines)
	{
		if(l.changed)
			linesChanged = true;
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
		ray.Draw(render);

	for(auto &raySwitch:  raySwitches)
		raySwitch.Draw(render);

	for(auto& tilterGroup : tilters)
		for(auto& tilter: tilterGroup)
			tilter.Draw(render);

	for(auto& l: toDrawLines)
	{
		if(l.reflective)
			render->DrawQuad(mirrorTex, LightRay::GetLineTransform(l.p1, l.p2, 8.0f, l.normal + 90.0f), glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
		else
			render->DrawQuad(mirrorTex, LightRay::GetLineTransform(l.p1, l.p2, 8.0f, l.normal + 90.0f), glm::vec4(0.5f, 0.8f, 0.3f, 1.0f));
	}

}
