#pragma once
#include <procrocklib/texture.h>

#include <Eigen/Core>

namespace procrock {
struct Mesh {
  Eigen::MatrixXd vertices;
  Eigen::MatrixXd normals;
  Eigen::MatrixXi faces;
  Eigen::MatrixXd uvs;

  TextureGroup textures;
};
}  // namespace procrock