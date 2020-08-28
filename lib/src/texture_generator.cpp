#include "texture_generator.h"

#include <igl/barycentric_coordinates.h>

namespace procrock {
std::shared_ptr<Mesh> TextureGenerator::run(Mesh* before) {
  if (isChanged() || firstRun) {
    mesh = generate(before);
  }

  if (firstRun) firstRun = !firstRun;
  return mesh;
}

// Texture Generators can be neither moved nor removed from the pipeline
bool TextureGenerator::isMoveable() const { return false; }
bool TextureGenerator::isRemovable() const { return false; }

void TextureGenerator::fillTextureFaceBased(
    const Mesh& mesh, Texture& tex, std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction) {
  tex.data.resize(tex.data.capacity());
  std::fill(tex.data.begin(), tex.data.end(), 255);

  // Loop through faces
  for (int i = 0; i < mesh.faces.rows(); i++) {
    auto face = mesh.faces.row(i);

    // Uvs of the current face
    Eigen::Vector2d uvs[3] = {mesh.uvs.row((face(0))), mesh.uvs.row((face(1))),
                              mesh.uvs.row((face(2)))};

    // Positions of the current face
    Eigen::Vector3d pos[3] = {mesh.vertices.row((face(0))), mesh.vertices.row((face(1))),
                              mesh.vertices.row((face(2)))};

    // Find boundary box of uv triangle
    double minU = std::min({uvs[0](0), uvs[1](0), uvs[2](0)});
    double minV = std::min({uvs[0](1), uvs[1](1), uvs[2](1)});

    double maxU = std::max({uvs[0](0), uvs[1](0), uvs[2](0)});
    double maxV = std::max({uvs[0](1), uvs[1](1), uvs[2](1)});

    // This value represents one pixel in uv space
    double pixelStep = 1.0 / tex.width;
    double pixelStepHalf = pixelStep / 2;

    int boundaryWidth = std::ceil((maxU - minU) / pixelStep);
    int boundaryHeight = std::ceil((maxV - minV) / pixelStep);

    int matrixCols = boundaryWidth * boundaryHeight * 9;
    Eigen::MatrixXd queries(matrixCols, 2);

    Eigen::MatrixXd corner1(matrixCols, 2);
    Eigen::MatrixXd corner2(matrixCols, 2);
    Eigen::MatrixXd corner3(matrixCols, 2);

    // Create queries for barycentriq coordinates
    for (int x = 0; x < boundaryWidth; x++) {
      for (int y = 0; y < boundaryHeight; y++) {
        for (int subX = 0; subX < 3; subX++) {
          for (int subY = 0; subY < 3; subY++) {
            double u = minU + (x * pixelStep) + subX * pixelStepHalf;
            double v = minV + (y * pixelStep) + subY * pixelStepHalf;

            int index = 9 * (x + boundaryWidth * y) + subX + 3 * subY;
            queries.row(index) << u, v;
            corner1.row(index) = uvs[0].transpose();
            corner2.row(index) = uvs[1].transpose();
            corner3.row(index) = uvs[2].transpose();
          }
        }
      }
    }

    Eigen::MatrixXd baryCoords;
    igl::barycentric_coordinates(queries, corner1, corner2, corner3, baryCoords);

    // Iterate through the texture part and fill the pixels
    int textureStartX = minU / pixelStep;
    int textureStartY = minV / pixelStep;

    for (int x = 0; x < boundaryWidth; x++) {
      for (int y = 0; y < boundaryHeight; y++) {
        int textureX = textureStartX + x;
        int textureY = textureStartY + y;

        int acc = 0;
        Eigen::Vector3i accColor{0, 0, 0};
        bool inside = false;
        for (int n = 0; n < 9; n++) {
          auto lamda = baryCoords.row(9 * (x + boundaryWidth * y) + n);
          if (lamda(0) < 0 || lamda(1) < 0 || lamda(2) < 0) continue;  // pixel outside of triangle

          inside = true;

          // Calculate coordinates in mesh / world space
          Eigen::Vector3d worldPos;
          worldPos(0) = lamda(0) * pos[0](0) + lamda(1) * pos[1](0) + lamda(2) * pos[2](0);
          worldPos(1) = lamda(0) * pos[0](1) + lamda(1) * pos[1](1) + lamda(2) * pos[2](1);
          worldPos(2) = lamda(0) * pos[0](2) + lamda(1) * pos[1](2) + lamda(2) * pos[2](2);

          acc++;
          accColor += colorFunction(worldPos);
        }
        int index = 3 * (textureX + tex.width * textureY);
        if (inside) {
          Eigen::Vector3i color = accColor / acc;
          tex.data[index] = color(0);
          tex.data[index + 1] = color(1);
          tex.data[index + 2] = color(2);
        }
      }
    }
  }
}

void TextureGenerator::fillTexturePixelBased(
    const Mesh& mesh, Texture& tex, std::function<Eigen::Vector3i(Eigen::Vector3d)> colorFunction) {
  tex.data.resize(tex.data.capacity());
  std::fill(tex.data.begin(), tex.data.end(), 255);

  double pixelWidth = 1.0 / tex.width;
  double pixelWidthHalf = pixelWidth / 2.0;

  Eigen::MatrixXd uvTriangleCorner[3];

  for (int i = 0; i < 3; i++) {
    uvTriangleCorner[i].resize(mesh.faces.rows(), 2);
  }

  for (int i = 0; i < mesh.faces.rows(); i++) {
    auto face = mesh.faces.row(i);
    uvTriangleCorner[0].row(i) = mesh.uvs.row((face(0)));
    uvTriangleCorner[1].row(i) = mesh.uvs.row((face(1)));
    uvTriangleCorner[2].row(i) = mesh.uvs.row((face(2)));
  }

  for (int x = 0; x < tex.width; x++) {
    for (int y = 0; y < tex.height; y++) {
      std::vector<Eigen::Vector3i> colors;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          double u = (pixelWidth * x) + (i * pixelWidthHalf);
          double v = (pixelWidth * y) + (j * pixelWidthHalf);

          Eigen::MatrixXd queries(mesh.faces.rows(), 2);
          queries.rowwise() = Eigen::Vector2d(u, v).transpose();

          Eigen::MatrixXd baryCoords;
          igl::barycentric_coordinates(queries, uvTriangleCorner[0], uvTriangleCorner[1],
                                       uvTriangleCorner[2], baryCoords);

          for (int n = 0; n < baryCoords.rows(); n++) {
            auto lamda = baryCoords.row(n);
            if (lamda(0) < 0 || lamda(1) < 0 || lamda(2) < 0) continue;

            auto face = mesh.faces.row(n);
            Eigen::Vector3d pos[3] = {mesh.vertices.row((face(0))), mesh.vertices.row((face(1))),
                                      mesh.vertices.row((face(2)))};

            Eigen::Vector3d worldPos;
            worldPos(0) = lamda(0) * pos[0](0) + lamda(1) * pos[1](0) + lamda(2) * pos[2](0);
            worldPos(1) = lamda(0) * pos[0](1) + lamda(1) * pos[1](1) + lamda(2) * pos[2](1);
            worldPos(2) = lamda(0) * pos[0](2) + lamda(1) * pos[1](2) + lamda(2) * pos[2](2);
            colors.push_back(colorFunction(worldPos));
            break;
          }
        }
      }
      int index = 3 * (x + tex.width * y);
      Eigen::Vector3i color{0, 0, 0};
      if (colors.size() != 0) {
        for (auto n : colors) color += n;
        color /= colors.size();
      }
      tex.data[index] = color(0);
      tex.data[index + 1] = color(1);
      tex.data[index + 2] = color(2);
    }
  }
}
}  // namespace procrock