#include "sprite.h"

Sprite::Sprite(Resource::Texture texture, glm::vec4 drawRect, float depth)
{
  this->texture = texture;
  this->drawRect = drawRect;
  this->depth = depth;
}

Sprite::Sprite(Resource::Texture texture, float depth)
{
  this->texture = texture;
  this->drawRect = glm::vec4(0, 0, texture.dim.x, texture.dim.y);
  this->depth = depth;
}

void Sprite::Update(glm::vec4 camRect)
{
  toDraw = gh::colliding(camRect, drawRect);
  if(changed)
  {
    changed = false;
    model = glmhelper::getModelMatrix(drawRect, rotation, depth);
  }
}

void Sprite::Draw(Render *render)
{
  if(toDraw)
  {
    render->DrawQuad(texture, model, colour, texOffset);
  }
}
