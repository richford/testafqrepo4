#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Alpha_shape_3.h>

#include <fstream>
#include <list>
#include <cassert>
#include <map>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Gt;

typedef CGAL::Alpha_shape_vertex_base_3<Gt>          Vb;
typedef CGAL::Alpha_shape_cell_base_3<Gt>            Fb;
typedef CGAL::Triangulation_data_structure_3<Vb,Fb>  Tds;
typedef CGAL::Delaunay_triangulation_3<Gt,Tds>       Triangulation_3;
typedef CGAL::Alpha_shape_3<Triangulation_3>         Alpha_shape_3;

typedef Gt::Point_3                                  Point;
typedef Alpha_shape_3::Alpha_iterator                Alpha_iterator;
typedef Alpha_shape_3::Facet                         Facet;
typedef Alpha_shape_3::Cell_handle                   Cell_handle;

void replaceExt(std::string& s, const std::string& newExt) {
  std::string::size_type i = s.rfind('.', s.length());

  if (i != std::string::npos) {
    s.replace(i+1, newExt.length(), newExt);
  }
}

int main(int argc, char **argv)
{
  std::list<Point> lp;
  std::map<Point, int> pMap;

  std::string filename(argv[1]);

  //read input
  std::ifstream is(filename.c_str());
  int n;
  is >> n;
  std::cout << "Reading " << n << " points from file " << filename << std::endl;
  Point p;
  int i = 0;
  for( ; n>0 ; n--)    {
    is >> p;
    lp.push_back(p);
    pMap[p] = i;
    i++;
  }

  // compute alpha shape
  Alpha_shape_3 as(lp.begin(),lp.end());
  std::cout << "Alpha shape computed in REGULARIZED mode by default"
            << std::endl;

  // find optimal alpha value
  Alpha_iterator opt = as.find_optimal_alpha(1);
  std::cout << "Optimal alpha value to get one connected component is "
            <<  *opt    << std::endl;
  as.set_alpha(*opt);
  assert(as.number_of_solid_components() == 1);

  std::list<Facet> facetsOut;

  as.get_alpha_shape_facets(std::back_inserter(facetsOut), Alpha_shape_3::REGULAR);
  as.get_alpha_shape_facets(std::back_inserter(facetsOut), Alpha_shape_3::SINGULAR);

  std::cout << "The alpha shape getter gives " << facetsOut.size()
            << " OUT facets" << std::endl;

  replaceExt(filename, "alpha_shape.facets");
  std::ofstream os(filename.c_str());

  // Print facets to stdout
  for(std::list<Facet>::const_iterator iter = facetsOut.begin(); iter != facetsOut.end(); iter++) {
    const Cell_handle& ch = iter->first;
    const int index = iter->second;

    const Point& a = ch->vertex((index+1)&3)->point();
    const Point& b = ch->vertex((index+2)&3)->point();
    const Point& c = ch->vertex((index+3)&3)->point();

    os << pMap[a] << " ";
    os << pMap[b] << " ";
    os << pMap[c] << std::endl;
  }

  return 0;
}
