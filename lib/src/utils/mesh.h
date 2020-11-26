#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_3.h>
#include <CGAL/Surface_mesh.h>
#include <mesh.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace procrock {
namespace utils {

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> CGAL_Mesh;

void inline convert(const Mesh& from, CGAL_Mesh& to) {
  to.clear();
  for (int i = 0; i < from.vertices.rows(); i++) {
    float x = from.vertices.row(i).x();
    float y = from.vertices.row(i).y();
    float z = from.vertices.row(i).z();

    to.add_vertex(Point{x, y, z});
  }

  for (int i = 0; i < from.faces.rows(); i++) {
    auto v0 = CGAL::SM_Vertex_index(from.faces.row(i)(0));
    auto v1 = CGAL::SM_Vertex_index(from.faces.row(i)(1));
    auto v2 = CGAL::SM_Vertex_index(from.faces.row(i)(2));
    to.add_face(v0, v1, v2);
  }
}

void inline convert(const CGAL_Mesh& from, Mesh& to) {
  to.vertices.resize(from.vertices().size(), 3);

  for (auto index : from.vertices()) {
    auto point = from.point(index);
    to.vertices.row(index) << point.x(), point.y(), point.z();
  }

  to.faces.resize(from.faces().size(), 3);
  for (auto index : from.faces()) {
    CGAL::Vertex_around_face_circulator<CGAL_Mesh> vcirc(from.halfedge(index), from);

    to.faces.row(index)(0) = *vcirc++;
    to.faces.row(index)(1) = *vcirc++;
    to.faces.row(index)(2) = *vcirc;
  }
}

void inline transform(Mesh& mesh, const Eigen::Vector3f translation, const Eigen::Vector3f scale,
                      const Eigen::Vector3f rotation) {
  Eigen::Transform<double, 3, Eigen::Affine> transform;

  Eigen::AngleAxisd xAxisRot = Eigen::AngleAxisd(rotation.x(), Eigen::Vector3d{1.0, 0.0, 0.0});
  Eigen::AngleAxisd yAxisRot = Eigen::AngleAxisd(rotation.y(), Eigen::Vector3d{0.0, 1.0, 0.0});
  Eigen::AngleAxisd zAxisRot = Eigen::AngleAxisd(rotation.z(), Eigen::Vector3d{0.0, 0.0, 1.0});

  transform =
      Eigen::Translation3d(translation.cast<double>()) * Eigen::Scaling(scale.cast<double>());
  transform = transform * xAxisRot * yAxisRot * zAxisRot;

  for (int i = 0; i < mesh.vertices.rows(); i++) {
    mesh.vertices.row(i) = transform * (Eigen::Vector3d)mesh.vertices.row(i);
  }
}
}  // namespace utils
}  // namespace procrock