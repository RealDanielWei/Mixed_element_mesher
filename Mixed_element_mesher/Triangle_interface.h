#pragma once
#include "mixed_mesher2d.h"
using namespace std;

namespace Mixed_mesher_2d {

	struct Segment {
		long p1, p2;
		Segment(long p1, long p2) {
			this->p1 = p1;
			this->p2 = p2;
		}
	};

	class dotPolyFile {
	public:
		string source;
		vector<V2d> vertices;
		vector<Segment> segments;
		vector<V2d> holes;

		void add_record(string filename, int marker) {
			Polygon poly(filename);
			vector<Segment> temp_segments;
			long Np_shift = this->vertices.size();
			for (long i = 0; i < poly.points.size(); i++) {
				this->vertices.push_back(poly.points[i]);
				if (i == poly.points.size() - 1) {
					temp_segments.push_back(Segment(i + Np_shift, 0 + Np_shift));
				}
				else {
					temp_segments.push_back(Segment(i + Np_shift, i + Np_shift + 1));
				}
			}
			//check if there is overlaped segments
			double bar = 1e-15;
			vector<bool> seg_shaders = vector<bool>(temp_segments.size(), false);
			for (long i = 0; i < temp_segments.size(); i++) {
				V2d p1 = poly.points[temp_segments[i].p1 - Np_shift];
				V2d p2 = poly.points[temp_segments[i].p2 - Np_shift];
				for (long j = 0; j < temp_segments.size(); j++) {
					if (j != i) {
						V2d p3 = poly.points[temp_segments[j].p1 - Np_shift];
						V2d p4 = poly.points[temp_segments[j].p2 - Np_shift];
						bool cond1 = norm(p1 - p3) < bar && norm(p2 - p4) < bar;
						bool cond2 = norm(p1 - p4) < bar && norm(p2 - p3) < bar;
						if (cond1 || cond2) {
							seg_shaders[i] = true;
						}
					}
				}
			}
			for (long i = 0; i < temp_segments.size(); i++) {
				if (seg_shaders[i] == false) {
					this->segments.push_back(temp_segments[i]);
				}
			}
		}

		dotPolyFile(string metafilename) {
			this->source = metafilename;
			ifstream file(metafilename);
			string str;
			int marker = 1;
			while (getline(file, str)) {
				istringstream istr(str);
				string filename;
				istr >> filename;
				this->add_record(filename, marker);
				marker++;
				string remain;
				istr >> remain;
				if (remain == "") {
					continue;
				}
				else {
					double xp = stod(remain);
					istr >> remain;
					double yp = stod(remain);
					this->holes.push_back(V2d(xp, yp));
				}
			}
	
		}

		void output(string filename) {
			ofstream file(filename);
			file << "#Generated from " + this->source + " using Triangle_interface.h" << endl;
			file << "#  Declare num of vertices, dimensions, attributes, and boundary markers." << endl;
			file << this->vertices.size() << " 2 0 1" << endl;
			file << "#  List the vertices by index, x, y, and boundary marker." << endl;
			for (long i = 0; i < this->vertices.size(); i++) {
				file << (i + 1) << " " << this->vertices[i].x << " " << this->vertices[i].y << " 1"<< endl;
			}
			file << "#  Declare the number of segments and the number of boundary markers." << endl;
			file << this->segments.size() << " 1" << endl;
			file << "#  List the segments by index, start and end node, and boundary marker." << endl;
			for (long i = 0; i < this->segments.size(); i++) {
				file << (i + 1) << " " << this->segments[i].p1 + 1 << " " << this->segments[i].p2 + 1 << " 1" << endl;
			}
			file << "#  Declare the number of holes." << endl;
			file << this->holes.size() << endl;
			file << "#  Define a hole by giving the coordinates of one point inside it." << endl;
			for (long i = 0; i < this->holes.size(); i++) {
				file << (i + 1) << " " << this->holes[i].x << " " << this->holes[i].y << endl;
			}
			file.close();
		}

	};
}