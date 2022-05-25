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
    RaySource() {}
    RaySource(glm::vec4 rect, float angle)
    {
      this->rect = rect;
      this->angle = angle;
    }
    glm::vec4 rect;
    float angle;
  };

  struct RayWithBox
  {

    bool on;
    RaySource ray;
    glm::vec4 box;
  };

  struct DoorWithBox
  {
    bool on;
    glm::vec4 doorRect;
    glm::vec4 box;
  };



  struct Tilter
  {
    Tilter(glm::vec4 rect, glm::vec2 pivot, int initialAngle)
    {
      this->rect = rect;
      this->pivot = pivot;
      this->initialAngle = initialAngle;
    }
    glm::vec4 rect;
    glm::vec2 pivot;
    float  initialAngle;
  };

  void SetPropsWithTiledMap(tiled::Map &map);

	std::vector<glm::vec4> colliders;
  std::vector<std::vector<glm::vec2>> polyColliders;
	std::vector<glm::vec4> mirrors;
  std::vector<std::vector<glm::vec2>> polyMirrors;
  std::vector<RaySource> raySources;
  std::vector<std::vector<Tilter>> tilters;
  std::vector<RayWithBox> switchRays;
  std::vector<DoorWithBox> doorBox;
  glm::vec4 goal;
	glm::vec4  playerSpawn;
  glm::vec4  mapRect;

private:
  void getObjGroupData(tiled::Map &map);
  void getTileData(tiled::Map &map);
};

}

#endif
