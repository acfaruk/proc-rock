#pragma once
#include <Eigen/Core>

namespace procrock {
struct Mesh {
  Eigen::MatrixXd vertices;
  Eigen::MatrixXd normals;
  Eigen::MatrixXi faces;
};
}  // namespace procrock