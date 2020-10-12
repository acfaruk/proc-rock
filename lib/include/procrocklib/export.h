#pragma once

#include <procrocklib/mesh.h>

#include <string>

namespace procrock {
void exportMesh(Mesh& mesh, const std::string filepath);
}