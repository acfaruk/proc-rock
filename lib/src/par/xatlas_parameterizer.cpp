#include "par/xatlas_parameterizer.h"

#include "utils/third_party/xatlas.h"

namespace procrock {
std::shared_ptr<Mesh> XAtlasParameterizer::parameterize(Mesh* mesh) {
  typedef Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> FData;
  typedef Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor> IData;

  xatlas::Atlas* atlas = xatlas::Create();

  xatlas::MeshDecl meshDecl;
  meshDecl.vertexCount = mesh->vertices.rows();
  meshDecl.indexCount = mesh->faces.size();

  float* mappedVertices = new float[mesh->vertices.size()];
  float* mappedNormals = new float[mesh->normals.size()];
  int* mappedFaces = new int[mesh->faces.size()];

  Eigen::Map<FData>(mappedVertices, mesh->vertices.rows(), mesh->vertices.cols()) =
      mesh->vertices.cast<float>();
  Eigen::Map<FData>(mappedNormals, mesh->normals.rows(), mesh->normals.cols()) =
      mesh->normals.cast<float>();
  Eigen::Map<IData>(mappedFaces, mesh->faces.rows(), mesh->faces.cols()) = mesh->faces;

  meshDecl.vertexPositionData = mappedVertices;
  meshDecl.vertexPositionStride = sizeof(float) * 3;

  meshDecl.vertexNormalData = mappedNormals;
  meshDecl.vertexNormalStride = sizeof(float) * 3;

  meshDecl.indexData = mappedFaces;
  meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

  xatlas::AddMesh(atlas, meshDecl);
  xatlas::Generate(atlas);

  auto& atlasMesh = atlas->meshes[0];
  Eigen::MatrixXd newVertices(atlasMesh.vertexCount, 3);
  Eigen::MatrixXd newNormals(atlasMesh.vertexCount, 3);
  Eigen::MatrixXi newFaces(atlasMesh.indexCount / 3, 3);
  Eigen::MatrixXd newUvs(atlasMesh.vertexCount, 2);

  for (int i = 0; i < atlasMesh.vertexCount; i++) {
    auto& vertex = atlasMesh.vertexArray[i];
    newVertices.row(i) = mesh->vertices.row(vertex.xref);
    newNormals.row(i) = mesh->normals.row(vertex.xref);
    newUvs.row(i) << vertex.uv[0] / (float)atlas->width, vertex.uv[1] / (float)atlas->height;
  }

  for (int i = 0; i < atlasMesh.indexCount; i++) {
    newFaces(i / 3, i % 3) = atlasMesh.indexArray[i];
  }

  auto result = std::make_shared<Mesh>();
  result->vertices = newVertices;
  result->normals = newNormals;
  result->faces = newFaces;
  result->uvs = newUvs;

  xatlas::Destroy(atlas);
  delete[] mappedVertices;
  delete[] mappedNormals;
  delete[] mappedFaces;
  return result;
}

Configuration XAtlasParameterizer::getConfiguration() {
  Configuration result = getBaseConfiguration();
  return result;
}

PipelineStageInfo& XAtlasParameterizer::getInfo() { return info; }
}  // namespace procrock