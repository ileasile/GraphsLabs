#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <limits>

using namespace std;
const int INF = numeric_limits<int>::max();

struct Edge {
	int a, b, w;
	bool operator< (const Edge & e) const {
		return w > e.w;
	}
	friend ostream & operator<< (ostream & s, Edge & e) {
		return s << e.a << " " << e.b << " ";
	}
};

struct ToEdge {
	int b, w;
};

class Graph {
public:
	typedef vector<Edge> EdgesList;
	typedef vector<vector<ToEdge>> AdjList;
	typedef vector<vector<int>> AdjMatrix;

	EdgesList e_list;
	AdjList a_list;
	AdjMatrix a_mtx;

public:
	size_t N() {
		return vert_original.size();
	}

	void add_edge(const Edge & e) {
		e_list.push_back(e);
		a_list[e.a].push_back({ e.b, e.w });
		a_mtx[e.a][e.b] = e.w;
	}

	void add_edge(int x, int y, int w) {
		add_edge({ x, y, w });
	}

	void init(int N, bool make_vert = true) {
		if (make_vert) {
			vert_index.resize(N + 1);
			vert_original.resize(N);
			for (int i = 0; i < N; ++i) {
				vert_index[i + 1] = i;
				vert_original[i] = i + 1;
			}
		}
		a_list.resize(N);
		a_mtx.resize(N, vector<int>(N, INF));
	}

	Graph() {

	}

	Graph(int N) {
		init(N);
	}

	Graph(const vector<int> & vert_original, const vector<int> & vert_index) {
		this->vert_index = vert_index;
		this->vert_original = vert_original;
		init(N(), false);
	}

	Graph(string filename, const vector<int> & vert) {
		//constructing edges list
		ifstream f(filename);
		int N;
		f >> N;
		init(vert.size(), false);
		vert_original = vert;
		vert_index.resize(N + 1, -1);
		for (size_t i = 0; i < vert.size(); ++i) {
			vert_index[vert[i]] = i;
		}

		while (1) {
			Edge e;
			f >> e.a;
			if (!f)
				break;
			f >> e.b >> e.w;
			e.a = vert_index[e.a];
			e.b = vert_index[e.b];
			if (e.a != -1 && e.b != -1)
				add_edge(e);
		}
		f.close();
	}

	Graph(string filename) {
		//constructing edges list
		ifstream f(filename);
		int N;
		f >> N;
		init(N);

		while (1) {
			Edge e;
			f >> e.a;
			if (!f)
				break;
			f >> e.b >> e.w;
			e.a = vert_index[e.a];
			e.b = vert_index[e.b];
			add_edge(e);
		}
		f.close();
	}

	void print_adjacency_matrix(ostream & s) {
		s << "Adjacency matrix:\n";
		for (auto & v : a_mtx) {
			for (auto & el : v)
				s << el << " ";
			s << "\n";
		}
		s << "\n";
	}
	void print_adjacency_list(ostream & s, bool skip_isolated = false) {
		s << "Adjacency list:\n";
		for (size_t i = 0; i < N(); ++i) {
			if (skip_isolated && a_list[i].empty())
				continue;
			s << vert_original[i] << ": ";
			for (auto & el : a_list[i])
				s << vert_original[el.b] << "(" << el.w << ") ";
			s << "\n";
		}
		s << "\n";
	}

private:
	

public:


};

int main() {
	ofstream f("report.txt");
	Graph g("input.dat");
	f << "The graph:\n";
	g.print_adjacency_list(f);
	//g.print_adjacency_matrix(f);


	f.close();
	return 0;
}