
#include "mod/subdivision_modifier.h"

#include <igl/loop.h>
#include <igl/per_vertex_normals.h>
namespace procrock {
std::shared_ptr<Mesh> SubdivisionModifier::modify(Mesh& mesh) {
  auto result = std::make_shared<Mesh>();

  igl::loop(mesh.vertices, mesh.faces, result->vertices, result->faces, subdivisions);
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
Configuration SubdivisionModifier::getConfiguration() {
  Configuration result;

  result.ints.emplace_back(
      Configuration::BoundedEntry<int>{"Subdivs", "How often to subdivide", &subdivisions, 1, 3});
  return result;
}
PipelineStageInfo& SubdivisionModifier::getInfo() { return info; }
}  // namespace procrock
