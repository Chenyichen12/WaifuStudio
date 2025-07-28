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
  vkCreateShadersEXT = reinterpret_cast<PFN_vkCreateShadersEXT>(
      vkGetDeviceProcAddr(_driver->GetDevice(), "vkCreateShadersEXT"));
  vkDestroyShaderEXT = reinterpret_cast<PFN_vkDestroyShaderEXT>(
      vkGetDeviceProcAddr(_driver->GetDevice(), "vkDestroyShaderEXT"));

  // create graphics pipeline for dynamic renderering
  {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.push_back({
        .binding = shader_gen::canvas_sd::ubo.binding,
        .descriptorType = shader_gen::canvas_sd::ubo.desc_type,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
    });
    bindings.push_back({
        .binding = shader_gen::canvas_sd::main_tex.binding,
        .descriptorType = shader_gen::canvas_sd::main_tex.desc_type,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    });

    VkDescriptorSetLayoutCreateInfo set0_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };
    vkCreateDescriptorSetLayout(_driver->GetDevice(), &set0_info, nullptr,
                                &_descriptor_set_layout);
  }
  {
    // shader objects
    _vertex_shader.stage_flag = VK_SHADER_STAGE_VERTEX_BIT;
    _fragment_shader.stage_flag = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkShaderCreateInfoEXT shader_create_infos[2];
    VkShaderCreateInfoEXT &vert_shader_create_info = shader_create_infos[0];
    vert_shader_create_info = {};
    vert_shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    vert_shader_create_info.pNext = nullptr;
    vert_shader_create_info.pName = "main";
    vert_shader_create_info.flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT;
    vert_shader_create_info.stage = _vertex_shader.stage_flag;
    vert_shader_create_info.nextStage = _fragment_shader.stage_flag;
    vert_shader_create_info.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    vert_shader_create_info.codeSize =
        sizeof(shader_gen::canvas_sd::vertex_spv);
    vert_shader_create_info.pCode = shader_gen::canvas_sd::vertex_spv;
    vert_shader_create_info.setLayoutCount = 1;
    vert_shader_create_info.pSetLayouts = &_descriptor_set_layout;

    VkShaderCreateInfoEXT &frag_shader_create_info = shader_create_infos[1];
    frag_shader_create_info = vert_shader_create_info;
    frag_shader_create_info.stage = _fragment_shader.stage_flag;
    frag_shader_create_info.nextStage = 0;
    frag_shader_create_info.codeSize =
        sizeof(shader_gen::canvas_sd::fragment_spv);
    frag_shader_create_info.pCode = shader_gen::canvas_sd::fragment_spv;

    VkShaderEXT shaderEXTs[2];

    AssertVkResult(vkCreateShadersEXT(
        driver->GetDevice(), 2, shader_create_infos, nullptr, shaderEXTs));
    _vertex_shader.shader = shaderEXTs[0];
    _fragment_shader.shader = shaderEXTs[1];
  }
}
void ModelRenderer::AddLayer(Layer2dResource *layer) {}
ModelRenderer::~ModelRenderer() {
  _vertex_shader.Destroy(_driver->GetDevice(), vkDestroyShaderEXT);
  _fragment_shader.Destroy(_driver->GetDevice(), vkDestroyShaderEXT);

  vkDestroyDescriptorSetLayout(_driver->GetDevice(), _descriptor_set_layout,
                               nullptr);
  vkDestroySampler(_driver->GetDevice(), _layer_sampler, nullptr);
}
void ModelRenderer::Render() {}

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