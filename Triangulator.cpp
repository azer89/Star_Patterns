
#include "Triangulator.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;

std::vector<ALine> Triangulator::GetTriangles(std::vector<std::pair<ALine, ALine> > shape, AVector seedPt)
{
    std::vector<ALine> triangles;

    CDT cdt;
    std::list<Point> list_of_seeds;
    for(int a = 0; a < shape.size(); a++)
    {
        ALine line1 = shape[a].first;
        ALine line2 = shape[a].second;

        Vertex_handle v1 = cdt.insert(Point(line1.GetPointA().x, line1.GetPointA().y));
        Vertex_handle v2 = cdt.insert(Point(line1.GetPointB().x, line1.GetPointB().y));
        Vertex_handle v3 = cdt.insert(Point(line2.GetPointA().x, line2.GetPointA().y));
        //Vertex_handle v4 = cdt.insert(Point(line2.GetPointB().x, line2.GetPointB().y));

        // to do: check intersection
        cdt.insert_constraint(v1, v2);
        cdt.insert_constraint(v3, v2);

        //AVector midPt = line1.GetPointA() + (line2.GetPointA() - line1.GetPointA()) * 0.5f;
        //list_of_seeds.push_back(Point(midPt.x, midPt.y));
    }

    list_of_seeds.push_back(Point(seedPt.x, seedPt.y));
    //list_of_seeds.push_back(Point(0, 0));
    CGAL::refine_Delaunay_mesh_2(cdt, list_of_seeds.begin(), list_of_seeds.end(),
                                   Criteria());

    for(CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit)
    {
        // to do: something wrong with is_in_domain
        if(fit->is_in_domain())
        {
            AVector v1(fit->vertex(0)->point().x(), fit->vertex(0)->point().y());
            AVector v2(fit->vertex(1)->point().x(), fit->vertex(1)->point().y());
            AVector v3(fit->vertex(2)->point().x(), fit->vertex(2)->point().y());

            triangles.push_back(ALine(v1, v2));
            triangles.push_back(ALine(v2, v3));
            triangles.push_back(ALine(v3, v1));
        }
    }
    //std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;

    //std::cout << "Number of vertices: " << triangles.size() << std::endl;
    return triangles;
}
