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
#include  "../../glm-include.h"

#include "logical.h"
#include "visual.h"
#include "elements/light_ray.h"

//#define SEE_COLLIDERS

class Level
{
public:
Level() {}
Level(std::string filename, Render* render, Resource::Font mapFont);
void Update(glm::vec4 cameraRect, Timer &timer);
void Draw(Render *render);

glm::vec4 getMapRect() { return logical.mapRect; }

private:

	Map::Logical logical;
	Map::Visual visual;

	std::vector<glm::vec4> activeColliders;
	std::vector<glm::vec4> mirrors;
	std::vector<LightRay> rays;

	glm::vec4 lastCamRect;
};


#endif
