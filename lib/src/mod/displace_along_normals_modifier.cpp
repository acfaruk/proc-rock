#include "mod/displace_along_normals_modifier.h"

#include <igl/bfs_orient.h>
#include <igl/per_vertex_normals.h>

#include <Eigen/Geometry>

namespace procrock {
std::shared_ptr<Mesh> DisplaceAlongNormalsModifier::modify(Mesh& mesh) {
  std::shared_ptr<Mesh> result = std::make_shared<Mesh>();
  result->vertices.resize(mesh.vertices.rows(), mesh.vertices.cols());
  result->faces = mesh.faces;

  rng.seed(seed);
  auto singleModule = singleNoiseModule.getModule();
  auto combinedModule = combinedNoiseModule.getModule();
  auto selectedModule = selectedNoiseModule.getModule();

  for (Eigen::Index i = 0; i < mesh.vertices.rows(); i++) {
    auto pos = mesh.vertices.row(i);
    float amount = 0;
    switch (selection) {
      case 0:
        amount = rng() / (float)rng.max();
        break;
      case 1:
        amount = (singleModule->GetValue(pos(0), pos(1), pos(2)) + 1) / 2.0;
        break;
      case 2:
        amount = (combinedModule->GetValue(pos(0), pos(1), pos(2)) + 1) / 2.0;
        break;
      case 3:
        amount = (selectedModule->GetValue(pos(0), pos(1), pos(2)) + 1) / 2.0;
        break;
      default:
        assert(0 && "Handle all cases!");
        break;
    }
    amount = std::min(1.0f, std::max(0.0f, amount));
    auto translation =
        Eigen::Translation<double, 3>((factor * amount * mesh.normals.row(i).normalized()));
    auto transformedPos = translation * Eigen::Vector3d(mesh.vertices.row(i));
    result->vertices.row(i) = transformedPos;
  }
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}
Configuration DisplaceAlongNormalsModifier::getConfiguration() {
  Configuration::ConfigurationGroup group;

  group.entry = {"General Settings", "Set various parameters of the displacement."};
  group.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Type", "Choose how the displacement should be calculated."},
      {{"RNG", "Simple random number generator."},
       {"Single Noise", "Based on a single noise function."},
       {"Combined Noise", "Based on two noise functions."},
       {"Selected Noise",
        "Based on two noise functions where the value is choosen by a third one."}},
      &selection});

  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Factor", "Amount of displacement"}, &factor, 0.001, 0.2});

  if (selection == 0) {
    group.ints.emplace_back(
        Configuration::BoundedEntry<int>{{"Seed", "Seed for the rng"}, &seed, 0, 100000});
  }

  Configuration result;
  result.insertToConfigGroups("General", group);

  switch (selection) {
    case 1:
      singleNoiseModule.addOwnGroups(result, "Noise");
      break;
    case 2:
      combinedNoiseModule.addOwnGroups(result, "Combined Noise");
      break;
    case 3:
      selectedNoiseModule.addOwnGroups(result, "Selected Noise");
      break;
    default:
      assert(0 && "Handle all cases!");
      break;
  }
  return result;
}
PipelineStageInfo& DisplaceAlongNormalsModifier::getInfo() { return info; }
}  // namespace procrock