#include "texgen/checkerboard_texture_generator.h"

#include <igl/barycentric_coordinates.h>

namespace procrock {
std::shared_ptr<Mesh> CheckerboardTextureGenerator::generate(Mesh* before) {
  auto result = std::make_shared<Mesh>(*before);

  Texture albedo;
  albedo.data.reserve(albedo.width * albedo.height * 3);

  switch (mode) {
    case 0:  // Normal UV based
    {
      unsigned int squareWidth = albedo.width / squares;
      for (int x = 0; x < albedo.width; x++) {
        for (int y = 0; y < albedo.height; y++) {
          int rx = std::floor((x / (float)squareWidth));
          int ry = std::floor((y / (float)squareWidth));
          if ((rx + ry) % 2 == 0) {
            albedo.data.emplace_back(255);
            albedo.data.emplace_back(255);
            albedo.data.emplace_back(255);
          } else {
            albedo.data.emplace_back(0);
            albedo.data.emplace_back(0);
            albedo.data.emplace_back(0);
          }
        }
      }

    } break;
    case 1:  // Global Mesh based
    {
      albedo.data.resize(albedo.data.capacity());
      std::fill(albedo.data.begin(), albedo.data.end(), 255);

      // Loop through faces
      for (int i = 0; i < before->faces.rows(); i++) {
        auto face = before->faces.row(i);

        // Uvs of the current face
        Eigen::Vector2d uvs[3] = {before->uvs.row((face(0))), before->uvs.row((face(1))),
                                  before->uvs.row((face(2)))};

        // Positions of the current face
        Eigen::Vector3d pos[3] = {before->vertices.row((face(0))), before->vertices.row((face(1))),
                                  before->vertices.row((face(2)))};

        // Find boundary box of uv triangle
        double minU = std::min({uvs[0](0), uvs[1](0), uvs[2](0)});
        double minV = std::min({uvs[0](1), uvs[1](1), uvs[2](1)});

        double maxU = std::max({uvs[0](0), uvs[1](0), uvs[2](0)});
        double maxV = std::max({uvs[0](1), uvs[1](1), uvs[2](1)});

        // This values represent subpixels in uv space
        double fullPixelWidth = (1.0 / albedo.width);
        double halfPixelWidth = fullPixelWidth / 2;
        double quarterPixelWidth = halfPixelWidth / 2;

        int boundaryWidth = (maxU - minU) / fullPixelWidth;
        int boundaryHeight = (maxV - minV) / fullPixelWidth;

        Eigen::MatrixXd queries(boundaryWidth * boundaryHeight * 4, 2);

        Eigen::MatrixXd corner1(boundaryWidth * boundaryHeight * 4, 2);
        Eigen::MatrixXd corner2(boundaryWidth * boundaryHeight * 4, 2);
        Eigen::MatrixXd corner3(boundaryWidth * boundaryHeight * 4, 2);

        // Create queries for barycentriq coordinates (4 queries per pixel for anti aliasing)
        for (int x = 0; x < boundaryWidth; x++) {
          for (int y = 0; y < boundaryHeight; y++) {
            double u1 = minU + (x * fullPixelWidth) + quarterPixelWidth;
            double v1 = minV + (y * fullPixelWidth) + quarterPixelWidth;

            double u2 = minU + (x * fullPixelWidth) + halfPixelWidth + quarterPixelWidth;
            double v2 = minV + (y * fullPixelWidth) + quarterPixelWidth;

            double u3 = minU + (x * fullPixelWidth) + quarterPixelWidth;
            double v3 = minV + (y * fullPixelWidth) + halfPixelWidth + quarterPixelWidth;

            double u4 = minU + (x * fullPixelWidth) + halfPixelWidth;
            double v4 = minV + (y * fullPixelWidth) + halfPixelWidth;

            int startI = 4 * (x + boundaryWidth * y);
            queries.row(startI) << u1, v1;
            queries.row(startI + 1) << u2, v2;
            queries.row(startI + 2) << u3, v3;
            queries.row(startI + 3) << u4, v4;

            for (int n = 0; n < 4; n++) {
              corner1.row(startI + n) = uvs[0].transpose();
              corner2.row(startI + n) = uvs[1].transpose();
              corner3.row(startI + n) = uvs[2].transpose();
            }
          }
        }

        Eigen::MatrixXd baryCoords;
        igl::barycentric_coordinates(queries, corner1, corner2, corner3, baryCoords);

        // Iterate trhough the texture part and fill the pixels
        int textureStartX = minU / fullPixelWidth;
        int textureStartY = minV / fullPixelWidth;

        for (int x = 0; x < boundaryWidth; x++) {
          for (int y = 0; y < boundaryHeight; y++) {
            int startI = 4 * (x + boundaryWidth * y);

            int color[4];
            Eigen::Vector3d meshPos[4];
            for (int n = 0; n < 4; n++) {
              auto lamda = baryCoords.row(startI + n);
              double xV = lamda(0) * pos[0](0) + lamda(1) * pos[1](0) + lamda(2) * pos[2](0);
              double yV = lamda(0) * pos[0](1) + lamda(1) * pos[1](1) + lamda(2) * pos[2](1);
              double zV = lamda(0) * pos[0](2) + lamda(1) * pos[1](2) + lamda(2) * pos[2](2);

              int xI = (xV / (1.0 / squares)) + 1000;
              int yI = (yV / (1.0 / squares)) + 1000;
              int zI = (zV / (1.0 / squares)) + 1000;
              color[n] = (zI % 2) ? ((xI + yI) % 2 ? 255 : 0) : (((xI + yI) % 2 ? 0 : 255));
            }

            int colorAvg = (color[0] + color[1] + color[3] + color[4]) / 4;
            // std::cout << x << "/" << y << "/";
            int index = 3 * ((textureStartX + x) + albedo.width * (textureStartY + y));
            // std::cout << index << std::endl;
            albedo.data[index] = colorAvg;
            albedo.data[index + 1] = colorAvg;
            albedo.data[index + 2] = colorAvg;
          }
        }
        // for (int i = 0; i < baryCoords.rows(); i++) {
        //  textureX++;
        //  if (textureX == textureStartX + boundaryWidth) {
        //    textureX = textureStartX;
        //    textureY++;
        //  }
        //  auto lamda = baryCoords.row(i);
        //  if (lamda(0) < 0 || lamda(1) < 0 || lamda(2) < 0) continue;  // pixel outside of
        //  triangle

        //  double x = lamda(0) * pos[0](0) + lamda(1) * pos[1](0) + lamda(2) * pos[2](0);
        //  double y = lamda(0) * pos[0](1) + lamda(1) * pos[1](1) + lamda(2) * pos[2](1);
        //  double z = lamda(0) * pos[0](2) + lamda(1) * pos[1](2) + lamda(2) * pos[2](2);

        //  int index = 3 * (textureX + albedo.width * textureY);

        //  int xI = (x / (1.0 / squares)) + 1000;
        //  int yI = (y / (1.0 / squares)) + 1000;
        //  int zI = (z / (1.0 / squares)) + 1000;

        //  int color = (zI % 2) ? ((xI + yI) % 2 ? 255 : 0) : (((xI + yI) % 2 ? 0 : 255));
        //  albedo.data[index] = color;
        //  albedo.data[index + 1] = color;
        //  albedo.data[index + 2] = color;
        //}
      }
    } break;
  }
  result->albedo = albedo;
  return result;
}

Configuration CheckerboardTextureGenerator::getConfiguration() {
  Configuration result;
  result.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Squares", "How many squares per dimension"}, &squares, 2, 100});

  result.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Type", "Select how the Checkerboard should be generated"},
      {{"Simple UV based", "Creates a 2D checkerboard texture."},
       {"Global Mesh based", "Projects a 3D checkerboard to the texture."}},
      &mode});

  return result;
}
PipelineStageInfo& CheckerboardTextureGenerator::getInfo() { return info; }

}  // namespace procrock
