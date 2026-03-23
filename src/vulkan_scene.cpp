#include <iostream>
#include <cstdint>
#include <fstream>

#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan_acceleration_structure.h>
#include <vulkan_index_buffer.h>
#include <vulkan_object.h>
#include <vulkan_scene.h>
#include <vulkan_vertex_buffer.h>

void Scene::parse_object_file(const char* buf) {
    char object_file_name[256];
    if (std::sscanf(buf, "o %255s", object_file_name) != 1) {
        throw std::runtime_error("parse_object_file(): Incorrect object format!");
    }
    VulkanObject* new_object = new VulkanObject(object_file_name, logical_device, physical_device, command_pool, graphics_queue);
    objects.push_back(new_object);
}

void Scene::parse_object_property(const char* buf) {
    if (objects.size() == 0) {
        throw std::runtime_error("parse_object_property(): objects.size() must be greater than 0!");
    }
    glm::vec3 value;
    glm::ivec3 value_int;
    if (std::sscanf(buf, "%*c %f %f %f", &value.x, &value.y, &value.z) == 3) {
        if (buf[0] == 'p') objects[objects.size() - 1]->properties.position = value;
        else if (buf[0] == 'r') objects[objects.size() - 1]->properties.rotation = value;
        else if (buf[0] == 's') objects[objects.size() - 1]->properties.scale = value;
        else {
            throw std::runtime_error("parse_object_property(): Incorrect object property format!");
        }
        return;
    }

    if (std::sscanf(buf, "%*c %d", &value_int.x) == 1) {
        if (buf[0] == 'v') objects[objects.size() - 1]->properties.visible = value_int.x;
        else if (buf[0] == 'e') objects[objects.size() - 1]->properties.emitting = value_int.x;
        else {
            throw std::runtime_error("parse_object_property(): Incorrect object property format!");
        }
        return;
    }
    throw std::runtime_error("parse_object_property(): Incorrect object property format!");
}

Scene::Scene(
    const std::string& file_name, 
    VkDevice logical_device, 
    VkPhysicalDevice physical_device, 
    VkCommandPool command_pool, 
    VkQueue graphics_queue
) : file_name(file_name) {
    this->logical_device = logical_device;
    this->physical_device = physical_device;
    this->command_pool = command_pool;
    this->graphics_queue = graphics_queue;
    load_scene_file(); 
}

Scene::~Scene() {
    for (VulkanObject* object : objects) delete object;
}

void Scene::load_scene_file() {
    std::ifstream scene_file(file_name, std::ios::ate | std::ios::binary);
    if (scene_file.fail()) {
        throw std::runtime_error("load_scene_file(): Failed to load file!");
    }

    size_t file_size = scene_file.tellg();
    std::string content(file_size, '\0');
    scene_file.seekg(0);
    scene_file.read(content.data(), file_size);
    scene_file.close();

    char* ptr = content.data();
    const char* end = ptr + file_size;

    while (ptr < end) {
        char* line_end = ptr;
        while (line_end < end && *line_end != '\n' && *line_end != '\r') line_end++;

        char end_char = '\0';
        if (line_end < end) {
            end_char = *line_end;
            *line_end = '\0';
        }

        size_t len = line_end - ptr;
        if (len > 1) {
            if (ptr[0] == 'o' && ptr[1] == ' ') {
                parse_object_file(ptr);
            } else if ((ptr[0] == 'p' || ptr[0] == 'r' || ptr[0] == 's' || ptr[0] == 'v' || ptr[0] == 'e') && ptr[1] == ' ') {
                parse_object_property(ptr);
            }
        }

        if (line_end < end) {
            *line_end = end_char;
        }

        while (line_end < end && (*line_end == '\n' || *line_end == '\r')) line_end++;
        ptr = line_end;
    }
}