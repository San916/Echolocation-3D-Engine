#ifndef VULKAN_SCENE_H
#define VULKAN_SCENE_H

#include <iostream>
#include <cstdint>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vulkan_object.h>

class Scene {
private:
    std::string file_name;

    VkDevice logical_device = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkQueue graphics_queue = VK_NULL_HANDLE;

    std::vector<VulkanObject*> objects;

    void parse_object_file(const char* buf);
    void parse_object_property(const char* buf);
    void load_scene_file();
public:
    Scene(const std::string& file_name, VkDevice logical_device, VkPhysicalDevice physical_device, VkCommandPool command_pool, VkQueue graphics_queue);
    ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    const std::vector<VulkanObject*>& get_objects() const { return objects; }
};

#endif