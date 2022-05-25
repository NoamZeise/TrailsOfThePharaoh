#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont)
{
	mirrorTex = render->LoadTexture("textures/pixel.png");
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);

	Resource::Texture mirror = render->LoadTexture("textures/level/mirror.png");
	Resource::Texture tilterBase = render->LoadTexture("textures/level/tilterBase.png");

	for(const auto &tilterGroup : logical.tilters)
	{
		tilters.push_back(std::vector<Tilter>());
		for(const auto& tilter:  tilterGroup)
			tilters.back().push_back(Tilter(Sprite(tilterBase, tilter.rect, 0.5f), Sprite(mirror, glm::vec4(0), 1.1f), tilter.pivot, tilter.initialAngle));
	}

	auto rayBox = Sprite(
								render->LoadTexture("textures/level/rayStationBase.png"),
								glm::vec4(1.0f),
								0.5f);

	auto rayBoxOn = Sprite(
									render->LoadTexture("textures/level/rayStationOn.png"),
									glm::vec4(1.0f),
									1.0f);
	auto rayBoxOff = Sprite(
									render->LoadTexture("textures/level/rayStationOff.png"),
									glm::vec4(1.0f),
									1.0f);

	setLineObjects(render, rayBox, rayBoxOn, rayBoxOff);

	for(const auto &ray  : logical.raySources)
	{
		rays.push_back(LightRay(render->LoadTexture("textures/pixel.png"), ray.rect, ray.angle, staticLinesOffset, rayBox, rayBoxOn, rayBoxOff));
	}
}


void Level::Update(glm::vec4 cameraRect, Timer &timer, Input::Controls &controls)
{
	visual.Update(cameraRect, timer, &activeColliders);

	tilterUpdate(cameraRect, controls);

	if(updatesSinceNotChanged < 2)
		rayDependantUpdate(cameraRect);

	//only recalculate rays on change
	if(!linesChanged)
		updatesSinceNotChanged++;
	else
		updatesSinceNotChanged = 0;
	linesChanged = false;
	for(auto &l: lines)
		if(l.changed)
			linesChanged = true;

}

void Level::tilterUpdate(glm::vec4 cameraRect, Input::Controls &controls)
{
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
}

void Level::rayDependantUpdate(glm::vec4 cameraRect)
{
	for(auto &l: lines)
	{
		l.hitSource.clear();
		l.hitDest.clear();
		l.lightHit = false;
	}

	for(auto &ray : rays)
		ray.Update(lines, cameraRect);

	for(auto& raySwitch : raySwitches)
		raySwitch.Update(lines, cameraRect);

	for(auto &dS : doorSwitches)
		dS.Update(lines, cameraRect);

	goal.Update(lines, cameraRect);
}

void Level::Draw(Render *render)
{
	#ifdef SEE_COLLIDERS
	for(const auto &rect: logical.colliders)
		render->DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(glm::vec4(rect.x, rect.y, rect.z + 10, rect.w + 10), 0, 5.0f), glm::vec4(1.0f));
	#endif

	visual.Draw(render);

	for(auto &ray : rays)
		ray.Draw(render);

	for(auto &dS : doorSwitches)
		dS.Draw(render);

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

	goal.Draw(render);

}


void Level::setLineObjects(Render *render, Sprite rayBox, Sprite rayBoxOn, Sprite rayBoxOff)
{
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

		auto onSprite = Sprite(
										render->LoadTexture("textures/level/lightBoxOn.png"),
										glm::vec4(1.0f),
										1.1f);
		auto offSprite = Sprite(
										render->LoadTexture("textures/level/lightBoxOff.png"),
										glm::vec4(1.0f),
										1.1f);

		int prevIndex = lines.size();
		addRectLine(logical.goal, false);
		goal = LightSwitch(false, prevIndex, 4, logical.goal, onSprite, offSprite);

		for(const auto &switches: logical.switchRays)
		{
			int prevIndex = lines.size();
			addRectLine(switches.box, false);
			raySwitches.push_back(RaySwitch(
					LightRay(render->LoadTexture("textures/pixel.png"), switches.ray.rect, switches.ray.angle, lines.size(), rayBox, rayBoxOn, rayBoxOff),
					switches.on,
					prevIndex,
					switches.box,
					onSprite,
					offSprite
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

	Sprite doorSprite = Sprite(
		render->LoadTexture("textures/level/door.png"),
		glm::vec4(0.0f), 1.2f);

	for(auto &doorSwitch: logical.doorBox)
	{
		int prevIndexSwitch = lines.size();
		addRectLine(doorSwitch.box, false);
		int prevIndexDoor = lines.size();
		addRectLine(doorSwitch.doorRect, false);
		doorSwitches.push_back(DoorSwitch(
					doorSprite, doorSwitch.doorRect, prevIndexDoor, doorSwitch.on,
					prevIndexSwitch, doorSwitch.box, onSprite, offSprite));
	}
}

void Level::addRectLine(glm::vec4 rect, bool reflective)
{
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x,
					rect.y
				),
				glm::vec2(
					rect.x + rect.z,
					rect.y
				),
				0.0f,
				reflective
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x,
					rect.y
				),
				glm::vec2(
					rect.x,
					rect.y + rect.w
				),
				0.0f,
				reflective
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x + rect.z,
					rect.y + rect.w
				),
				glm::vec2(
					rect.x + rect.z,
					rect.y
				),
				0.0f,
				reflective
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x + rect.z,
					rect.y + rect.w
				),
				glm::vec2(
					rect.x,
					rect.y + rect.w
				),
				0.0f,
				reflective
			)
		);
}
