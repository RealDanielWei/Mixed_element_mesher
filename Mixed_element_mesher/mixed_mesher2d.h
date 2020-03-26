#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

namespace Mixed_mesher_2d {
	struct V2d{
		double x, y;
		V2d(){
			this->x = 0.0;
			this->y = 0.0;
		}
		V2d(double xp, double yp) {
			this->x = xp;
			this->y = yp;
		}

		friend V2d operator +(V2d V1, V2d V2) {
			return V2d(V1.x + V2.x, V1.y + V2.y);
		}
		friend V2d operator -(V2d V1, V2d V2) {
			return V2d(V1.x - V2.x, V1.y - V2.y);
		}
		friend double operator *(V2d V1, V2d V2) {
			return V1.x * V2.x + V1.y * V2.y;
		}
		friend V2d operator *(double k , V2d V) {
			return V2d(V.x * k, V.y * k);
		}
		friend double norm(V2d V) {
			return sqrt(V * V);
		}
		friend double det(V2d c1, V2d c2) {
			return c1.x * c2.y - c1.y * c2.x;
		}
		
	};

	struct SpaceIndex2d {
		long idx, idy;

		SpaceIndex2d(){
			this->idx = -1;
			this->idy = -1;
		}
		SpaceIndex2d(long nx, long ny) {
			this->idx = nx;
			this->idy = ny;
		}
	};
	

	class Polygon {
	public:
		vector<V2d> points;

		Polygon(string filename) {
			double bar = 2.5e-5;
			ifstream file(filename);
			string str;
			getline(file, str);  // ignore first line
			while (getline(file, str)) {
				istringstream istr(str);
				double xp, yp;
				istr >> xp;
				istr >> yp;
				V2d p_to_add(xp, yp);
				if (this->points.size() != 0) {
					V2d p_previous = this->points[this->points.size() - 1];
					if (norm(p_previous - p_to_add) < bar) {
						continue;
					}
				}
				this->points.push_back(V2d(xp, yp));
			}
			if (this->points.size()>0 && norm(this->points[0] - this->points[this->points.size() - 1]) < bar) {
				this->points.pop_back();
			}
		}

		Polygon(vector<V2d> points) {
			this->points = points;
		}

		V2d get_next_vertex(long n) {
			return this->points[(n + 1) % (this->points.size())];
		}

		bool check_if_two_segments_intersect(V2d p1, V2d p2, V2d p3, V2d p4) {
			V2d v1 = p2 - p1;
			V2d v2 = p3 - p4;
			V2d v3 = p3 - p1;
			double bar = 5e-16;
			double det0 = det(v1, v2);
			if (abs(det0) < bar) {
				if (abs(det(p2 - p1, p3 - p1)) < bar) {
					bool cond1 = (p3 - p1) * (p4 - p1) < bar;
					bool cond2 = (p3 - p2) * (p4 - p2) < bar;
					if (cond1 || cond2) {
						return true;
					}
				}
				return false;
			}
			double x = det(v3, v2) / det0;
			double y = det(v1, v3) / det0;
			if (x > -bar && y > -bar && x < 1.0 + bar && y < 1.0 + bar) {
				return true;
			}
			return false;
		}

		bool check_if_segment_intersect_polygon(V2d p1, V2d p2) {
			for (long i = 0; i < this->points.size(); i++) {
				if (this->check_if_two_segments_intersect(p1, p2, this->points[i], this->get_next_vertex(i))) {
					return true;
				}
			}
			return false;
		}

		bool check_if_half_line_intersect_segment(V2d p, V2d p1, V2d p2) {
			double bar = 5e-16;
			if (abs(p1.y - p2.y) < bar) {
				return false;
			}
			V2d v1 = V2d(1.0, 0.0);
			V2d v2 = p1 - p2;
			V2d v3 = p1 - p;
			double det0 = det(v1, v2);
			double x = det(v3, v2) / det0;
			double y = det(v1, v3) / det0;
			//check if cut at vertex
			if (x>-bar && (abs(y) < bar || abs(y - 1.0) < bar)) {
				if (abs(y) < bar && p1.y > p2.y) {
					return true;
				}
				if (abs(y-1.0) < bar && p1.y < p2.y) {
					return true;
				}
				return false;
			}
			//check if cut in the middle
			if (x > -bar && y > bar && y < 1.0 - bar) {
				return true;
			}
			return false;
		}

		bool check_if_point_inside_polygon(V2d point) {
			long N = 0;
			for (long i = 0; i < this->points.size(); i++) {
				if (check_if_half_line_intersect_segment(point, this->points[i], this->get_next_vertex(i))) {
					N++;
				}
			}
			if (N % 2 == 0) {
				return false;
			}
			else {
				return true;
			}
		}

