
#include "mod/cut_plane_modifier.h"

#include <cork.h>
#include <igl/writeOBJ.h>
#include <igl/per_vertex_normals.h>

namespace procrock {
CutPlaneModifier::CutPlaneModifier() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set various parameters."};
  config.insertToConfigGroups("General", group);
}

std::shared_ptr<Mesh> CutPlaneModifier::modify(Mesh& mesh) {
  auto result = std::make_shared<Mesh>();

  Eigen::Matrix<float, 8, 3> vertices;

  float half_length = 10;
  float half_height = 10;
  float half_depth = 10;

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

  CorkTriMesh mesh1;
  mesh1.n_vertices = mesh.vertices.rows();
  mesh1.n_triangles = mesh.faces.rows();
  std::vector<unsigned int> faces1;
  std::vector<float> vertices1;

  convert(mesh.vertices.cast<float>(), vertices1);
  convert(mesh.faces, faces1);

  mesh1.triangles = faces1.data();
  mesh1.vertices = vertices1.data();

  CorkTriMesh mesh2;
  mesh2.n_vertices = vertices.rows();
  mesh2.n_triangles = faces.rows();
  std::vector<unsigned int> faces2;
  std::vector<float> vertices2;

  convert(vertices, vertices2);
  convert(faces, faces2);

  mesh2.triangles = faces2.data();
  mesh2.vertices = vertices2.data();

  CorkTriMesh resultMesh;
  computeDifference(mesh1, mesh2, &resultMesh);

  result->vertices.resize(resultMesh.n_vertices, 3);
  result->faces.resize(resultMesh.n_triangles, 3);

  for (int i = 0; i < resultMesh.n_vertices; i++) {
    result->vertices.row(i).x() = (double)resultMesh.vertices[i * 3 + 0];
    result->vertices.row(i).y() = (double)resultMesh.vertices[i * 3 + 1];
    result->vertices.row(i).z() = (double)resultMesh.vertices[i * 3 + 2];
  }

  for (int i = 0; i < resultMesh.n_triangles; i++) {
    result->faces.row(i).x() = resultMesh.triangles[i * 3 + 0];
    result->faces.row(i).y() = resultMesh.triangles[i * 3 + 1];
    result->faces.row(i).z() = resultMesh.triangles[i * 3 + 2];
  }

  freeCorkTriMesh(&resultMesh);

  igl::writeOBJ("booleanTest.obj", result->vertices, result->faces);

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
PipelineStageInfo& CutPlaneModifier::getInfo() { return info; }

void CutPlaneModifier::convert(Eigen::MatrixXf vertices, std::vector<float>& vOut) {
  vOut.clear();
  vOut.resize(vertices.rows() * 3);
  for (int i = 0; i < vertices.rows(); i++) {
    vOut[i * 3 + 0] = vertices.row(i).x();
    vOut[i * 3 + 1] = vertices.row(i).y();
    vOut[i * 3 + 2] = vertices.row(i).z();
  }
}

void CutPlaneModifier::convert(Eigen::MatrixXi faces, std::vector<unsigned int>& fOut) {
  fOut.clear();
  fOut.resize(faces.rows() * 3);
  for (int i = 0; i < faces.rows(); i++) {
    fOut[i * 3 + 0] = faces.row(i).x();
    fOut[i * 3 + 1] = faces.row(i).y();
    fOut[i * 3 + 2] = faces.row(i).z();
  }
}
}  // namespace procrock
