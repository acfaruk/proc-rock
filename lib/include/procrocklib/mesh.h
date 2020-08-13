#pragma once
#include <Eigen/Core>

namespace procrock {
struct Mesh {
  Eigen::MatrixX3f vertices;
  Eigen::MatrixX3f normals;
  Eigen::MatrixX3i faces;
};
}  // namespace procrock