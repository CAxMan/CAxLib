#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/minkowski_sum_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include <fstream>
#include <iostream>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef CGAL::Polyhedron_3<Kernel>         Polyhedron;
typedef Polyhedron::Halfedge_handle        Halfedge_handle;
typedef Polyhedron::Point_3         Point;
typedef Polyhedron::Halfedge_handle Halfedge_handle;

Polyhedron::Halfedge_handle make_cube_3(Polyhedron& P, double s)
{
	Halfedge_handle h = P.make_tetrahedron(Point(s, -s, -s), Point(-s, -s, s), Point(-s, -s, -s), Point(-s, s, -s));
	Halfedge_handle g = h->next()->opposite()->next();
	P.split_edge(h->next());
	P.split_edge(g->next());
	P.split_edge(g);                                            
	h->next()->vertex()->point() = Point(s, -s, s);
	g->next()->vertex()->point() = Point(-s, s, s);
	g->opposite()->vertex()->point() = Point(s, s, -s);            
	Halfedge_handle f = P.split_facet(g->next(), g->next()->next()->next()); 
	Halfedge_handle e = P.split_edge(f);
	e->vertex()->point() = Point(s, s, s);                     
	P.split_facet(e, f->next()->next());                      
	return h;
}

int main(int argc, char *argv[]) 
{
	Polyhedron P, E;

	if (argc < 3)
	{
		printf("\nOffsetter V0.1alpha - by Marco Attene\n------\n");
		printf("Usage: Offsetter input.off size [input2.off]\n");
		printf("  Processes 'input.off' and saves the result to 'input_offset_size.off'\n");
		printf("\n  This is an approximated offset!\n");
		printf("  The input mesh is Minkowski-summed to a cube whose side is 2*'size'\n");
		printf("  This means that the input is displaced by an amount that can vary between 'size' and sqrt(3)*'size'\n");
		printf("  If the optional 'input2.off' file is provided, it will be simply merged to the output without being offsetted.\n");
		return 10;
	}

	printf("Loading %s...\n", argv[1]);
	std::ifstream in(argv[1]);
	in >> P;
	in.close();

	double size = atof(argv[2]);
	make_cube_3(E, size);

	printf("Converting to NEF polyhedron...\n");
	Nef_polyhedron NP(P);
	Nef_polyhedron NE(E);

	printf("Performing Minkowski sum...\n");
	Nef_polyhedron result = CGAL::minkowski_sum_3(NP, NE);

	Polyhedron OP;
	printf("Converting back to standard polyhedron...\n");
	result.convert_to_Polyhedron(OP);

	if (argc > 3)
	{
		printf("Loading %s...\n", argv[3]);
		std::ifstream in(argv[3]);
		in >> OP;
		in.close();
	}

	char outfilename[2048], outfilename2[2048];
	strcpy(outfilename, argv[1]); outfilename[strlen(argv[1]) - 4] = '\0';
	sprintf(outfilename2, "%s_offset_%f.off", outfilename, size);

	std::ofstream out(outfilename2);
	out << OP;
	out.close();

	return 0;
}
