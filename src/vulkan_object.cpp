#include <iostream>
#include <cstdint>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vulkan_acceleration_structure.h>
#include <vulkan_index_buffer.h>
#include <vulkan_object_loading.h>
#include <vulkan_object.h>
#include <vulkan_vertex_buffer.h>

VulkanObject::VulkanObject(const std::string& file_name) : file_name(file_name) {}

VulkanObject::~VulkanObject() {
    if (logical_device == VK_NULL_HANDLE) return;

    vkDestroyBuffer(logical_device, index_buffer, nullptr);
    vkFreeMemory(logical_device, index_buffer_memory, nullptr);

    vkDestroyBuffer(logical_device, vertex_buffer, nullptr);
    vkFreeMemory(logical_device, vertex_buffer_memory, nullptr);

    cleanup_acceleration_structure(logical_device, blas_buffer, blas_buffer_memory, blas);
}

void VulkanObject::init_object(VkDevice logical_device, VkPhysicalDevice physical_device, VkCommandPool command_pool, VkQueue graphics_queue) {
    this->logical_device = logical_device;

    load_obj_file(file_name, vertices, indices);
    create_vertex_buffer(logical_device, physical_device, command_pool, graphics_queue, vertices, vertex_buffer, vertex_buffer_memory);
    create_index_buffer(logical_device, physical_device, command_pool, graphics_queue, indices, index_buffer, index_buffer_memory);
    create_bottom_level_acceleration_structure(
        logical_device, physical_device,
        command_pool, graphics_queue,
        vertex_buffer, index_buffer,
        vertices, indices,
        blas_buffer, blas_buffer_memory, blas
    );
}
