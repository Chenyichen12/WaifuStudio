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
    VulkanDriver *pdriver = nullptr;
    CPUImage *pimage = nullptr;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    std::span<ModelVertex> vertices = {};
    std::span<uint32_t> indices = {};
  };
  static std::unique_ptr<Layer2dResource> CreateFromImage(
      const ImageConfig &config);

  ~Layer2dResource() override;
};

class ModelRenderer {
  VulkanDriver *_driver = nullptr;
  VkSampler _layer_sampler = VK_NULL_HANDLE;
  std::vector<Layer2dResource *> _render_layers;
  VkDescriptorSetLayout _descriptor_set_layout = VK_NULL_HANDLE;

  // func
  PFN_vkCreateShadersEXT vkCreateShadersEXT = nullptr;
  PFN_vkDestroyShaderEXT vkDestroyShaderEXT = nullptr;

  struct Shader {
    VkShaderStageFlagBits stage_flag = VK_SHADER_STAGE_VERTEX_BIT;
    VkShaderEXT shader = VK_NULL_HANDLE;
    void Destroy(const VkDevice &device, const PFN_vkDestroyShaderEXT destroy) {
      destroy(device, shader, nullptr);
      shader = VK_NULL_HANDLE;
    };
  };
  Shader _vertex_shader;
  Shader _fragment_shader;

 public:
  ModelRenderer(VulkanDriver *driver);
  void AddLayer(Layer2dResource *layer);
  std::span<Layer2dResource *> GetLayers() { return _render_layers; }
  ~ModelRenderer();

  void Render();
};
}  // namespace rdc
#endif  // RENDER_CORE_RENDERER_H_
