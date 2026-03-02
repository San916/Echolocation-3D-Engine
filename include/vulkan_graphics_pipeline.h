#ifndef VULKAN_GRAPHICS_PIPELINE_H
#define VULKAN_GRAPHICS_PIPELINE_H

void create_graphics_pipeline(
    VkDevice logical_device,
    VkExtent2D swap_chain_extent, VkFormat swap_chain_image_format,
    VkPipelineLayout& pipeline_layout, VkRenderPass& render_pass, VkPipeline& graphics_pipeline
);

#endif