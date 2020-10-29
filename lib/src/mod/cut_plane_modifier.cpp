#include "mod/cut_plane_modifier.h"

#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <igl/per_vertex_normals.h>
#include <math.h>

#include <Eigen/Geometry>

#include "utils/mesh.h"

namespace procrock {
CutPlaneModifier::CutPlaneModifier() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set how the plane should be positioned."};
  group.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Translation", "Translate the plane in space."}, &translation, {-2, -2, -2}, {2, 2, 2}});
  group.float3s.emplace_back(
      Configuration::BoundedEntry<Eigen::Vector3f>{{"Rotation", "Rotate the plane in space."},
                                                   &rotation,
                                                   {0, 0, 0},
                                                   {2 * M_PI, 2 * M_PI, 2 * M_PI}});

  config.insertToConfigGroups("General", group);
}

std::shared_ptr<Mesh> CutPlaneModifier::modify(Mesh& mesh) {
  utils::CGAL_Mesh firstBoolMesh;
  utils::convert(mesh, firstBoolMesh);

  Eigen::Matrix<double, 8, 3> vertices;

  double half_length = 100;
  double half_height = 100;
  double half_depth = 100;

  vertices.row(0) << half_length, -half_height, -half_depth;
  vertices.row(1) << half_length, half_height, -half_depth;
  vertices.row(2) << 0, half_height, -half_depth;
  vertices.row(3) << 0, -half_height, -half_depth;

  vertices.row(4) << half_length, -half_height, half_depth;
  vertices.row(5) << half_length, half_height, half_depth;
  vertices.row(6) << 0, half_height, half_depth;
  vertices.row(7) << 0, -half_height, half_depth;

  Eigen::Matrix<int, 12, 3> faces;

  faces.row(0) << 0, 2, 1;
  faces.row(1) << 0, 3, 2;

  faces.row(2) << 0, 1, 5;
  faces.row(3) << 0, 5, 4;

  faces.row(4) << 1, 2, 6;
  faces.row(5) << 1, 6, 5;

  faces.row(6) << 2, 3, 7;
  faces.row(7) << 2, 7, 6;

  faces.row(8) << 3, 0, 4;
  faces.row(9) << 3, 4, 7;

  faces.row(10) << 4, 5, 6;
  faces.row(11) << 4, 6, 7;

  auto cubeMesh = std::make_shared<Mesh>();
  cubeMesh->vertices = vertices;
  cubeMesh->faces = faces;

  utils::transform(*cubeMesh, translation, {1, 1, 1}, rotation);

  utils::CGAL_Mesh secondBoolMesh;
  utils::convert(*cubeMesh, secondBoolMesh);

  utils::CGAL_Mesh resultMesh;
  CGAL::Polygon_mesh_processing::corefine_and_compute_difference(firstBoolMesh, secondBoolMesh,
                                                                 resultMesh);
  auto result = std::make_shared<Mesh>();

  utils::convert(resultMesh, *result);

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}
PipelineStageInfo& CutPlaneModifier::getInfo() { return info; }
}  // namespace procrock