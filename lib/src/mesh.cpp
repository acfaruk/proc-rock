#include "mesh.h"

#include "internal_mesh.h"
namespace procrock {
void Mesh::Test() {
  InternalMesh mesh;
  InternalMesh::VertexHandle vertices[4];
  vertices[0] = mesh.add_vertex(OpenMesh::Vec3f(0, -1, -1));
  vertices[1] = mesh.add_vertex(OpenMesh::Vec3f(0.866f, -1, 0.5));
  vertices[2] = mesh.add_vertex(OpenMesh::Vec3f(-0.866f, -1, 0.5));
  vertices[3] = mesh.add_vertex(OpenMesh::Vec3f(0, 1, 0));

  std::vector<InternalMesh::VertexHandle> face_handles;
  face_handles.push_back(vertices[0]);
  face_handles.push_back(vertices[1]);
  face_handles.push_back(vertices[2]);
  mesh.add_face(face_handles);

  face_handles.clear();
  face_handles.push_back(vertices[1]);
  face_handles.push_back(vertices[0]);
  face_handles.push_back(vertices[3]);
  mesh.add_face(face_handles);

  face_handles.clear();
  face_handles.push_back(vertices[2]);
  face_handles.push_back(vertices[1]);
  face_handles.push_back(vertices[3]);
  mesh.add_face(face_handles);

  face_handles.clear();
  face_handles.push_back(vertices[0]);
  face_handles.push_back(vertices[2]);
  face_handles.push_back(vertices[3]);
  mesh.add_face(face_handles);

  auto test = internalmesh::convert(mesh);
}
}  // namespace procrock
