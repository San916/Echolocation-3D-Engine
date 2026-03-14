#ifndef VULKAN_OBJECT_LOADING_H
#define VULKAN_OBJECT_LOADING_H

#include <string>
#include <vector>

#include <vulkan_vertex_buffer.h>

void load_obj_file(const std::string file_name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

#endif