		bool check_if_contains_a_polygon(const Polygon& poly) {
			V2d test_point = 0.5 * (poly.points[0] + poly.points[1]);
			return this->check_if_point_inside_polygon(test_point);
		}

		V2d get_one_inner_point() {
			V2d p0 = this->points[0];
			long i = 2;
			V2d p = 0.5 * (p0 + this->points[i]);
			while (!check_if_point_inside_polygon(p) && i < this->points.size() - 1) {
				i++;
				p = 0.5 * (p0 + this->points[i]);
			}
			if (i == this->points.size() - 1) {
				cout << "Polygon::get_one_inner_point: cannot find such a point!" << endl;
				return V2d(0.0, 0.0);
			}
			else {
				return p;
			}
		}

		void output_as_record(string filename) {
			ofstream file(filename);
			file << "0.0 0.000013" << endl;
			for (long i = 0; i < this->points.size(); i++) {
				file << this->points[i].x << " " << this->points[i].y << endl;
			}
			file.close();
		}

	};

	class Grid2d {
	public:
		V2d shift;
		long Nx, Ny;
		long Npoint, Nxedge, Nyedge, Npatch;
		vector<double> x_pposition_list;
		vector<double> y_pposition_list;
		vector<bool> point_shader_list;
		vector<bool> xedge_shader_list;
		vector<bool> yedge_shader_list;
		vector<bool> patch_shader_list;
		vector<bool> extended_patch_shader_list;
		vector<int> xedge_contour_marker;
		vector<int> yedge_contour_marker;
		vector<Polygon> registered_polygons;
		vector<Polygon> wrapping_polygons;
		vector<int> contour_polygon_sign_marker;
		vector<string> registered_polygon_file_names;

		Grid2d(vector<double> x_p_list, vector<double> y_p_list, V2d shift) {
			this->shift = shift;
			this->Nx = x_p_list.size() - 1;
			this->Ny = y_p_list.size() - 1;
			this->Npoint = (this->Nx + 1) * (this->Ny + 1);
			this->Nxedge = this->Nx * (this->Ny + 1);
			this->Nyedge = (this->Nx + 1) * this->Ny;
			this->Npatch = this->Nx * this->Ny;
			this->x_pposition_list = x_p_list;
			this->y_pposition_list = y_p_list;
			this->point_shader_list = vector<bool>(this->Npoint, false);
			this->xedge_shader_list = vector<bool>(this->Nxedge, false);
			this->yedge_shader_list = vector<bool>(this->Nyedge, false);
			this->patch_shader_list = vector<bool>(this->Npatch, false);
			this->extended_patch_shader_list = vector<bool>(this->Npatch, false);
			this->xedge_contour_marker = vector<int>(this->Nxedge, -2);
			this->yedge_contour_marker = vector<int>(this->Nyedge, -2);
		}

		SpaceIndex2d p_global_index_to_space_index(long id) {
			SpaceIndex2d sp;
			if (id < 0 || id >= this->Npoint) {
				cout << "p_global_index_to_space_index: invalid input!" << endl;
				return sp;
			}
			sp.idx = id % (this->Nx + 1);
			sp.idy = id / (this->Nx + 1);
			return sp;
		}

		long p_space_index_to_global_index(SpaceIndex2d sp) {
			if (sp.idx < 0 || sp.idx >= (this->Nx + 1) || sp.idy < 0 || sp.idy >= (this->Ny + 1)) {
				cout << "p_space_index_to_global_index: invalid input" << endl;
				return -1;
			}
			return sp.idy * (this->Nx + 1) + sp.idx;
		}

		V2d get_point(long idx, long idy) {
			return V2d(this->x_pposition_list[idx], this->y_pposition_list[idy]) + shift;
		}

		V2d get_point(SpaceIndex2d sp) {
			return this->get_point(sp.idx, sp.idy);
		}

		V2d get_point(long id) {
			return this->get_point(this->p_global_index_to_space_index(id));
		}

		SpaceIndex2d xedge_global_index_to_space_index(long id) {
			SpaceIndex2d sp;
			if (id < 0 || id >= this->Nxedge) {
				cout << "xedge_global_index_to_space_index: invalid input" << endl;
				return sp;
			}
			sp.idx = id % (this->Nx);
			sp.idy = id / (this->Nx);
			return sp;
		}

