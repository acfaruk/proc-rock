#include "mod/displace_along_normals_modifier.h"

#include <igl/bfs_orient.h>
#include <igl/per_vertex_normals.h>

#include <Eigen/Geometry>

namespace procrock {
DisplaceAlongNormalsModifier::DisplaceAlongNormalsModifier() {
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

  group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the rng", [&]() { return selection == 0; }}, &seed, 0, 100000});

  /*group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Ignored Verts", "How many vertices should NOT be modified?"},
      &ignoredVerticesCount,
      0,
      100000});*/
  config.insertToConfigGroups("General", group);

  std::function<bool()> preferDirectionActive = [&]() { return preferDirection; };
  Configuration::ConfigurationGroup preferGroup;
  preferGroup.entry = {"General Settings", "Displacement values preferring a specific direction.",
                       preferDirectionActive};
  preferGroup.float3s.emplace_back(Configuration::BoundedEntry<Eigen::Vector3f>{
      {"Direction", "Direction to prefer."}, &preferredDirection, {-1, -1, -1}, {1, 1, 1}});
  preferGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Strength", "How strong the direction is preferred."}, &preferStrength, 1.0f, 10.0f});
  config.insertToConfigGroups("Prefer Direction", preferGroup);

  noiseGraph.addOwnGroups(config, "Noise", [&]() { return selection == 1; });
}

std::shared_ptr<Mesh> DisplaceAlongNormalsModifier::modify(Mesh& mesh) {
  std::shared_ptr<Mesh> result = std::make_shared<Mesh>();
  vertexCount = mesh.vertices.rows();
  result->vertices = mesh.vertices;
  result->faces = mesh.faces;

  rng.seed(seed);
  auto module = evaluateGraph(noiseGraph);

  //auto verticesToModify = pickSet(vertexCount, vertexCount - ignoredVerticesCount);

  for (int v = 0; v < vertexCount; v++) {
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
PipelineStageInfo& DisplaceAlongNormalsModifier::getInfo() { return info; }
}  // namespace procrock