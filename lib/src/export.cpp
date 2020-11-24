#include "export.h"

#include <igl/writeOBJ.h>
#include <stb_image_write.h>

namespace procrock {
void exportMesh(Mesh& mesh, const std::string filepath, bool albedo, bool normals, bool roughness,
                bool metal, bool displace, bool ambientOcc) {
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

  if (albedo) {
    std::string albedoFile = base + "albedo.png";
    stbi_write_png(albedoFile.c_str(), mesh.textures.width, mesh.textures.height, 3,
                   mesh.textures.albedoData.data(), 0);
  }

  if (normals) {
    std::string normalFile = base + "normal.png";
    stbi_write_png(normalFile.c_str(), mesh.textures.width, mesh.textures.height, 3,
                   mesh.textures.normalData.data(), 0);
  }

  if (metal) {
    std::string metalFile = base + "metal.png";
    stbi_write_png(metalFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                   mesh.textures.metalData.data(), 0);
  }

  if (roughness) {
    std::string roughnessFile = base + "roughness.png";
    stbi_write_png(roughnessFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                   mesh.textures.roughnessData.data(), 0);
  }

  if (ambientOcc) {
    std::string ambientOccFile = base + "ambientOcc.png";
    stbi_write_png(ambientOccFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                   mesh.textures.ambientOccData.data(), 0);
  }

  if (displace) {
    std::vector<unsigned char> displacementExport;
    displacementExport.resize(mesh.textures.displacementData.size());
    for (int i = 0; i < mesh.textures.displacementData.size(); i++) {
      displacementExport[i] = mesh.textures.displacementData[i] * 255;
    }
    std::string displacementFile = base + "displacement.png";
    stbi_write_png(displacementFile.c_str(), mesh.textures.width, mesh.textures.height, 1,
                   displacementExport.data(), 0);
  }
}
}  // namespace procrock