#include "logical.h"

namespace Map
{

void Logical::SetPropsWithTiledMap(tiled::Map &map)
{
  mapRect = glm::vec4(0, 0, map.width * map.tileWidth, map.height * map.tileHeight);

  getObjGroupData(map);

  getTileData(map);

  setTilterAngles();
}

void Logical::getObjGroupData(tiled::Map &map)
{
  for(const auto &objGroup: map.objectGroups)
  {
    if(objGroup.props.switchLayer)
      switchRays.push_back(RayWithBox());
    for(const auto &obj: objGroup.objs)
    {
      if(obj.props.mirror || objGroup.props.mirror)
        mirrors.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));

      if(obj.props.collidable || objGroup.props.collidable)
        colliders.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));

      if(obj.props.lightSource || objGroup.props.lightSource)
      {
        if(objGroup.props.switchLayer)
        {
          switchRays.back().ray = RaySource(glm::vec4(obj.x, obj.y, obj.w, obj.h),  obj.props.angle);
          switchRays.back().on = obj.props.on;
        }
        else
          raySources.push_back(RaySource(glm::vec4(obj.x, obj.y, obj.w, obj.h),  obj.props.angle));
      }
      if(obj.props.raySwitch)
      {
        switchRays.back().box = glm::vec4(obj.x, obj.y, obj.w, obj.h);
      }

      if(obj.props.playerSpawn)
        playerSpawn = glm::vec4(obj.x, obj.y, obj.w, obj.h);


      if(obj.props.goal ||  objGroup.props.goal)
        goal = glm::vec4(obj.x, obj.y, obj.w, obj.h);
    }
    for(const auto &poly: objGroup.polys)
    {
      std::vector<glm::vec2> points;
      for(auto& p: poly.points)
        points.push_back(glm::vec2(p.x + poly.obj.x,  p.y + poly.obj.y));

      if(poly.closed)
        points.push_back(points[0]);

      if(poly.obj.props.mirror || objGroup.props.mirror)
        polyMirrors.push_back(points);

      else if(poly.obj.props.collidable || objGroup.props.collidable)
        polyColliders.push_back(points);
    }
  }
}

void Logical::getTileData(tiled::Map &map)
{
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

    if(layer.props.tilter)
    {
      tilters.push_back(std::vector<Tilter>());
      size_t i = 0;
      for(unsigned int y = 0; y < map.height; y++)
        for(unsigned int x = 0; x < map.width; x++)
        {
          if(layer.data[i] != 0)
            tilters.back().push_back(Tilter(glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight), glm::vec2(0), layer.data[i]));
          i++;
        }
    }
  }
}

void Logical::setTilterAngles()
{
  int minTile = 6; //TODO: get without hardcoding?
  //assuming tilter tilemap order -> 0:Up, 1:Right, 2:Down, 3:Left
  for(auto &tilterGroup : tilters)
  {
    for(auto& tilter  : tilterGroup)
    {
      int direction  = tilter.tile - minTile;
      tilter.initialAngle = direction * 90.0f;
      switch(direction)
      {
        case 0:
          tilter.pivot = glm::vec2(tilter.rect.x + tilter.rect.z/2, tilter.rect.y + tilter.rect.w/5);
          break;
        case 1:
          tilter.pivot = glm::vec2(tilter.rect.x + tilter.rect.z - tilter.rect.z/5, tilter.rect.y + tilter.rect.w/2);
          break;
        case 2:
          tilter.pivot = glm::vec2(tilter.rect.x + tilter.rect.z/2, tilter.rect.y + tilter.rect.w - + tilter.rect.w/5);
          break;
        case 3:
          tilter.pivot = glm::vec2(tilter.rect.x + tilter.rect.z/5, tilter.rect.y + tilter.rect.w/2);
          break;
      }
    }
  }
}


}
