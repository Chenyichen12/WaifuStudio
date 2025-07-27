#ifndef RENDER_CORE_RENDERER_H_
#define RENDER_CORE_RENDERER_H_
#include <glm/glm.hpp>
#include <memory>
#include <span>
#include <vector>

#include "editor/types.hpp"
#include "vulkan_driver.h"
namespace rdc {

struct ModelVertex {
  glm::vec2 position;
  glm::vec2 uv;
};

class Layer2dResource : public IRenderResource, public NoCopyable {
  friend class ModelRenderer;
  bool dirty = false;

  VkImage _image = VK_NULL_HANDLE;
  VmaAllocation _allocation = VK_NULL_HANDLE;
  VkImageView _image_view = VK_NULL_HANDLE;
  VulkanDriver *_driver = nullptr;
  Layer2dResource() = default;

  std::vector<ModelVertex> _vertices;
  std::vector<uint32_t> _indices;

 public:
  inline void MarkDirty() { dirty = true; }
  struct ImageConfig {
    VulkanDriver *driver = nullptr;
    CPUImage *image = nullptr;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    std::span<ModelVertex> vertices = {};
    std::span<uint32_t> indices = {};
  };
  static std::unique_ptr<Layer2dResource> CreateFromImage(
      const ImageConfig &config);
};

class ModelRenderer {
  VulkanDriver *_driver = nullptr;

 public:
  ModelRenderer(VulkanDriver *driver);
  ~ModelRenderer();

  void Render();
};
}  // namespace rdc
#endif  // RENDER_CORE_RENDERER_H_
