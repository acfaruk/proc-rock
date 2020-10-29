#include "mod/transformation_modifier.h"

#include <igl/per_vertex_normals.h>
#include <math.h>

#include <Eigen/Geometry>

#include "utils/mesh.h"

namespace procrock {
TransformationModifier::TransformationModifier() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set scale, translation and rotation."};

  group.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Translation", "Translate the rock."}, &translation, {-2, -2, -2}, {2, 2, 2}});
  group.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Scale", "Scale the rock."}, &scale, {0, 0, 0}, {3, 3, 3}});
  group.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Rotate", "Rotate the rock."}, &rotation, {0, 0, 0}, {2 * M_PI, 2 * M_PI, 2 * M_PI}});

  config.insertToConfigGroups("General", group);
}

std::shared_ptr<Mesh> TransformationModifier::modify(Mesh& mesh) {
  auto result = std::make_shared<Mesh>(mesh);

  utils::transform(*result, translation, scale, rotation);

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}
PipelineStageInfo& TransformationModifier::getInfo() { return info; }
}  // namespace procrock