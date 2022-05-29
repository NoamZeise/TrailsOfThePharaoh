#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont, Audio::Manager *audio)
{
	this->audio = audio;

	mirrorTex = render->LoadTexture("textures/level/mirrorUncontrollable.png");
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);

	Resource::Texture mirror = render->LoadTexture("textures/level/mirror.png");
	Resource::Texture tilterBase = render->LoadTexture("textures/level/tilterBase.png");
	Resource::Texture tilterOutline = render->LoadTexture("textures/level/tilterOutline.png");

	for(const auto &tilterGroup : logical.tilters)
	{
		tilters.push_back(std::vector<Tilter>());
		for(const auto& tilter:  tilterGroup)
			tilters.back().push_back(Tilter(Sprite(tilterBase, tilter.rect, 0.5f)
			,Sprite(tilterOutline, tilter.rect, 0.49f),
			 Sprite(mirror, glm::vec4(0), 1.1f), tilter.pivot, tilter.initialAngle, audio));
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


void Level::Update(glm::vec4 cameraRect, float scale, Timer &timer, Input::Controls &controls)
{
	beingMoved = false;
	beingRotated = false;
	moveSelected = false;
	rotateSelected = false;

	visual.Update(cameraRect, timer, &activeColliders);

	tilterUpdate(cameraRect, controls);

	for(auto& mover: movers)
	{
		if(mover.isMoving())
			beingMoved = true;
		if(mover.isHovered())
		 	moveSelected = true;
		mover.Update(cameraRect, controls, lines);
	}

	if(updatesSinceNotChanged < 5)
		rayDependantUpdate(cameraRect, scale);

	if(goal.isOn())
	{
		if(!playingEnd)
		{
			playingEnd = true;
			audio->Play("audio/sfx/Crystal Charge.wav", false, 0.2f);
		}
		winTimer += timer.FrameElapsed();
	}
	else
	{
		if(playingEnd)
		{
			playingEnd = false;
			audio->Stop("audio/sfx/Crystal Charge.wav");
		}
		winTimer = 0.0f;
	}

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
				if(tilterGroup[i].isControlled())
					beingRotated = true;
				if(tilterGroup[i].isHovered())
					rotateSelected = true;
			}
	}
}

