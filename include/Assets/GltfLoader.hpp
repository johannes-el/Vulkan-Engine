#pragma once

#include <vector>
#include <filesystem>
#include "Mesh.hpp"

std::vector<Mesh> loadGltf(std::filesystem::path &path);
