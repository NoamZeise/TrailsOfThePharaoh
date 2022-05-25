#ifndef TILED_MAP_H
#define TILED_MAP_H

#include <string>
#include <vector>
#include <string>
#include <iostream>

#include "../../renderer-include.h"
#include  "../../glm-include.h"
#include "../sprites/sprite.h"
#include "tiled.h"
#include <timer.h>
#include <input.h>

#include "logical.h"
#include "visual.h"
#include "elements/light_ray.h"
#include "elements/tilter.h"
#include "elements/ray_switch.h"
#include "elements/light_switch.h"
#include "elements/door_switch.h"
#include "elements/mover.h"

#include <glmhelper.h>
#include <gamehelper.h>

//#define SEE_COLLIDERS

class Level
{
public:
Level() {}
Level(std::string filename, Render* render, Resource::Font mapFont);
void Update(glm::vec4 cameraRect, Timer &timer, Input::Controls &controls);
void Draw(Render *render);

glm::vec4 getMapRect() { return logical.mapRect; }

bool complete() { return goal.isOn(); }

private:
	void setLineObjects(Render *render, Sprite rayBox, Sprite rayBoxOn, Sprite rayBoxOff);
	void tilterUpdate(glm::vec4 cameraRect, Input::Controls &controls);
	void rayDependantUpdate(glm::vec4 cameraRect);

	void addRectLine(glm::vec4 rect, bool reflective);

	Map::Logical logical;
	Map::Visual visual;

	std::vector<glm::vec4> activeColliders;
	std::vector<glm::vec4> mirrors;
	std::vector<LightRay> rays;
	std::vector<DoorSwitch> doorSwitches;
	std::vector<std::vector<Tilter>> tilters;
	std::vector<RaySwitch> raySwitches;
  std::vector<Mover> movers;

	int staticLinesOffset = 0;
	std::vector<LightRay::LightElements> lines;
	std::vector<LightRay::LightElements> toDrawLines;

	Resource::Texture mirrorTex;

	LightSwitch goal;

	bool linesChanged = true;
	int updatesSinceNotChanged = 0;
};


#endif
