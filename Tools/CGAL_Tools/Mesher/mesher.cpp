#include <string.h>
#include <stdlib.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/boost/graph/helpers.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>
#include <CGAL/IO/Polyhedron_iostream.h> 

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Mesh_domain;
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

#define CAXMAN_USE_TARGET_NUMTETS

double volume(const Polyhedron& tmesh)
{
		CGAL::Point_3<K> origin(0, 0, 0);
		double volume = 0.0;
		K::Compute_volume_3 cv3;

		for (Polyhedron::Facet_const_iterator f = tmesh.facets_begin(); f != tmesh.facets_end(); f++)
		{
			volume += cv3(origin, (*f).halfedge()->vertex()->point(), (*f).halfedge()->next()->vertex()->point(), (*f).halfedge()->opposite()->vertex()->point());
		}
		return volume;
}


using namespace CGAL::parameters;

void usage()
{
	printf("\nMesher V0.3alpha - by Marco Attene\n------\n");
	printf("Usage: Mesher inmeshfile edge_length max_num_tets\n");
	printf("  Reads a triangle mesh from 'inmeshfile.off' and saves a tetrahedral mesh to 'inmeshfile.mesh'\n");
	printf("  The tool also saves an ASCII file to 'inmeshfile.ael' with a single line indicating the average edge length in the tetmesh.\n");
//	printf("  An additional file '_volume_mesh_border.off' is produced for debugging purposes.\n");
	printf("  inmeshfile.mesh is a tetrahedral mesh whose vertices are sampled on the input inmeshfile.off\n");
	printf("  'edge_length' controls the edge length of the resulting tetrahedra;\n");
	printf("  'max_num_tets' bounds the complexity of the resulting mesh;\n");
	printf("\n  Example: mesher mygeometry.off 0.6 100000\n");
	printf("   would produce a mesh whose average edge length is 0.6, unless this would require more than 100000 tetrahedra.\n");
	printf("   In the latter case, the average edge length would be larger than 'edge_length' due to the bound.\n");
	printf("\nHIT ENTER TO EXIT.\n");
	getchar();
	exit(0);
}

void error(const char *msg)
{
	fprintf(stderr, msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc < 4) usage();

	const char* fname = argv[1];

	// Create input polyhedron
	Polyhedron polyhedron;
	std::ifstream input(fname);
	input >> polyhedron;

	if (input.fail()) error("Can't load file");
	input.close();

	if (!CGAL::is_triangle_mesh(polyhedron)) error("Input geometry is not triangulated.");
	if (!CGAL::is_closed(polyhedron)) error("Input geometry has unexpected boundaries.");

	double facet_angle_val = 15, cell_radius_edge_ratio_val = 2.1, edge_length_val = 0.6;
	
	int target_num_tets = 100000;
	if ((edge_length_val = atof(argv[2])) <= 0) error("edge_length out of range (must be > 0)\n");
	if ((target_num_tets = atoi(argv[3])) <= 0) error("max_num_tets out of range (must be > 0)\n");

	double mesh_vol = volume(polyhedron);
	double ntp = mesh_vol / (0.12*edge_length_val*edge_length_val*edge_length_val);

	int num_tets_using_edgelength = (ntp<INT_MAX)?(ntp):(INT_MAX);
	printf("Projected number of tetrahedra using edge_length: %d\n", num_tets_using_edgelength);
	if (num_tets_using_edgelength <= target_num_tets) target_num_tets = num_tets_using_edgelength;

	target_num_tets *= 2; // Seems to be necessary to compensate for something I cannot understand...
	double target_tet_vol = mesh_vol / target_num_tets;
	double target_circumradius = 2.039649*pow(target_tet_vol, 1.0 / 3.0);


	Mesh_criteria criteria(cell_size = target_circumradius, facet_angle = facet_angle_val, cell_radius_edge_ratio = cell_radius_edge_ratio_val);

	//// Create domain
	Mesh_domain domain(polyhedron);

	//// Mesh generation
	C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, no_perturb());
	//// Output

	double av_edge_length = 2.039649*pow(mesh_vol / c3t3.number_of_cells(), 1.0 / 3.0);
	printf("Saving mesh made of %d tetrahedra\n", c3t3.number_of_cells());
	printf("Average length of a tetrahedron edge: %f\n", av_edge_length);

	char outfilename[2048], avfilename[2048];
	strcpy(outfilename, argv[1]); outfilename[strlen(argv[1]) - 4] = '\0'; strcat(outfilename, ".mesh");
	std::ofstream medit_file(outfilename);
	c3t3.output_to_medit(medit_file);
	medit_file.close();

	strcpy(avfilename, argv[1]); avfilename[strlen(argv[1]) - 4] = '\0'; strcat(avfilename, ".ael");
	std::ofstream size_file(avfilename);
	size_file << av_edge_length << "\n";
	size_file.close();

	//std::ofstream off_file("_volume_mesh_border.off");
	//c3t3.output_boundary_to_off(off_file);
	//off_file.close();

	return 0;
}
