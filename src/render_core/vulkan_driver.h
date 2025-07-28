#ifndef SRC_RENDER_CORE_VULKAN_DRIVER_H_
#define SRC_RENDER_CORE_VULKAN_DRIVER_H_

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../tools.hpp"

namespace rdc {

void AssertVkResult(const VkResult &result);
void AssertVkResult(const VkResult &result, const char *message);
struct VulkanDriverConfig {
  std::vector<const char *> instance_extensions;
  std::vector<const char *> instance_layers;
  std::vector<const char *> device_extensions;
  uint32_t initial_height;
  uint32_t initial_width;
  std::function<VkResult(VkInstance instance, VkSurfaceKHR &surface)>
      create_surface_callback;
};

class VulkanDriver {
  VkInstance _instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;

  struct QueuePacket {
    uint32_t graphics_queue_family_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    uint32_t present_queue_family_index = UINT32_MAX;
    VkQueue present_queue = VK_NULL_HANDLE;
  } _queue_packet;
  VkDevice _device = VK_NULL_HANDLE;
  VkSurfaceKHR _surface = VK_NULL_HANDLE;
  VmaAllocator _vma_allocator = VK_NULL_HANDLE;
  class SwapchainPacket {
   public:
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat image_format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = {0, 0};
    std::vector<VkImage> images;
    std::vector<VkImageView> image_views;

    VkPresentModeKHR ChoosePresentMode() const;
    VkSurfaceFormatKHR ChooseSurfaceFormat() const;
    VkExtent2D ChooseExtent(uint32_t width, uint32_t height) const;
    void QuerySwapchainSupport(VkPhysicalDevice physical_device,
                               VkSurfaceKHR surface);
    void Destroy(VkDevice device);

    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
    VkSurfaceCapabilitiesKHR capabilities;

  } swapchain_packet;
  VkCommandPool _command_pool = VK_NULL_HANDLE;

  void CreateSwapchain(const VkExtent2D &extent);

 public:
  static std::unique_ptr<VulkanDriver> Create() { return nullptr; }
  VulkanDriver(const VulkanDriverConfig &config);

  // getter for vulkan driver
  inline const VkSwapchainKHR &GetSwapchain() const {
    return swapchain_packet.swapchain;
  }
  inline const VkFormat &GetSwapchainFormat() const {
    return swapchain_packet.image_format;
  }
  inline const std::vector<VkImageView> &GetSwapchainImageViews() const {
    return swapchain_packet.image_views;
  }

  inline const VkQueue &GetGraphicsQueue() const {
    return _queue_packet.graphics_queue;
  }
  inline const VkQueue &GetPresentQueue() const {
    return _queue_packet.present_queue;
  }
  inline const uint32_t &GetGraphicsQueueFamilyIndex() const {
    return _queue_packet.graphics_queue_family_index;
  }
  inline const uint32_t &GetPresentQueueFamilyIndex() const {
    return _queue_packet.present_queue_family_index;
  }

  inline const VkDevice &GetDevice() const { return _device; }
  inline const VkCommandPool &GetCommandPool() const { return _command_pool; }
  inline const VmaAllocator &GetVmaAllocator() const { return _vma_allocator; }

  // helpers
  VkSampler HCreateSimpleSampler() const;
  VkCommandBuffer HBeginOneTimeCommandBuffer() const;
  void HEndOneTimeCommandBuffer(const VkCommandBuffer &command_buffer,
                                const VkQueue &submit_queue) const;

  enum class TransitionState {
    kInit2Transfer,
    kTransfer2Read,
  };
  void HTransitionImageLayout(const VkCommandBuffer &cmd, const VkImage &image,
                              const TransitionState &state,
                              const VkImageSubresourceRange &range = {
                                  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .baseMipLevel = 0,
                                  .levelCount = 1,
                                  .baseArrayLayer = 0,
                                  .layerCount = 1,
                              });

  void HTransitionImageLayout(const VkCommandBuffer &cmd, const VkImage &image,
                              const VkAccessFlags src_access,
                              const VkAccessFlags dst_access,
                              const VkPipelineStageFlags src_stage,
                              const VkPipelineStageFlags dst_stage,
                              const VkImageLayout old_layout,
                              const VkImageLayout new_layout,
                              const VkImageSubresourceRange &range = {
                                  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .baseMipLevel = 0,
                                  .levelCount = 1,
                                  .baseArrayLayer = 0,
                                  .layerCount = 1,
                              }) const;
  void HCreateBuffer(uint32_t size, VkBufferUsageFlags usage,
                     VmaMemoryUsage vma_flags, VkBuffer &buffer,
                     VmaAllocation &allocation) const;

  ~VulkanDriver();
};

class IRenderResource {
  friend class RenderResourceManager;

 protected:
  uint32_t _id = UINT32_MAX;

 public:
  uint32_t GetId() const { return _id; }
  virtual ~IRenderResource() = default;
};

class ImageRenderResource : public IRenderResource {
  VkImage _image = VK_NULL_HANDLE;
  VmaAllocation _allocation = VK_NULL_HANDLE;
  VkImageView _image_view = VK_NULL_HANDLE;
  VulkanDriver *_driver = nullptr;

 public:
  ImageRenderResource(VkImage image, VmaAllocation allocation,
                      VkImageView image_view, VulkanDriver *driver)
      : _image(image),
        _allocation(allocation),
        _image_view(image_view),
        _driver(driver) {}
  ~ImageRenderResource() {
    vkDestroyImageView(_driver->GetDevice(), _image_view, nullptr);
    vmaDestroyImage(_driver->GetVmaAllocator(), _image, _allocation);
  }
};

class RenderResourceManager {
  std::unordered_map<uint32_t, std::unique_ptr<IRenderResource>> _resources;
  IdAllocator _id_allocator;

 public:
  template <typename T, typename... Args>
  T *CreateResource(Args &&...args) {
    auto resource = std::make_unique<T>(std::forward<Args>(args)...);
    resource->_id = _id_allocator.AllocateId();
    T *ptr = resource.get();
    _resources[ptr->_id] = std::move(resource);
    return ptr;
  }
  template <typename T>
  T *AddResource(std::unique_ptr<T> resource) {
    resource->_id = _id_allocator.AllocateId();
    T *ptr = resource.get();
    _resources[ptr->_id] = std::move(resource);
    return ptr;
  }
  template <typename T>
  T *AddResource(T *resource) {
    resource->_id = _id_allocator.AllocateId();
    T *ptr = resource;
    _resources[ptr->_id] = std::unique_ptr<T>(resource);
    return ptr;
  }

  template <typename T>
  T *GetResource(uint32_t id) {
    return static_cast<T *>(_resources[id].get());
  }

  void ReleaseResource(uint32_t id) { _resources.erase(id); }
};

}  // namespace rdc

#endif  // SRC_RENDER_CORE_VULKAN_DRIVER_H_
