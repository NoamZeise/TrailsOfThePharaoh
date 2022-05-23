#ifndef LOGICAL_MAP_H
#define LOGICAL_MAP_H

#include <vector>
#include "tiled.h"
#include "../../glm-include.h"

namespace Map
{

struct Logical
{
  struct RaySource
  {
    RaySource(glm::vec4 rect, float angle)
    {
      this->rect = rect;
      this->angle = angle;
    }
    glm::vec4 rect;
    float angle;
  };

  struct Tilter
  {
    Tilter(glm::vec4 rect, glm::vec2 pivot, int tile)
    {
      this->rect = rect;
      this->pivot = pivot;
      this->tile = tile;
    }
    glm::vec4 rect;
    glm::vec2 pivot;
    int tile;
  };

  void SetPropsWithTiledMap(tiled::Map &map);

	std::vector<glm::vec4> colliders;
	std::vector<glm::vec4> mirrors;
  std::vector<RaySource> raySources;
  std::vector<Tilter> tilters;
  std::vector<glm::vec4> goals;
	glm::vec4  playerSpawn;
  glm::vec4  mapRect;
  glm::vec4 goal;

private:
  void getObjGroupData(tiled::Map &map);
  void getTileData(tiled::Map &map);
  void setTilterAngles();
};

}

#endif
