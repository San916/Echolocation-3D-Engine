#ifndef VULKAN_PHYSICAL_DEVICE_H
#define VULKAN_PHYSICAL_DEVICE_H

#include <optional>

#include <vector>

#include <vulkan/vulkan.h>

typedef struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family_index;
    std::optional<uint32_t> present_family_index;

    bool isComplete() {
        return graphics_family_index.has_value() && present_family_index.has_value();
    }
} QueueFamilyIndices;

const std::vector<const char*> required_device_extensions = {
    "VK_KHR_swapchain",
    "VK_KHR_acceleration_structure",
    "VK_KHR_ray_query",
    "VK_KHR_deferred_host_operations",
    "VK_KHR_buffer_device_address",
    "VK_EXT_descriptor_indexing",
};

void setup_physical_device(VkInstance& vk_instance, VkPhysicalDevice& physical_device, VkSurfaceKHR surface, QueueFamilyIndices &queue_family_indices);

#endif