#ifndef RENDERER_INCLUDE_H
#define RENDERER_INCLUDE_H

#ifdef GFX_ENV_VULKAN
  #include "vulkan-render/render.h"
  #include "vulkan-render/resources.h"
#endif

#ifdef GFX_ENV_OPENGL
  #include "opengl-render/render.h"
  #include "opengl-render/resources/resources.h"
#endif


#endif