void Level::rayDependantUpdate(glm::vec4 cameraRect, float scale)
{
	for(auto &l: lines)
	{
		l.hitSource.clear();
		l.hitDest.clear();
		l.lightHit = false;
	}
	shaderRays.clear();
	raysChanged = true;
	for(auto &ray : rays)
	{
		ray.Update(lines, cameraRect);
		for(auto& shaderRay : ray.rays)
			shaderRays.push_back(shaderRay);
	}

	for(auto& raySwitch : raySwitches)
	{
		raySwitch.rayUpdate(lines, cameraRect);
		for(auto& shaderRay : raySwitch.lightRay.rays)
			shaderRays.push_back(shaderRay);
	}

	for(auto &dS : doorSwitches)
		dS.Update(lines, cameraRect);

	for(auto& raySwitch : raySwitches)
		raySwitch.Update(lines, cameraRect);

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

	for(auto& mover: movers)
		mover.Draw(render);

	for(int i = 0; i < toDrawTransforms.size(); i++)
	{
		if(true)
			render->DrawQuad(mirrorTex, toDrawTransforms[i], glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		else
			render->DrawQuad(mirrorTex, toDrawTransforms[i], glm::vec4(1.0f, 0.8f, 0.3f, 1.0f));
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
				toDrawTransforms.push_back(LightRay::GetLineTransform(lines.back().p1, lines.back().p2, LINE_ELEM_THICKNESS, lines.back().normal + 90.0f));
			}
		for(const auto &points: logical.polyMirrors)
			for(int i = 1; i < points.size(); i++)
			{
				lines.push_back(LightRay::LightElements(points[i-1], points[i], 0.0f, true));
				toDrawLines.push_back(lines.back());
				toDrawTransforms.push_back(LightRay::GetLineTransform(lines.back().p1, lines.back().p2, LINE_ELEM_THICKNESS, lines.back().normal + 90.0f));
			}



		int prevIndex = lines.size();
		addRectLine(logical.goal, false, 0.8f);
		goal = LightSwitch(false, prevIndex, 4, logical.goal,
			Sprite(
				render->LoadTexture("textures/level/goalOn.png"),
				glm::vec4(1.0f), 1.1f),
				Sprite(render->LoadTexture("textures/level/goalOff.png"),
				glm::vec4(1.0f), 1.1f)
		);

		auto onSprite = Sprite(
										render->LoadTexture("textures/level/lightBoxOn.png"),
										glm::vec4(1.0f),
										1.1f);
		auto offSprite = Sprite(
										render->LoadTexture("textures/level/lightBoxOff.png"),
										glm::vec4(1.0f),
										1.1f);

		for(const auto &switches: logical.switchRays)
		{
			int prevIndex = lines.size();
			addRectLine(switches.box, false, 1.0f);

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

	Sprite doorReflectiveSprite = Sprite(
		render->LoadTexture("textures/level/doorReflective.png"),
		glm::vec4(0.0f), 1.2f);

	Sprite doorOffSprite = Sprite(
		render->LoadTexture("textures/level/doorOff.png"),
		glm::vec4(0.0f), 1.2f);

	for(auto &doorSwitch: logical.doorBox)
	{
		int prevIndexSwitch = lines.size();
		addRectLine(doorSwitch.box, false, 0.75f);
		int prevIndexDoor = lines.size();
		addRectLine(doorSwitch.doorRect, doorSwitch.reflective, 1.0f);
		doorSwitches.push_back(DoorSwitch(
			doorSwitch.reflective ? doorReflectiveSprite : doorSprite,
			doorOffSprite, doorSwitch.doorRect, prevIndexDoor, doorSwitch.on,
			prevIndexSwitch, doorSwitch.box, onSprite, offSprite)
		);
	}

	Sprite moverColliderSprite = Sprite(render->LoadTexture("textures/level/solidBox.png"), glm::vec4(0.0f), 1.2f);
	Sprite moverMirrorSprite = Sprite(render->LoadTexture("textures/level/mirrorBox.png"), glm::vec4(0.0f), 1.2f);
	Sprite moverLineSprite = Sprite(render->LoadTexture("textures/level/lineMover.png"), glm::vec4(0.0f), 1.2f);
	Sprite moverHandleSprite = Sprite(render->LoadTexture("textures/level/handle.png"), glm::vec4(0.0f), 1.3f);
	Sprite moverOutlineSprite = Sprite(render->LoadTexture("textures/level/outlineBox.png"), glm::vec4(0.0f), 1.1f);
	for(auto& mover : logical.movers)
	{
		int prevIndex = lines.size();
		addRectLine(mover.rect, mover.reflective, 1.0f);
		movers.push_back(
			Mover(
				moverLineSprite,
				moverOutlineSprite,
				mover.reflective ? moverMirrorSprite : moverColliderSprite,
				moverHandleSprite,
				mover.rect,
				mover.trackStart,
				mover.trackEnd,
				prevIndex,
				audio
			)
		);
	}
}

void Level::addRectLine(glm::vec4 rect, bool reflective, float scale)
{
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x + rect.z *scale,
					rect.y+ rect.w *scale
				),
				glm::vec2(
					rect.x + rect.z - rect.z *scale,
					rect.y + rect.w *scale
				),
				0.0f,
				reflective
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x + rect.z *scale,
					rect.y+ rect.w *scale
				),
				glm::vec2(
					rect.x+ rect.z *scale,
					rect.y + rect.w - rect.w *scale
				),
				0.0f,
				reflective
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x + rect.z - rect.z *scale,
					rect.y + rect.w - rect.w *scale
				),
				glm::vec2(
					rect.x + rect.z - rect.z*scale,
					rect.y + rect.w *scale
				),
				0.0f,
				reflective
			)
		);
		lines.push_back(
			LightRay::LightElements(
				glm::vec2(
					rect.x + rect.z - rect.z *scale,
					rect.y + rect.w - rect.w *scale
				),
				glm::vec2(
					rect.x + rect.z *scale,
					rect.y + rect.w - rect.w*scale
				),
				0.0f,
				reflective
			)
		);
}
