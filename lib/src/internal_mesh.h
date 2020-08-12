#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include "mesh.h"

namespace procrock {
typedef OpenMesh::TriMesh_ArrayKernelT<> InternalMesh;

namespace internalmesh {
std::unique_ptr<Mesh> convert(InternalMesh& in);
}  // namespace internalmesh
}  // namespace procrock