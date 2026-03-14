#ifndef VULKAN_VERTEX_BUFFER_H
#define VULKAN_VERTEX_BUFFER_H

#include <vector>

#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

typedef struct Vertex {
    glm::vec3 pos;

    bool operator==(const Vertex& other) const {
        return pos == other.pos;
    }
} Vertex;

namespace std {
    template <> struct hash<Vertex> {
        std::size_t operator()(const Vertex& vertex) const {
            return hash<glm::vec3>()(vertex.pos);
        }
    };
}

void get_vertex_binding_description(VkVertexInputBindingDescription& vertex_binding_description);
void get_vertex_attribute_descriptions(std::vector<VkVertexInputAttributeDescription>& vertex_attribute_descriptions);
void create_vertex_buffer(
    VkDevice logical_device, 
    VkPhysicalDevice physical_device, 
    VkCommandPool command_pool, 
    VkQueue graphics_queue, 
    const std::vector<Vertex>& vertices, 
    VkBuffer& vertex_buffer, 
    VkDeviceMemory& vertex_buffer_memory
);

#endif