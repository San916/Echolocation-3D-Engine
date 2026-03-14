#include <iostream>
#include <cstdint>
#include <fstream>

#include <unordered_map>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vulkan_index_buffer.h>
#include <vulkan_object_loading.h>
#include <vulkan_vertex_buffer.h>

// MODIFIES: vertices, seen_vertices, index_copies
// EFFECTS: Parses the vertex coordinates from the given line buffer
//     v x y z -> vertex{x, y, z}
//     Deduplicates vertices and stores indices leading to the same 
static void parse_vertex(
    const char* buf,
    std::vector<Vertex>& vertices,
    std::unordered_map<Vertex, uint32_t>& seen_vertices,
    std::vector<uint32_t>& index_copies
) {
    Vertex vertex{};
    if (std::sscanf(buf, "v %f %f %f", &vertex.pos.x, &vertex.pos.y, &vertex.pos.z) != 3) {
        throw std::runtime_error("parse_vertex(): Incorrect vertex format!");
    }
    auto [it, inserted] = seen_vertices.emplace(vertex, (uint32_t)vertices.size());
    if (inserted) vertices.push_back(vertex);
    index_copies.push_back(it->second);
}

// MODIFIES: indices
// EFFECTS: Parses the face line, remaps OBJ indices through index_copies,
//     Fan-triangulates for faces with > 3 vertices
static void parse_face(
    const char* buf,
    const std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& index_copies)
{
    uint32_t face_indices[64];
    int count = 0;

    buf = buf + 2;
    while (*buf && count < 64) {
        while (*buf && (*buf == ' ' || *buf == '\t')) buf++;
        if (!*buf) break;

        uint32_t index = 0;
        if (std::sscanf(buf, "%u", &index) != 1) break;
        face_indices[count] = index - 1;
        count++;

        while (*buf && *buf != ' ' && *buf != '\t') buf++;
    }

    if (count < 3) throw std::runtime_error("parse_face(): Face has fewer than 3 vertices!");

    uint32_t remapped[64];
    for (int i = 0; i < count; i++) {
        remapped[i] = index_copies[face_indices[i]];
    }

    glm::vec3 A = vertices[remapped[0]].pos;
    glm::vec3 ref_normal = glm::cross(vertices[remapped[1]].pos - A,
                                      vertices[remapped[2]].pos - A);

    for (int i = 1; i <= count - 2; i++) {
        uint32_t i1 = remapped[0];
        uint32_t ia = remapped[i];
        uint32_t ib = remapped[i + 1];
        glm::vec3 n = glm::cross(vertices[ia].pos - A, vertices[ib].pos - A);
        if (glm::dot(n, ref_normal) >= 0.0f) {
            indices.push_back(i1);
            indices.push_back(ia);
            indices.push_back(ib);
        } else {
            indices.push_back(i1);
            indices.push_back(ib);
            indices.push_back(ia);
        }
    }
}

// MODIFIES: vertices, indices
// EFFECTS: Loads .obj file, and fills the vertices and indices with the vertex coordinate and face indices, respectively
void load_obj_file(const std::string file_name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    std::ifstream object_file(file_name, std::ios::ate | std::ios::binary);
    if (object_file.fail()) {
        throw std::runtime_error("load_obj_file(): Failed to load file!");
    }

    size_t file_size = object_file.tellg();
    std::string content(file_size, '\0');
    object_file.seekg(0);
    object_file.read(content.data(), file_size);
    object_file.close();

    std::unordered_map<Vertex, uint32_t> seen_vertices;
    std::vector<uint32_t> index_copies;
    vertices.reserve(file_size / 32);
    index_copies.reserve(file_size / 32);
    indices.reserve(file_size / 10);
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
            if (ptr[0] == 'v' && ptr[1] == ' ') {
                parse_vertex(ptr, vertices, seen_vertices, index_copies);
            } else if (ptr[0] == 'f' && ptr[1] == ' ') {
                parse_face(ptr, vertices, indices, index_copies);
            }
        }

        if (line_end < end) {
            *line_end = end_char;
        }

        while (line_end < end && (*line_end == '\n' || *line_end == '\r')) line_end++;
        ptr = line_end;
    }
}