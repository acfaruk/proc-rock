#include "export.h"

#include <igl/writeOBJ.h>

#include "utils/third_party/stb_image_write.h"

namespace procrock {
void exportMesh(Mesh& mesh, const std::string filepath) {
  igl::writeOBJ(filepath, mesh.vertices, mesh.faces, mesh.normals, mesh.faces, mesh.uvs,
                mesh.faces);

  std::string base = filepath;

  // Remove extension if present.
  const size_t period_idx = base.rfind('.');
  if (std::string::npos != period_idx) {
    base.erase(period_idx);
  }

  base = base + "-tex-";

  stbi_flip_vertically_on_write(true);

  std::string albedoFile = base + "albedo.png";
  stbi_write_png(albedoFile.c_str(), mesh.textures.width, mesh.textures.height, 3,
                 mesh.textures.albedoData.data(), 0);

  std::string normalFile = base + "normal.png";
  stbi_write_png(normalFile.c_str(), mesh.textures.width, mesh.textures.height, 3,
                 mesh.textures.normalData.data(), 0);

  std::string metalFile = base + "metal.png";
  stbi_write_png(metalFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                 mesh.textures.metalData.data(), 0);

  std::string roughnessFile = base + "roughness.png";
  stbi_write_png(roughnessFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                 mesh.textures.roughnessData.data(), 0);

  std::string ambientOccFile = base + "ambientOcc.png";
  stbi_write_png(ambientOccFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                 mesh.textures.ambientOccData.data(), 0);
}
}  // namespace procrock