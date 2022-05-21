#include "sprite.h"

Sprite::Sprite(Resource::Texture texture, glm::vec4 drawRect)
{
  this->texture = texture;
  this->drawRect = drawRect;
}

void Sprite::Update(glm::vec4 camRect)
{
  toDraw = gh::colliding(camRect,  drawRect);
  model = glmhelper::getModelMatrix(drawRect, 0.0f, 0.0f);
}

void Sprite::Draw(Render *render)
{
  if(toDraw)
  {
    render->DrawQuad(texture, model, colour);
  }
}
