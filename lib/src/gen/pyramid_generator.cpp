#include "gen/pyramid_generator.h"

#include "igl/per_vertex_normals.h"

namespace procrock {
PyramidGenerator::PyramidGenerator() {
  Configuration::ConfigurationGroup group;
  group.entry = {"Base", "Set parameters affecting the base of the pyramid."};

  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Height", "At which height the base should be."}, &baseHeight, -2, 0});
  group.floats.emplace_back(
      Configuration::BoundedEntry<float>{{"Radius", "Radius of the base."}, &baseRadius, 0.1, 3});
  group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Subdivisions", "Amount of sides at the base."}, &baseSubdivisions, 4, 30});
  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Wiggle amount", "Wiggle the vertices at the base."}, &baseWiggleAmount, 0, 1});
  group.ints.emplace_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for the wiggling."}, &seed, 0, 100000});

  Configuration::ConfigurationGroup other;
  other.entry = {"Other", "Rest of the parameters."};
  other.floats.emplace_back(
      Configuration::BoundedEntry<float>{{"Height", "Height of the pyramid."}, &height, 1, 4});
  other.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Tip Z", "Set the z coordinate of the tip."}, &tipZ, -2, 2});
  other.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Tip X", "Set the y coordinate of the tip."}, &tipX, -2, 2});

  config.insertToConfigGroups("General", group);
  config.insertToConfigGroups("General", other);
}

std::shared_ptr<Mesh> PyramidGenerator::generate() {
  int vertexCount = baseSubdivisions + 1;
  int faceCount = baseSubdivisions * 2 - 2;
  Eigen::Matrix<float, -1, 3> vertices;
  vertices.resize(vertexCount, 3);
  Eigen::Matrix<int, -1, 3> faces;
  faces.resize(faceCount, 3);

  vertices.row(0) << 0, baseHeight, 0;

  rng.seed(seed);

  int n = baseSubdivisions - 1;
  for (int i = 0; i < n; i++) {
    float ratio = i / (float)n;

    float r = ratio * M_PI * 2;
    float x = std::cos(r) * baseRadius;
    float z = std::sin(r) * baseRadius;

    unsigned int randomInt1 = rng();
    unsigned int randomInt2 = rng();
    float random1 = (float)randomInt1 / rng.max();
    float random2 = (float)randomInt2 / rng.max();
    random1 = (random1 - 0.5) * 2.0;
    random2 = (random2 - 0.5) * 2.0;

    vertices.row(i + 1) << x + random1 * baseWiggleAmount, baseHeight,
        z + random2 * baseWiggleAmount;
  }

  vertices.row(baseSubdivisions) << tipX, height, tipZ;

  for (int i = 0; i < n; i++) {
    int i1 = (i + 1) % n == 0 ? i + 1 : (i + 1) % n;
    int i2 = (i + 2) % n == 0 ? i + 2 : (i + 2) % n;

    faces.row(i) << 0, i1, i2;
    faces.row(n + i) << i1, baseSubdivisions, i2;
  }

  auto result = std::make_shared<Mesh>();
  result->vertices = vertices.cast<double>();
  result->faces = faces;
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}

PipelineStageInfo& PyramidGenerator::getInfo() { return info; }

}  // namespace procrock
