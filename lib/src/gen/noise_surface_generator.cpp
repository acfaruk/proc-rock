#include "gen/noise_surface_generator.h"

#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <igl/copyleft/marching_cubes.h>
#include <igl/per_vertex_normals.h>

#include "utils/mesh.h"

namespace procrock {
NoiseSurfaceGenerator::NoiseSurfaceGenerator() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters of the cuboid."};
  group.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Resolution", "Resolution of the grid to sample the mesh."}, &resolution, 10, 500});
  group.bools.emplace_back(
      Configuration::SimpleEntry<bool>{{"Fill Holes", "Fill Holes that are created."}, &fillHoles});
  config.insertToConfigGroups("General", group);
  noiseGraph.addOwnGroups(config, "Noise");
}

std::shared_ptr<Mesh> NoiseSurfaceGenerator::generate() {
  int pointAmount = std::pow(resolution, 3);
  Eigen::MatrixXd points;
  points.resize(pointAmount, 3);

  Eigen::VectorXd values;
  values.resize(pointAmount);

  Eigen::Vector3d originPoint{-0.5, -0.5, -0.5};

  auto noise = evaluateGraph(noiseGraph);

  for (int z = 0; z < resolution; z++) {
    for (int y = 0; y < resolution; y++) {
      for (int x = 0; x < resolution; x++) {
        int index = x + y * resolution + z * resolution * resolution;
        Eigen::Vector3d point = originPoint;
        point.x() += x * (1.0 / resolution);
        point.y() += y * (1.0 / resolution);
        point.z() += z * (1.0 / resolution);

        points.row(index) = point;

        double value = noise->GetValue(point.x(), point.y(), point.z());

        values.row(index) << value;
      }
    }
  }

  auto result = std::make_shared<Mesh>();

  igl::copyleft::marching_cubes(values, points, resolution, resolution, resolution,
                                result->vertices, result->faces);

  if (fillHoles) {
    utils::CGAL_Mesh resultCGAL;
    utils::convert(*result, resultCGAL);

    typedef boost::graph_traits<utils::CGAL_Mesh>::halfedge_descriptor halfedge_descriptor;
    typedef boost::graph_traits<utils::CGAL_Mesh>::face_descriptor face_descriptor;
    typedef boost::graph_traits<utils::CGAL_Mesh>::vertex_descriptor vertex_descriptor;

    unsigned int nb_holes = 0;
    std::vector<halfedge_descriptor> border_cycles;
    CGAL::Polygon_mesh_processing::extract_boundary_cycles(resultCGAL,
                                                           std::back_inserter(border_cycles));

    for (halfedge_descriptor h : border_cycles) {
      std::vector<face_descriptor> patch_facets;
      std::vector<vertex_descriptor> patch_vertices;
      bool success = std::get<0>(CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole(
          resultCGAL, h, std::back_inserter(patch_facets), std::back_inserter(patch_vertices)));
      ++nb_holes;
    }

    utils::convert(resultCGAL, *result);
  }

  igl::per_vertex_normals(result->vertices, result->faces, result->normals);

  return result;
}

PipelineStageInfo& NoiseSurfaceGenerator::getInfo() { return info; }

}  // namespace procrock
