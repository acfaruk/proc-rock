#include "par/xatlas_parameterizer.h"

#include <xatlas.h>

namespace procrock {
XAtlasParameterizer::XAtlasParameterizer() {
  Configuration::ConfigurationGroup chartGroup;
  chartGroup.entry = {"Chart Options",
                      "Chart Options for XATLAS. The weights define how large the single charts "
                      "get. If the max cost per chart is reached, then new charts are generated. "
                      "(Resulting in more charts in the UV mapping.)"};

  chartGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Normal Deviation Weight", "Angle between face and avg. chart normal."},
      &chartOptions.normalDeviationWeight,
      0.0f,
      1000.0f});
  chartGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Roundness Weight", "There was no documentation available for this value."},
      &chartOptions.roundnessWeight,
      0.0f,
      1000.1f});
  chartGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Straighness Weight", "There was no documentation available for this value."},
      &chartOptions.straightnessWeight,
      0.0f,
      1000.1f});
  chartGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Normal Seam Weight", "Normals fully respected if > 1000"},
      &chartOptions.normalSeamWeight,
      0.0f,
      1000.1f});
  chartGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Texture Seam Weight", "There was no documentation available for this value."},
      &chartOptions.textureSeamWeight,
      0.0f,
      1000.1f});
  chartGroup.floats.emplace_back(
      Configuration::BoundedEntry<float>{{"Max. Cost",
                                          "If total of all metrics * weights > Max. Cost, then the "
                                          "chart wont grow bigger. (Creating more charts.)"},
                                         &chartOptions.maxCost,
                                         0.0f,
                                         1000.1f});
  chartGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Max. Iters", "Number of iterations. Higher values result in better charts."},
      &chartOptions.maxIterations,
      1,
      10});

  config.insertToConfigGroups("Chart Options", chartGroup);

  Configuration::ConfigurationGroup packGroup;
  packGroup.entry = {"Pack Options", "How to pack the charts generated."};
  packGroup.bools.emplace_back(Configuration::SimpleEntry<bool>{
      {"Bilinear",
       "Leave space around the charts for texels that would be sampled by bilinear filtering."},
      &packOptions.bilinear});
  packGroup.bools.emplace_back(Configuration::SimpleEntry<bool>{
      {"Block Align", "Align charts to 4x4 blocks. Also improves packing speed."},
      &packOptions.blockAlign});
  packGroup.bools.emplace_back(Configuration::SimpleEntry<bool>{
      {"Brute Force", "Gives best result, but is slower."}, &packOptions.bruteForce});
  packGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Padding", "Pixels to pad charts with."}, &packOptions.padding, 10, 50});

  config.insertToConfigGroups("Pack Options", packGroup);
}

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

  xatlas::ChartOptions cOpts;
  cOpts.normalDeviationWeight = chartOptions.normalDeviationWeight;
  cOpts.roundnessWeight = chartOptions.roundnessWeight;
  cOpts.straightnessWeight = chartOptions.straightnessWeight;
  cOpts.normalSeamWeight = chartOptions.normalSeamWeight;
  cOpts.textureSeamWeight = chartOptions.textureSeamWeight;

  cOpts.maxCost = chartOptions.maxCost;
  cOpts.maxIterations = chartOptions.maxIterations;

  xatlas::PackOptions pOpts;
  pOpts.bilinear = packOptions.bilinear;
  pOpts.blockAlign = packOptions.blockAlign;
  pOpts.bruteForce = packOptions.bruteForce;
  pOpts.padding = packOptions.padding;

  xatlas::Generate(atlas, cOpts, pOpts);

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

PipelineStageInfo& XAtlasParameterizer::getInfo() { return info; }
}  // namespace procrock