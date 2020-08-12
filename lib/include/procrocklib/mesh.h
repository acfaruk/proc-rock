#pragma once
#define _USE_MATH_DEFINES
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <glm/glm.hpp>

namespace procrock {

class Mesh {
 public:
  void BuildAndOutputPyramid();

 private:
  typedef OpenMesh::TriMesh_ArrayKernelT<> InternalMesh;
};
}  // namespace procrock