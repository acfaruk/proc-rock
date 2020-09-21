#include "mod/displace_along_normals_modifier.h"

#include <igl/bfs_orient.h>
#include <igl/per_vertex_normals.h>

#include <Eigen/Geometry>

namespace procrock {
std::shared_ptr<Mesh> DisplaceAlongNormalsModifier::modify(Mesh& mesh) {
  std::shared_ptr<Mesh> result = std::make_shared<Mesh>();
  vertexCount = mesh.vertices.rows();
  result->vertices = mesh.vertices;
  result->faces = mesh.faces;

  rng.seed(seed);
  auto module = evaluateGraph(noiseGraph);

  auto verticesToModify = pickSet(vertexCount, vertexCount - ignoredVerticesCount);

  for (int v : verticesToModify) {
    auto pos = mesh.vertices.row(v);
    float amount = 0;
    switch (selection) {
      case 0:
        amount = rng() / (float)rng.max();
        break;
      case 1:
        if (module != nullptr) {
          amount = (module->GetValue(pos(0), pos(1), pos(2)) + 1) / 2.0;
        } else {
          amount = 0;
        }
        break;
      default:
        assert(0 && "Handle all cases!");
        break;
    }
    amount = std::min(1.0f, std::max(0.0f, amount));

    float preferAmount = 1;
    if (preferDirection) {
      Eigen::Vector3f first = mesh.normals.row(v).normalized().cast<float>();
      Eigen::Vector3f second = preferredDirection.normalized();
      Eigen::Vector3f distance = first - second;
      float norm = distance.norm();
      preferAmount = norm * preferStrength;
    }
    auto translation = Eigen::Translation<double, 3>(
        (factor * amount * preferAmount * mesh.normals.row(v).normalized()));
    auto transformedPos = translation * Eigen::Vector3d(mesh.vertices.row(v));
    result->vertices.row(v) = transformedPos;
  }
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
Configuration DisplaceAlongNormalsModifier::getConfiguration() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set various parameters of the displacement."};
  group.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Type", "Choose how the displacement should be calculated."},
      {{"RNG", "Simple random number generator."}, {"Noise", "Based on a noise graph."}},
      &selection});

  group.floats.emplace_back(
      Configuration::BoundedEntry<float>{{"Factor", "Amount of displacement"}, &factor, -1, 1});

  group.bools.emplace_back(Configuration::SimpleEntry<bool>{
      {"Prefer Direction",
       "Enable displacement values preferring a specific direction and therefore displacing more "
       "in that direction."},
      &preferDirection});

  if (selection == 0) {
    group.ints.emplace_back(
        Configuration::BoundedEntry<int>{{"Seed", "Seed for the rng"}, &seed, 0, 100000});
  }

  group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Ignored Verts", "How many vertices should NOT be modified?"},
      &ignoredVerticesCount,
      0,
      vertexCount - 1});

  Configuration result;
  result.insertToConfigGroups("General", group);

  if (preferDirection) {
    Configuration::ConfigurationGroup preferGroup;
    preferGroup.entry = {"General Settings",
                         "Displacement values preferring a specific direction."};
    preferGroup.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
        {"Direction", "Direction to prefer."}, &preferredDirection, {-1, -1, -1}, {1, 1, 1}});
    preferGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
        {"Strength", "How strong the direction is preferred."}, &preferStrength, 1.0f, 10.0f});
    result.insertToConfigGroups("Prefer Direction", preferGroup);
  }

  switch (selection) {
    case 1:
      noiseGraph.addOwnGroups(result, "Noise");
      break;
    default:
      assert(0 && "Handle all cases!");
      break;
  }
  return result;
}
PipelineStageInfo& DisplaceAlongNormalsModifier::getInfo() { return info; }

std::set<int> DisplaceAlongNormalsModifier::pickSet(int N, int k) {
  std::set<int> elems;
  for (int r = N - k; r < N; ++r) {
    int v = std::uniform_int_distribution<>(0, r)(rng);
    if (!elems.insert(v).second) {
      elems.insert(r);
    }
  }
  return elems;
}
}  // namespace procrock