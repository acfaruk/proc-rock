#include "gen/skin_surface_generator.h"

#include <CGAL/Cartesian_d.h>
#include <CGAL/make_skin_surface_mesh_3.h>
#include <CGAL/point_generators_d.h>

#include <fstream>

#include "igl/per_vertex_normals.h"
#include "igl/writeOBJ.h"
#include "utils/mesh.h"

namespace procrock {
SkinSurfaceGenerator::SkinSurfaceGenerator() {
  Configuration::ConfigurationGroup group;
  group.entry = {"General Settings", "Set various parameters."};

  group.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Distribution Method", "Select how the points should be distributed."},
      {{"RNG", "Simple random points in a geometric shape."}},
      &distributionMethod});

  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Point Size", "Size of the balls to be combined."}, &pointSize, 0.01, 0.4});
  group.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Shrink Factor", "Shrink Factor used in the CGAL package."}, &shrinkFactor, 0.01, 1.0});

  Configuration::ConfigurationGroup RNGdistrGroup;
  auto RNGdistrFunc = [&] { return distributionMethod == 0; };
  RNGdistrGroup.entry = {"RNG Distribution", "Simple random points in a geometric shape.",
                         RNGdistrFunc};

  RNGdistrGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Points", "Amount of points to generate."}, &pointAmount, 10, 100});
  RNGdistrGroup.ints.emplace_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for "}, &seed, 0, 1000000});

  config.insertToConfigGroups("General", group);
  config.insertToConfigGroups("General", RNGdistrGroup);
}

std::shared_ptr<Mesh> SkinSurfaceGenerator::generate() {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
  typedef CGAL::Skin_surface_traits_3<K> Traits;
  typedef CGAL::Skin_surface_3<Traits> Skin_surface_3;
  typedef Skin_surface_3::FT FT;
  typedef Skin_surface_3::Bare_point Bare_point;
  typedef Skin_surface_3::Weighted_point Weighted_point;
  typedef CGAL::Polyhedron_3<K, CGAL::Skin_surface_polyhedral_items_3<Skin_surface_3>> Polyhedron;
  typedef CGAL::Cartesian_d<double> Kd;
  typedef Kd::Point_d Point;

  auto result = std::make_shared<Mesh>();
  std::list<Weighted_point> points;
  CGAL::Random rng(seed);

  CGAL::Random_points_in_ball_d<Point> gen(3, 1, rng);

  for (int i = 0; i < pointAmount; i++) {
    points.push_back(Weighted_point(Bare_point(gen->x(), gen->y(), gen->z()), pointSize));
    gen++;
  }

  Polyhedron p;
  CGAL::make_skin_surface_mesh_3(p, points.begin(), points.end(), shrinkFactor);

  typedef Polyhedron::Vertex_const_iterator VCI;
  typedef Polyhedron::Facet_const_iterator FCI;
  typedef Polyhedron::Halfedge_around_facet_const_circulator HFCC;
  typedef CGAL::Inverse_index<VCI> Index;
  Index index(p.vertices_begin(), p.vertices_end());

  result->vertices.resize(p.size_of_vertices(), 3);

  int vertex = 0;
  for (auto iter = p.points_begin(); iter != p.points_end(); iter++) {
    auto point = *iter;
    result->vertices.row(vertex) << point.x(), point.y(), point.z();
    vertex++;
  }

  result->faces.resize(p.size_of_facets(), 3);
  int face = 0;
  for (auto iter = p.facets_begin(); iter != p.facets_end(); iter++) {
    HFCC hc = iter->facet_begin();
    int n = circulator_size(hc);
    assert(n == 3);
    for (int i = 0; i < 3; i++) {
      result->faces.row(face)(i) = index[VCI(hc->vertex())];
      ++hc;
    }
    face++;
  }
  igl::per_vertex_normals(result->vertices, result->faces, result->normals);
  return result;
}

PipelineStageInfo& SkinSurfaceGenerator::getInfo() { return info; }

}  // namespace procrock
