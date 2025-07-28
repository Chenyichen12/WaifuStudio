#include "renderer.h"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "canvas_sd.gen.h"
#include "vulkan_driver.h"

namespace rdc {
ModelRenderer::ModelRenderer(VulkanDriver *driver) {
  _driver = driver;
  _layer_sampler = _driver->HCreateSimpleSampler();
  // create graphics pipeline for dynamic renderering
  {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.push_back({
        .binding = shader_gen::canvas_sd::ubo.binding,
        .descriptorType = shader_gen::canvas_sd::ubo.desc_type,
        .descriptorCount = 1,
    });
    bindings.push_back({
        .binding = shader_gen::canvas_sd::main_tex.binding,
        .descriptorType = shader_gen::canvas_sd::main_tex.desc_type,
        .descriptorCount = 1,
    });

    // shader_gen::canvas_sd::
    VkDescriptorSetLayoutCreateInfo set0_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };
    vkCreateDescriptorSetLayout(_driver->GetDevice(), &set0_info, nullptr,
                                &_descriptor_set_layout);

    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &_descriptor_set_layout,
    };

    vkCreatePipelineLayout(_driver->GetDevice(), &layout_info, nullptr,
                           &_pipeline_layout);
  }
}
ModelRenderer::~ModelRenderer() {
  vkDestroyDescriptorSetLayout(_driver->GetDevice(), _descriptor_set_layout,
                               nullptr);
  vkDestroyPipelineLayout(_driver->GetDevice(), _pipeline_layout, nullptr);
  vkDestroySampler(_driver->GetDevice(), _layer_sampler, nullptr);
}

Layer2dResource::~Layer2dResource() {
  vmaDestroyImage(_driver->GetVmaAllocator(), _image, _allocation);
  vkDestroyImageView(_driver->GetDevice(), _image_view, nullptr);
}

std::unique_ptr<Layer2dResource> Layer2dResource::CreateFromImage(
    const ImageConfig &config) {
  auto result = std::unique_ptr<Layer2dResource>(new Layer2dResource());
  result->_driver = config.pdriver;

  // upload data
  auto cpu_image = config.pimage;

  VkDeviceSize size =
      cpu_image->width * cpu_image->height * cpu_image->channels;
  VkBuffer staging_buffer;
  VmaAllocation staging_allocation;
  config.pdriver->HCreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VMA_MEMORY_USAGE_CPU_ONLY, staging_buffer,
                                staging_allocation);
  void *data;
  vmaMapMemory(config.pdriver->GetVmaAllocator(), staging_allocation, &data);
  memcpy(data, cpu_image->data, size);
  vmaUnmapMemory(config.pdriver->GetVmaAllocator(), staging_allocation);

  // create vkimage
  VmaAllocationCreateInfo alloc_info = {
      .usage = VMA_MEMORY_USAGE_GPU_ONLY,
      .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  };
  VkImageCreateInfo image_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = config.format,
      .extent =
          {
              .width = cpu_image->width,
              .height = cpu_image->height,
              .depth = 1,
          },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  AssertVkResult(vmaCreateImage(config.pdriver->GetVmaAllocator(), &image_info,
                                &alloc_info, &result->_image,
                                &result->_allocation, nullptr),
                 "Failed to create image");

  VkCommandBuffer single_command_buffer =
      config.pdriver->HBeginOneTimeCommandBuffer();

  config.pdriver->HTransitionImageLayout(
      single_command_buffer, result->_image, 0, VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  VkBufferImageCopy copy_region = {
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .imageExtent =
          {
              .width = cpu_image->width,
              .height = cpu_image->height,
              .depth = 1,
          },
  };
  vkCmdCopyBufferToImage(single_command_buffer, staging_buffer, result->_image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

  config.pdriver->HTransitionImageLayout(
      single_command_buffer, result->_image, VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  config.pdriver->HEndOneTimeCommandBuffer(single_command_buffer,
                                           config.pdriver->GetGraphicsQueue());

  vmaDestroyBuffer(config.pdriver->GetVmaAllocator(), staging_buffer,
                   staging_allocation);

  // create image view
  VkImageViewCreateInfo image_view_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .image = result->_image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = config.format,
      .components =
          {
              .r = VK_COMPONENT_SWIZZLE_IDENTITY,
              .g = VK_COMPONENT_SWIZZLE_IDENTITY,
              .b = VK_COMPONENT_SWIZZLE_IDENTITY,
              .a = VK_COMPONENT_SWIZZLE_IDENTITY,
          },
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };
  AssertVkResult(
      vkCreateImageView(config.pdriver->GetDevice(), &image_view_info, nullptr,
                        &result->_image_view),
      "Failed to create image view");
  result->_vertices =
      std::vector<ModelVertex>(config.vertices.begin(), config.vertices.end());
  result->_indices =
      std::vector<uint32_t>(config.indices.begin(), config.indices.end());
  return result;
};

}  // namespace rdc