#include "internal_mesh.h"

#include <OpenMesh/Core/IO/exporter/ExporterT.hh>

namespace procrock {
namespace internalmesh {
std::unique_ptr<Mesh> convert(InternalMesh& in) {
  auto exporter = OpenMesh::IO::ExporterT<InternalMesh>(in);

  auto result = std::make_unique<Mesh>();
  for (auto it = in.vertices_begin(); it != in.vertices_end(); ++it) {
    auto point = exporter.point(*it);
    result->positions.emplace_back(point.data()[0], point.data()[1], point.data()[2]);
  }

  for (auto it = in.faces_begin(); it != in.faces_end(); ++it) {
    std::vector<OpenMesh::VertexHandle> indexes;
    indexes.reserve(3);
    exporter.get_vhandles(*it, indexes);
    result->faces.emplace_back(indexes[0].idx(), indexes[1].idx(), indexes[2].idx());
  }

  return result;
}
}  // namespace internalmesh
}  // namespace procrock