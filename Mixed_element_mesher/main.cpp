#include "mixed_mesher2d.h"
#include "mesh2Dto3D.h"
int main() {
	
	long N_pad = 5;
	long Nx = 52 + 2 * N_pad;
	long Ny = 292 + 2 * N_pad;
	double delta = 50 * 1e-6;
	vector<double> x_p_list;
	for (long i = 0; i <= Nx; i++) {
		x_p_list.push_back(i * delta);
	}
	vector<double> y_p_list;
	for (long i = 0; i <= Ny; i++) {
		y_p_list.push_back(i * delta);
	}
	Mixed_mesher_2d::V2d shift(-0.0056 - N_pad * delta, -0.0146 - N_pad * delta);
	Mixed_mesher_2d::Grid2d grid(x_p_list, y_p_list, shift);

	Mixed_mesher_2d::Polygon mypolygon3("poly3.polyrecord");
	Mixed_mesher_2d::Polygon mypolygon4("poly4.polyrecord");
	Mixed_mesher_2d::Polygon mypolygon5("poly5.polyrecord");
	
	grid.register_polygon(mypolygon3);
	grid.register_polygon(mypolygon4);
	grid.register_polygon(mypolygon5);

	grid.summarize();
	grid.output_wrapping_polygons("gridpoly");
	//grid.output_background_grid("background.2dmesh");
	
	//mesh2Dto3D::Mesh_2d mesh2d("poly5.1.node", "poly5.1.ele");
	//mesh2d.output_mesh("poly5.2dmesh");
	system("pause");
}