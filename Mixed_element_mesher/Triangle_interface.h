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
			long Np_shift = this->vertices.size();
			for (long i = 0; i < poly.points.size(); i++) {
				this->vertices.push_back(poly.points[i]);
				if (i == poly.points.size() - 1) {
					this->segments.push_back(Segment(i + Np_shift, 0 + Np_shift));
				}
				else {
					this->segments.push_back(Segment(i + Np_shift, i + Np_shift + 1));
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