		long xedge_space_index_to_global_index(SpaceIndex2d sp) {
			if (sp.idx < 0 || sp.idx >= this->Nx || sp.idy < 0 || sp.idy >= (this->Ny + 1)) {
				cout << "xedge_space_index_to_global_index: invalid output" << endl;
				return -1;
			}
			return sp.idy * this->Nx + sp.idx;
		}

		SpaceIndex2d yedge_global_index_to_space_index(long id) {
			SpaceIndex2d sp;
			if (id < 0 || id >= this->Nyedge) {
				cout << "yedge_global_index_to_space_index: invalid input" << endl;
				return sp;
			}
			sp.idx = id % (this->Nx + 1);
			sp.idy = id / (this->Nx + 1);
			return sp;
		}

		long yedge_space_index_to_global_index(SpaceIndex2d sp) {
			if (sp.idx < 0 || sp.idx >= (this->Nx + 1) || sp.idy < 0 || sp.idy >= this->Ny) {
				cout << "yedge_space_index_to_global_index: invalid output" << endl;
				return -1;
			}
			return sp.idy * (this->Nx + 1) + sp.idx;
		}

		SpaceIndex2d patch_global_index_to_space_index(long id) {
			SpaceIndex2d sp;
			if (id < 0 || id >= this->Npatch) {
				cout << "patch_global_index_to_space_index: invalid input" << endl;
				return sp;
			}
			sp.idx = id % (this->Nx);
			sp.idy = id / (this->Nx);
			return sp;
		}

		long patch_spaceindex_to_global_index(SpaceIndex2d sp) {
			if (sp.idx < 0 || sp.idx >= (this->Nx) || sp.idy < 0 || sp.idy >= (this->Ny)) {
				cout << "patch_spaceindex_to_global_index: invalid output" << endl;
				return -1;
			}
			return sp.idy * (this->Nx) + sp.idx;
		}

