
#include "mod/subdivision_modifier.h"

#include <igl/false_barycentric_subdivision.h>
#include <igl/loop.h>
#include <igl/per_vertex_normals.h>
#include <igl/upsample.h>

namespace procrock {
std::shared_ptr<Mesh> SubdivisionModifier::modify(Mesh& mesh) {
  auto result = std::make_shared<Mesh>();
  switch (mode) {
    case 0:  // Loop
      igl::loop(mesh.vertices, mesh.faces, result->vertices, result->faces, subdivisions);
      break;
    case 1:  // Upsample
      igl::upsample(mesh.vertices, mesh.faces, result->vertices, result->faces, subdivisions);
      break;
    case 2:  // False Barycentric
    {
      Mesh temp;
      temp.vertices = mesh.vertices;
      temp.faces = mesh.faces;
      for (int i = 0; i < subdivisions; ++i) {
        igl::false_barycentric_subdivision(temp.vertices, temp.faces, result->vertices,
                                           result->faces);
        temp.vertices = result->vertices;
        temp.faces = result->faces;
      }
      break;
    }
    default:
      assert(false);
      break;
  }

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
Configuration SubdivisionModifier::getConfiguration() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set various parameters of the subdivision."};
  group.ints.emplace_back(
      Configuration::BoundedEntry<int>{{"Subdivs", "How often to subdivide"}, &subdivisions, 1, 3});
  group.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Type", "Select the type to subdivide"},
      {{"Loop", "Smoothes the mesh"},
       {"Upsample", "No smoothing"},
       {"False Barycentric", "Each triangle becomes 3 triangles at their center"}},
      &mode});

  Configuration result;
  result.insertToConfigGroups("General", group);
  return result;
}
PipelineStageInfo& SubdivisionModifier::getInfo() { return info; }
}  // namespace procrock
