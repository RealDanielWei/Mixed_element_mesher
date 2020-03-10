#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

namespace mesh2Dto3D {
	struct Point_2d
	{
		double x, y;
		Point_2d(double x, double y) {
			this->x = x;
			this->y = y;
		}
	};

	struct Point_3d
	{
		double x, y, z;
		Point_3d(double x, double y, double z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};

	struct Triangle
	{
		long p1, p2, p3;

		Triangle(long p1, long p2, long p3) {
			this->p1 = p1;
			this->p2 = p2;
			this->p3 = p3;
		}
	};

	struct Tetrahedron
	{
		long p1, p2, p3, p4;

		Tetrahedron(long p1, long p2, long p3, long p4) {
			this->p1 = p1;
			this->p2 = p2;
			this->p3 = p3;
			this->p4 = p4;
		}
	};


	class Mesh_2d {
	public:
		vector<Point_2d> points;
		vector<Triangle> triangles;

		Mesh_2d(string nodefile, string elefile) {
			//read nodes
			ifstream nodes(nodefile);
			string str;
			bool first_line_flag = false;
			while (getline(nodes, str)) {
				istringstream istr(str);
				string init;
				istr >> init;
				if (init == "" || !isdigit(init.at(0))) {
					continue;
				}
				else {
					if (first_line_flag == false) {
						first_line_flag = true;
						continue;
					}
					else {
						string num_str;
						double xp, yp;
						istr >> num_str;
						xp = stod(num_str);
						istr >> num_str;
						yp = stod(num_str);
						this->points.push_back(Point_2d(xp, yp));
					}
				}
			}
			nodes.close();
			//read triangles
			ifstream eles(elefile);
			first_line_flag = false;
			while (getline(eles, str)) {
				istringstream istr(str);
				string init;
				istr >> init;
				if (init == "" || !isdigit(init.at(0))) {
					continue;
				}
				else {
					if (first_line_flag == false) {
						first_line_flag = true;
						continue;
					}
					else {
						string num_str;
						long p1, p2, p3;
						istr >> num_str;
						p1 = stol(num_str) - 1;
						istr >> num_str;
						p2 = stol(num_str) - 1;
						istr >> num_str;
						p3 = stol(num_str) - 1;
						this->triangles.push_back(Triangle(p1, p2, p3));
					}
				}
			}
			eles.close();
		}

		void output_mesh(string filename) {
			ofstream of;
			of.open(filename);
			for (long i = 0; i < this->triangles.size(); i++) {
				long p1 = this->triangles[i].p1;
				long p2 = this->triangles[i].p2;
				long p3 = this->triangles[i].p3;
				of << this->points[p1].x << " " << this->points[p1].y << " " << this->points[p2].x << " " << this->points[p2].y << endl;
				of << this->points[p1].x << " " << this->points[p1].y << " " << this->points[p3].x << " " << this->points[p3].y << endl;
				of << this->points[p2].x << " " << this->points[p2].y << " " << this->points[p3].x << " " << this->points[p3].y << endl;
			}
			of.close();
		}
	};

	class Mesh_3d {
	public:
		vector<Point_3d> points;
		vector<Tetrahedron> tetras;

		Mesh_3d(Mesh_2d& mesh2d, double zmax, long N) {
			//set vertices
			for (long i = 0; i < mesh2d.points.size(); i++) {
				for (long j = 0; j < (N+1); j++) {
					this->points.push_back(Point_3d(mesh2d.points[i].x, mesh2d.points[i].y, (double(j) / N) * zmax));
				}
			}
			//set tetras
			for (long i = 0; i < mesh2d.triangles.size(); i++) {
				for (long j = 0; j < N; j++) {
					long p1 = (N + 1) * mesh2d.triangles[i].p1 + j;
					long p2 = (N + 1) * mesh2d.triangles[i].p2 + j;
					long p3 = (N + 1) * mesh2d.triangles[i].p3 + j;
					long p4 = (N + 1) * mesh2d.triangles[i].p1 + j + 1;
					long p5 = (N + 1) * mesh2d.triangles[i].p2 + j + 1;
					long p6 = (N + 1) * mesh2d.triangles[i].p3 + j + 1;
					this->tetras.push_back(Tetrahedron(p1, p4, p5, p6));
					this->tetras.push_back(Tetrahedron(p1, p2, p3, p5));
					this->tetras.push_back(Tetrahedron(p1, p3, p5, p6));
				}
			}
		}

		void output(string nodefile, string elefile) {
			//output nodes
			ofstream of_node;
			of_node.open(nodefile);
			for (long i = 0; i < this->points.size(); i++) {
				of_node << this->points[i].x << " " << this->points[i].y << " " << this->points[i].z << " " << i + 1 << endl;
			}
			of_node.close();
			//output eles
			ofstream of_eles;
			of_eles.open(elefile);
			for (long i = 0; i < this->tetras.size(); i++) {
				of_eles << this->tetras[i].p1 + 1 << " " << this->tetras[i].p2 + 1 << " " << this->tetras[i].p3 + 1 << " " << this->tetras[i].p4 + 1 << " " << i + 1 << endl;
			}
			of_eles.close();
		}
	};
}

