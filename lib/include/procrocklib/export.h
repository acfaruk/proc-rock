#pragma once

#include <procrocklib/mesh.h>

#include <string>

namespace procrock {
void exportMesh(Mesh& mesh, const std::string filepath, bool albedo = true, bool normals = true,
                bool roughness = true, bool metal = true, bool displace = true,
                bool ambientOcc = true);
}