#include <iostream>

#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vulkan_utils.h>
#include <vulkan_vertex_buffer.h>

// MODIFIES: vertex_binding_description
// EFFECTS: Creates vertex input binding description and returns it
void get_vertex_binding_description(VkVertexInputBindingDescription& vertex_binding_description) {
    vertex_binding_description = VkVertexInputBindingDescription{};
    vertex_binding_description.binding = 0;
    vertex_binding_description.stride = sizeof(Vertex);
    vertex_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

// MODIFIES: vertex_attribute_descriptions
// EFFECTS: Creates vertex input attribute descriptions and returns it
void get_vertex_attribute_descriptions(std::vector<VkVertexInputAttributeDescription>& vertex_attribute_descriptions) {
    vertex_attribute_descriptions.resize(1);
    vertex_attribute_descriptions[0].binding = 0;
    vertex_attribute_descriptions[0].location = 0;
    vertex_attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attribute_descriptions[0].offset = offsetof(Vertex, pos);
}

// MODIFIES: vertex_buffer, vertex_buffer_memory
// EFFECTS: Creates vertex buffer handle, and initializes vertex buffer memory
void create_vertex_buffer(
    VkDevice logical_device, 
    VkPhysicalDevice physical_device, 
    std::vector<Vertex> vertices, 
    VkBuffer& vertex_buffer, 
    VkDeviceMemory& vertex_buffer_memory
) {
    create_buffer(
        logical_device, 
        physical_device, 
        vertices.size() * sizeof(vertices[0]), 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertex_buffer,
        vertex_buffer_memory
    );

    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.pNext = nullptr;
    buffer_create_info.size = vertices.size() * sizeof(vertices[0]);
    buffer_create_info.usage = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    void* data;
    vkMapMemory(logical_device, vertex_buffer_memory, 0, vertices.size() * sizeof(vertices[0]), 0, &data);
        memcpy(data, vertices.data(), (size_t) buffer_create_info.size);
    vkUnmapMemory(logical_device, vertex_buffer_memory);
}