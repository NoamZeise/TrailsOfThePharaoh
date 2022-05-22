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

  void SetPropsWithTiledMap(tiled::Map &map)
  {
    mapRect = glm::vec4(0, 0, map.width * map.tileWidth, map.height * map.tileHeight);
    //get tile data
    int layerIndex = -1;
    for(const auto &layer: map.layers)
    {
      layerIndex++;
      if(layer.props.collidable)
      {
        size_t i = 0;
        for(unsigned int y = 0; y < map.height; y++)
          for(unsigned int x = 0; x < map.width; x++)
          {
            if(layer.data[i] != 0)
              colliders.push_back(glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight));
            i++;
          }
      }

      if(layer.props.mirror)
      {
        size_t i = 0;
        for(unsigned int y = 0; y < map.height; y++)
          for(unsigned int x = 0; x < map.width; x++)
          {
            if(layer.data[i] != 0)
              mirrors.push_back(glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight));
            i++;
          }
      }
    }

    //Get object group data
    for(const auto &objGroup: map.objectGroups)
    {
      for(const auto &obj: objGroup.objs)
      {
        if(obj.props.mirror || objGroup.props.mirror)
          mirrors.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));

        if(obj.props.collidable || objGroup.props.collidable)
          colliders.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));

        if(obj.props.lightSource || objGroup.props.lightSource)
         raySources.push_back(RaySource(glm::vec4(obj.x, obj.y, obj.w, obj.h),  obj.props.angle));

        if(obj.props.playerSpawn)
          playerSpawn = glm::vec4(obj.x, obj.y, obj.w, obj.h);
      }
    }

    colliders.push_back(glm::vec4(-100, -100, 100, mapRect.w + 100));
    colliders.push_back(glm::vec4(0, -100, mapRect.z, 90));
    colliders.push_back(glm::vec4(0, mapRect.w, mapRect.z, 100));
    colliders.push_back(glm::vec4(mapRect.z, -100, 100, mapRect.w + 100));
  }

	std::vector<glm::vec4> colliders;
	std::vector<glm::vec4> mirrors;
  std::vector<RaySource> raySources;
	glm::vec4  playerSpawn;
  glm::vec4  mapRect;
};

}

#endif
