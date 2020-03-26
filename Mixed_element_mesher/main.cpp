#include "mixed_mesher2d.h"
#include "mesh2Dto3D.h"
#include "Triangle_interface.h"
int main() {
	
	long N_pad = 5;
	long Nx = 13 + 2 * N_pad;
	long Ny = 73 + 2 * N_pad;
	double deltax = 200 * 1e-6;
	double deltay = 200 * 1e-6;
	vector<double> x_p_list;
	for (long i = 0; i <= Nx; i++) {
		x_p_list.push_back(i * deltax);
	}
	vector<double> y_p_list;
	for (long i = 0; i <= Ny; i++) {
		y_p_list.push_back(i * deltay);
	}
	Mixed_mesher_2d::V2d shift(-0.0056 - N_pad * deltax, -0.0146 - N_pad * deltay);
	Mixed_mesher_2d::Grid2d grid(x_p_list, y_p_list, shift);

	grid.read_polygon_inputs();

	grid.summarize();

	grid.output_wrapping_polygons();

	Mixed_mesher_2d::dotPolyFile polyfile("./outputs/polys.metapolyrecord");
	polyfile.output("./outputs/mixed_ele.poly");
	
	grid.output_background_grid("./outputs/background.2dmesh");
	
	//mesh2Dto3D::Mesh_2d mesh2d("mixed_ele.1.node", "mixed_ele.1.ele");
	//mesh2d.output_mesh("mixed_ele.2dmesh");
	system("pause");
}