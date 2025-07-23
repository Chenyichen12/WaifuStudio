#ifndef RENDER_CORE_RENDERER_H_
#define RENDER_CORE_RENDERER_H_
#include "vulkan_driver.h"
namespace rdc {
class ModelRenderer {
public:
  ModelRenderer(VulkanDriver *driver);
  ~ModelRenderer();

  void Render();
};
} // namespace rdc
#endif // RENDER_CORE_RENDERER_H_
