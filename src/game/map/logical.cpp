#include "logical.h"

namespace Map
{

void Logical::SetPropsWithTiledMap(tiled::Map &map)
{
  mapRect = glm::vec4(0, 0, map.width * map.tileWidth, map.height * map.tileHeight);

  getObjGroupData(map);

  getTileData(map);
}

void Logical::getObjGroupData(tiled::Map &map)
{
  for(const auto &objGroup: map.objectGroups)
  {
    if(objGroup.props.switchLayer)
      switchRays.push_back(RayWithBox());
    if(objGroup.props.tilter)
      tilters.push_back(std::vector<Tilter>());
    if(objGroup.props.door)
      doorBox.push_back(DoorWithBox());
    if(objGroup.props.mover)
      movers.push_back(Mover());
    for(const auto &obj: objGroup.objs)
    {
      if( objGroup.props.mirror)
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
      if(obj.props.raySwitch && objGroup.props.switchLayer)
      {
        switchRays.back().box = glm::vec4(obj.x, obj.y, obj.w, obj.h);
      }
      if(obj.props.raySwitch && objGroup.props.door)
      {
        doorBox.back().box = glm::vec4(obj.x, obj.y, obj.w, obj.h);
      }
      if(obj.props.door && objGroup.props.door)
      {
        doorBox.back().doorRect = glm::vec4(obj.x, obj.y, obj.w, obj.h);
        doorBox.back().on = obj.props.on;
        doorBox.back().reflective = obj.props.mirror;
      }
      if(obj.props.playerSpawn)
        playerSpawn = glm::vec4(obj.x, obj.y, obj.w, obj.h);


      if(obj.props.goal ||  objGroup.props.goal)
        goal = glm::vec4(obj.x, obj.y, obj.w, obj.h);

      if(objGroup.props.tilter)
        tilters.back().push_back(Tilter(glm::vec4(obj.x, obj.y, obj.w, obj.h), glm::vec2(obj.x + obj.w/2, obj.y + obj.h/2), obj.props.angle));

      if(obj.props.mover && objGroup.props.mover)
      {
        movers.back().rect = glm::vec4(obj.x, obj.y, obj.w, obj.h);
        movers.back().reflective = obj.props.mirror;
      }
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

      if(poly.obj.props.track && objGroup.props.mover)
      {
        if(points.size() != 2)
          throw std::runtime_error("track points was not equal to two");
        movers.back().trackStart = points[0];
        movers.back().trackEnd = points[1];
      }
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
  }
}



}
