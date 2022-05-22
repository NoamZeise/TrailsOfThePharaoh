#ifndef MAP_VISUAL_H
#define MAP_VISUAL_H

#include "../../renderer-include.h"g
#include  "../../glm-include.h"
#include "tiled.h"
#include <timer.h>
#include <glmhelper.h>
#include <gamehelper.h>

namespace Map
{

  class Visual
  {
  public:
  Visual() {}
  Visual(tiled::Map map, Render* render, Resource::Font mapFont);
  void Update(glm::vec4 cameraRect, Timer &timer, std::vector<glm::vec4> *activeColliders);
  void Draw(Render *render);

  private:

    struct Tile
    {
      glm::vec4 tileRect;
      Resource::Texture texture;
    };

    struct MapText
    {
      MapText(glm::vec2 pos, glm::vec4 rect, glm::vec4 colour, std::string text, float pixelSize)
      {
        this->pos = pos;
        this->rect = rect;
        this->colour = colour;
        this->text = text;
        this->pixelSize = pixelSize;
      }
      glm::vec4 rect;
      glm::vec2 pos;
      glm::vec4 colour;
      std::string text;
      float pixelSize;
      bool toDraw = false;
    };

    struct TileDraw
    {
      TileDraw() {}
      TileDraw(Resource::Texture tex, glm::mat4 tileMat, glm::vec4 texOffset)
      {
        this->tex = tex;
        this->tileMat = tileMat;
        this->texOffset = texOffset;
      }
      Resource::Texture tex;
      glm::mat4 tileMat;
      glm::vec4 texOffset;
    };

    tiled::Map map;

    std::vector<std::vector<glm::mat4>> tileMats;
    std::vector<MapText> mapTexts;
    std::vector<glm::vec4> tileRects;
    std::vector<Tile> tiles;
    //std::vector<glm::vec4> cameraRects;
    Resource::Font mapFont;

    std::vector<TileDraw> toDraw;
  };
}

#endif
