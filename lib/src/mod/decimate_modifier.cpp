#include "mod/decimate_modifier.h"

#include <igl/decimate.h>
#include <igl/per_vertex_normals.h>

namespace procrock {
std::shared_ptr<Mesh> DecimateModifier::modify(Mesh& mesh) {
  auto result = std::make_shared<Mesh>();
  faceCount = mesh.faces.rows();

  Eigen::VectorXi birthIndices;
  switch (mode) {
    case 0:
      igl::decimate(mesh.vertices, mesh.faces, std::max(4.0f, relativeValue * faceCount),
                    result->vertices, result->faces, birthIndices);
      break;
    case 1:
      igl::decimate(mesh.vertices, mesh.faces, absoluteValue, result->vertices, result->faces,
                    birthIndices);
      break;
    default:
      assert(0 && "handle all cases!");
      break;
  }

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
Configuration DecimateModifier::getConfiguration() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set various parameters of the decimation."};

  group.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Type", "Choose absolute or relative face count decimation."},
      {{"Relative", "Relative Face count"}, {"Absolute", "Absolute Face count"}},
      &mode});

  switch (mode) {
    case 0:
      group.floats.emplace_back(Configuration::BoundedEntry<float>{
          {"Rel. Value",
           "Relative value of faces to keep. E.g 0.4 means the mesh will keep max. 40% of its "
           "original face count."},
          &relativeValue,
          0.01,
          0.99});
      break;
    case 1:
      group.ints.emplace_back(
          Configuration::BoundedEntry<int>{{"Abs. Value",
                                            "Absolute value of faces to keep. E.g 100 means the "
                                            "mesh will be decimated to max. 100 faces."},
                                           &absoluteValue,
                                           4,
                                           faceCount - 1});
      break;
    default:
      assert(0 && "handle all cases!");
      break;
  }

  Configuration result;
  result.insertToConfigGroups("General", group);
  return result;
}
PipelineStageInfo& DecimateModifier::getInfo() { return info; }
}  // namespace procrock