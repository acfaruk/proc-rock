#include "par/lscm_parameterizer.h"

#include <igl/lscm.h>

namespace procrock {
std::shared_ptr<Mesh> LSCM_Parameterizer::parameterize(Mesh* mesh) {
  auto result = std::make_shared<Mesh>();

  // Find two random positions
  Eigen::VectorXi boundary_indices(2, 1);
  boundary_indices << 0, 1;

  Eigen::MatrixXd boundaryConditions(2, 2);
  boundaryConditions << 0, 0, 1, 0;

  igl::lscm(mesh->vertices, mesh->faces, boundary_indices, boundaryConditions, result->uvs);

  result->vertices = mesh->vertices;
  result->faces = mesh->faces;
  result->normals = mesh->normals;

  return result;
}

Configuration LSCM_Parameterizer::getConfiguration() { return Configuration(); }

PipelineStageInfo& LSCM_Parameterizer::getInfo() { return info; }
}  // namespace procrock