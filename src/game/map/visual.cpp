#include "level.h"


namespace Map
{

Visual::Visual(tiled::Map map, Render* render, Resource::Font mapFont)
{
  this->map = map;
	this->mapFont = mapFont;

	tileMats.resize(map.layers.size());
	toDraw.resize(map.width * map.height);
	tileRects.resize(map.width * map.height);
	int index = 0;
	for(unsigned int y = 0; y < map.height; y++)
		for(unsigned int x = 0; x < map.width; x++)
		{
			tileRects[index] = glm::vec4((x * map.tileWidth), (y * map.tileHeight), map.tileWidth, map.tileHeight);
			for(unsigned int i = 0; i < map.layers.size(); i++)
				tileMats[i].push_back(glmhelper::calcMatFromRect(tileRects[index], 0, 0.0f + (float)i/10.0f));
			index++;
		}


	tiles.resize(map.totalTiles + 1);
	tiles[0] = Tile();
	tiles[0].tileRect = glm::vec4(0, 0, 1, 1);
	for(const auto &tileset: map.tilesets)
	{
		Resource::Texture tex = render->LoadTexture(tileset.imageSource);
		unsigned int id = tileset.firstTileID;
		for(unsigned int y = 0; y < tileset.tileCount / tileset.columns; y++)
			for(unsigned int x = 0; x < tileset.columns; x++)
			{
				tiles[id] = Tile();
				tiles[id].texture = tex;
				tiles[id++].tileRect = glmhelper::calcTexOffset(glm::vec2(tileset.imageWidth, tileset.imageHeight),
					glm::vec4(tileset.margin + (x * (tileset.tileWidth + tileset.spacing)), tileset.margin + ( y * (tileset.tileHeight + tileset.spacing)), tileset.tileWidth, tileset.tileHeight));
			}
	}

	for(const auto &txt: map.texts)
	{
		mapTexts.push_back(
			MapText(
				glm::vec2(txt.obj.x, txt.obj.y),
				glm::vec4(txt.obj.x, txt.obj.y - txt.obj.h/1.5f, txt.obj.w, txt.obj.h),
				glm::vec4(txt.colour.r / 255.0f, txt.colour.g / 255.0f, txt.colour.b / 255.0f, txt.colour.a / 255.0f),
				txt.text,
				txt.pixelSize));
	}

}


void Visual::Update(glm::vec4 cameraRect, Timer &timer, std::vector<glm::vec4> *activeColliders)
{
	for(auto &txt: mapTexts)
	{
		txt.toDraw = gh::colliding(txt.rect, cameraRect);
	}

  activeColliders->clear();
	toDraw.clear();
	for(unsigned int tile = 0; tile < tileRects.size(); tile++)
		if(gh::colliding(cameraRect, tileRects[tile]))
			for(unsigned int layer = map.layers.size() - 1; layer >= 0 && layer < map.layers.size(); layer--)
				if(map.layers[layer].data[tile] != 0)
				{
					toDraw.push_back(TileDraw(tiles[map.layers[layer].data[tile]].texture, tileMats[layer][tile], tiles[map.layers[layer].data[tile]].tileRect));
					if(map.layers[layer].props.collidable)
					{
						activeColliders->push_back(tileRects[tile]);
					}
					//break; //only draw top layer tile of each tileRect
				}
}

void Visual::Draw(Render *render)
{
	for(auto &txt: mapTexts)
	{
		//render->DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(txt.rect, 0, -5.0f));
		if(txt.toDraw)
			render->DrawString(mapFont, txt.text,
				glm::vec2(txt.pos.x, txt.pos.y),
				 txt.pixelSize, 0, txt.colour, 0.0f);
	}
	for(unsigned int i = 0; i < toDraw.size(); i++)
	{
		render->DrawQuad(toDraw[i].tex, toDraw[i].tileMat, glm::vec4(1.0f), toDraw[i].texOffset);
	}
}

void Visual::getTilesetTexAndOffset(int tileID, Resource::Texture *tilesetTexture, glm::vec4 *texOffset)
{
  *tilesetTexture = tiles[tileID].texture;
  *texOffset = tiles[tileID].tileRect;
}

}
