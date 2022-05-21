#include "level.h"

Level::Level(std::string filename, Render* render, Resource::Font mapFont)
{
	tiled::Map map = tiled::Map(filename);
	logical.SetPropsWithTiledMap(map);
	visual = Map::Visual(map, render, mapFont);
}


void Level::Update(glm::vec4 cameraRect, Timer &timer)
{
	lastCamRect = cameraRect;
	visual.Update(cameraRect, timer, &activeColliders);
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

}
