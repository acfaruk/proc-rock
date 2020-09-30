#include "mod/transformation_modifier.h"

#include <igl/per_vertex_normals.h>
#include <math.h>

#include <Eigen/Geometry>

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

  Eigen::Transform<double, 3, Eigen::Affine> transform;

  Eigen::AngleAxisd xAxisRot = Eigen::AngleAxisd(rotation.x(), Eigen::Vector3d{1.0, 0.0, 0.0});
  Eigen::AngleAxisd yAxisRot = Eigen::AngleAxisd(rotation.y(), Eigen::Vector3d{0.0, 1.0, 0.0});
  Eigen::AngleAxisd zAxisRot = Eigen::AngleAxisd(rotation.z(), Eigen::Vector3d{0.0, 0.0, 1.0});

  transform =
      Eigen::Translation3d(translation.cast<double>()) * Eigen::Scaling(scale.cast<double>());
  transform = transform * xAxisRot * yAxisRot * zAxisRot;

  for (int i = 0; i < mesh.vertices.rows(); i++) {
    result->vertices.row(i) = transform * (Eigen::Vector3d)result->vertices.row(i);
  }

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}
PipelineStageInfo& TransformationModifier::getInfo() { return info; }
}  // namespace procrock