		void register_polygon(string polygonfilename) {
			Polygon poly = Polygon(polygonfilename);
			if (poly.points.size() < 3) {
				return;
			}
			this->registered_polygons.push_back(poly);
			this->registered_polygon_file_names.push_back(polygonfilename);
			//set shader for points
			for (long i = 0; i < this->Npoint; i++) {
				if (poly.check_if_point_inside_polygon(this->get_point(i))) {
					this->point_shader_list[i] = true;
				}
			}
			//set shader for xedges
			for (long i = 0; i < this->Nxedge; i++) {
				SpaceIndex2d sp = this->xedge_global_index_to_space_index(i);
				V2d p1 = this->get_point(sp.idx, sp.idy);
				V2d p2 = this->get_point(sp.idx + 1, sp.idy);
				if (poly.check_if_segment_intersect_polygon(p1, p2)) {
					this->xedge_shader_list[i] = true;
				}
			}
			//set shader for yedges
			for (long i = 0; i < this->Nyedge; i++) {
				SpaceIndex2d sp = this->yedge_global_index_to_space_index(i);
				V2d p1 = this->get_point(sp.idx, sp.idy);
				V2d p2 = this->get_point(sp.idx, sp.idy + 1);
				if (poly.check_if_segment_intersect_polygon(p1, p2)) {
					this->yedge_shader_list[i] = true;
				}
			}
			//set shader for patches
			for (long i = 0; i < this->Npatch; i++) {
				SpaceIndex2d sp = this->patch_global_index_to_space_index(i);
				bool cond_p1 = point_shader_list[this->p_space_index_to_global_index(SpaceIndex2d(sp.idx, sp.idy))];
				bool cond_p2 = point_shader_list[this->p_space_index_to_global_index(SpaceIndex2d(sp.idx + 1, sp.idy))];
				bool cond_p3 = point_shader_list[this->p_space_index_to_global_index(SpaceIndex2d(sp.idx, sp.idy + 1))];
				bool cond_p4 = point_shader_list[this->p_space_index_to_global_index(SpaceIndex2d(sp.idx + 1, sp.idy + 1))];
				bool cond_xedge1 = xedge_shader_list[this->xedge_space_index_to_global_index(SpaceIndex2d(sp.idx, sp.idy))];
				bool cond_xedge2 = xedge_shader_list[this->xedge_space_index_to_global_index(SpaceIndex2d(sp.idx, sp.idy + 1))];
				bool cond_yedge1 = yedge_shader_list[this->yedge_space_index_to_global_index(SpaceIndex2d(sp.idx, sp.idy))];
				bool cond_yedge2 = yedge_shader_list[this->yedge_space_index_to_global_index(SpaceIndex2d(sp.idx + 1, sp.idy))];
				if (cond_p1 || cond_p2 || cond_p3 || cond_p4 || cond_xedge1 || cond_xedge2 || cond_yedge1 || cond_yedge2) {
				//if (cond_xedge1 || cond_xedge2 || cond_yedge1 || cond_yedge2) {
					this->patch_shader_list[i] = true;
				}
			}
			//set extended shader for patches
			for (long i = 0; i < this->Npatch; i++) {
				SpaceIndex2d sp = this->patch_global_index_to_space_index(i);
				bool flag = this->patch_shader_list[i];
				if (sp.idx != 0) {
					flag = flag || this->patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx - 1, sp.idy))];
				}
				if (sp.idx != this->Nx - 1) {
					flag = flag || this->patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx + 1, sp.idy))];
				}
				if (sp.idy != 0) {
					flag = flag || this->patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx, sp.idy - 1))];
				}
				if (sp.idy != this->Ny - 1) {
					flag = flag || this->patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx, sp.idy + 1))];
				}
				this->extended_patch_shader_list[i] = flag;
			}
			
		}

		void read_polygon_inputs() {
			ifstream file("./inputs/input_list.info");
			string str;
			while (getline(file, str)) {
				this->register_polygon("./inputs/" + str);
			}
		}

		Polygon extract_and_mark_polygon_from_one_xedge(SpaceIndex2d sp, int polyid) {
			vector<V2d> points;
			vector<V2d> dummy;
			SpaceIndex2d start_point = sp;
			SpaceIndex2d previous_point = sp;
			SpaceIndex2d current_point(sp.idx + 1, sp.idy);
			if (this->xedge_contour_marker[this->xedge_space_index_to_global_index(sp)] != -1) {
				cout << "mark_polygon_from_one_xedge: invalid input!" << endl;
				return Polygon(dummy);
			}
			this->xedge_contour_marker[this->xedge_space_index_to_global_index(sp)] = polyid;
			points.push_back(this->get_point(previous_point.idx, previous_point.idy));
			while (!(current_point.idx == start_point.idx && current_point.idy == start_point.idy)) {
				//try one step to +x
				if (this->xedge_contour_marker[this->xedge_space_index_to_global_index(SpaceIndex2d(current_point.idx, current_point.idy))] == -1) {
					if (!(previous_point.idx == current_point.idx + 1 && previous_point.idy == current_point.idy)) {
						this->xedge_contour_marker[this->xedge_space_index_to_global_index(SpaceIndex2d(current_point.idx,current_point.idy))] = polyid;
						previous_point = current_point;
						current_point.idx = current_point.idx + 1;
						points.push_back(this->get_point(previous_point.idx, previous_point.idy));
						continue;
					}
				}
				//try one step to -x
				if (this->xedge_contour_marker[this->xedge_space_index_to_global_index(SpaceIndex2d(current_point.idx - 1, current_point.idy))] == -1) {
					if (!(previous_point.idx == current_point.idx - 1 && previous_point.idy == current_point.idy)) {
						this->xedge_contour_marker[this->xedge_space_index_to_global_index(SpaceIndex2d(current_point.idx - 1, current_point.idy))] = polyid;
						previous_point = current_point;
						current_point.idx = current_point.idx - 1;
						points.push_back(this->get_point(previous_point.idx, previous_point.idy));
						continue;
					}
				}
				//try one step to +y
				if (this->yedge_contour_marker[this->yedge_space_index_to_global_index(SpaceIndex2d(current_point.idx, current_point.idy))] == -1) {
					if (!(previous_point.idx == current_point.idx && previous_point.idy == current_point.idy + 1)) {
						this->yedge_contour_marker[this->yedge_space_index_to_global_index(SpaceIndex2d(current_point.idx, current_point.idy))] = polyid;
						previous_point = current_point;
						current_point.idy = current_point.idy + 1;
						points.push_back(this->get_point(previous_point.idx, previous_point.idy));
						continue;
					}
				}
				//try one step to -y
				if (this->yedge_contour_marker[this->yedge_space_index_to_global_index(SpaceIndex2d(current_point.idx, current_point.idy - 1))] == -1) {
					if (!(previous_point.idx == current_point.idx && previous_point.idy == current_point.idy - 1)) {
						this->yedge_contour_marker[this->yedge_space_index_to_global_index(SpaceIndex2d(current_point.idx, current_point.idy - 1))] = polyid;
						previous_point = current_point;
						current_point.idy = current_point.idy - 1;
						points.push_back(this->get_point(previous_point.idx, previous_point.idy));
						continue;
					}
				}
				cout << "Cannot output contour since it's not continous" << endl;
				return Polygon(dummy);
			}
			return Polygon(points);
		}

		bool get_one_xedge_of_unmarked_polygon(long& xedgeid) {
			for (long i = 0; i < this->Nxedge; i++) {
				if (this->xedge_contour_marker[i] == -1) {
					xedgeid = i;
					return true;
				}
			}
			return false;
		}

		void summarize() {
			//set contour marker for xedges
			for (long i = 0; i < this->Nxedge; i++) {
				SpaceIndex2d sp = this->xedge_global_index_to_space_index(i);
				if (sp.idy != 0 && sp.idy != this->Ny) {
					bool upper_shader = this->extended_patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx, sp.idy))];
					bool lower_shader = this->extended_patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx, sp.idy - 1))];
					if (upper_shader != lower_shader) {
						this->xedge_contour_marker[i] = -1;
					}
				}
			}
			//set contour marker for yedges
			for (long i = 0; i < this->Nyedge; i++) {
				SpaceIndex2d sp = this->yedge_global_index_to_space_index(i);
				if (sp.idx != 0 && sp.idx != this->Nx) {
					bool left_shader = this->extended_patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx - 1, sp.idy))];
					bool right_shader = this->extended_patch_shader_list[this->patch_spaceindex_to_global_index(SpaceIndex2d(sp.idx, sp.idy))];
					if (left_shader != right_shader) {
						this->yedge_contour_marker[i] = -1;
					}
				}
			}
			long xedge_id = -1;
			while (this->get_one_xedge_of_unmarked_polygon(xedge_id)) {
				this->wrapping_polygons.push_back(this->extract_and_mark_polygon_from_one_xedge(this->xedge_global_index_to_space_index(xedge_id), this->contour_polygon_sign_marker.size()));
				this->contour_polygon_sign_marker.push_back(-1);
			}
			for (long i = 0; i < this->contour_polygon_sign_marker.size(); i++) {
				long parent_id = -1;
				for (long j = 0; j < this->contour_polygon_sign_marker.size(); j++) {
					if (j != i) {
						if (this->wrapping_polygons[j].check_if_contains_a_polygon(this->wrapping_polygons[i])) {
							parent_id = j;
							continue;
						}
					}
				}
				if (parent_id != -1) {
					this->contour_polygon_sign_marker[i] = parent_id;
				}
			}
		}

		void output_background_grid(string filename) {
			ofstream file(filename);
			for (long i = 0; i < this->Npatch; i++) {
				if (this->extended_patch_shader_list[i] == false) {
					SpaceIndex2d sp = this->patch_global_index_to_space_index(i);
					V2d p1 = this->get_point(sp.idx, sp.idy);
					V2d p2 = this->get_point(sp.idx + 1, sp.idy);
					V2d p3 = this->get_point(sp.idx, sp.idy + 1);
					V2d p4 = this->get_point(sp.idx + 1, sp.idy + 1);
					file << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;
					file << p1.x << " " << p1.y << " " << p3.x << " " << p3.y << endl;
					file << p4.x << " " << p4.y << " " << p2.x << " " << p2.y << endl;
					file << p4.x << " " << p4.y << " " << p3.x << " " << p3.y << endl;
				}
			}
			file.close();
		}

		void output_wrapping_polygons() {
			//output meta data
			ofstream metafile("./outputs/polys.metapolyrecord");
			for (long i = 0; i < this->wrapping_polygons.size(); i++) {
				metafile << "./outputs/gridpoly" + to_string(i) + ".polyrecord";
				if (this->contour_polygon_sign_marker[i] != -1) {
					V2d p_inner = this->wrapping_polygons[i].get_one_inner_point();
					metafile << " " << p_inner.x << " " << p_inner.y;
				}
				metafile << endl;
			}
			for (long i = 0; i < this->registered_polygons.size(); i++) {
				metafile << "./outputs/inputpoly" + to_string(i) + ".polyrecord";
				//V2d p_inner = this->registered_polygons[i].get_one_inner_point();
				//metafile << " " << p_inner.x << " " << p_inner.y;
				metafile << endl;
			}
			metafile.close();
			//output polygon records
			for (long i = 0; i < this->wrapping_polygons.size(); i++) {
				this->wrapping_polygons[i].output_as_record("./outputs/gridpoly" + to_string(i) + ".polyrecord");
			}
			for (long i = 0; i < this->registered_polygons.size(); i++) {
				this->registered_polygons[i].output_as_record("./outputs/inputpoly" + to_string(i) + ".polyrecord");
			}
		}

	};
}