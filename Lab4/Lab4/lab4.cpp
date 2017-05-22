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
	bool operator< (const Edge & e) const{
		return w > e.w;
	}
	friend ostream & operator<< (ostream & s, Edge & e) {
		return s << e.a << " " << e.b << " ";
	}
};

struct ToEdge {
	int b, w;
};

class DSU {
	vector<int> parent;
public:
	DSU(int n) {
		parent.resize(n);
		for(int v = 0; v < n; ++v)
			parent[v] = v;
	}

	int find_set(int v) {
		if (v == parent[v])
			return v;
		return parent[v] = find_set(parent[v]);
	}

	void union_sets(int a, int b) {
		a = find_set(a);
		b = find_set(b);
		if (a != b)
			parent[b] = a;
	}
};

class Graph {
public:
	typedef vector<Edge> EdgesList;
	typedef vector<vector<ToEdge>> AdjList;
	typedef vector<vector<int>> AdjMatrix;

	vector<int> vert_original;
	vector<int> vert_index;
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
		a_list[e.b].push_back({ e.a, e.w });

		a_mtx[e.b][e.a] = a_mtx[e.a][e.b] = e.w;
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

	Graph get_regular_graph() {
		Graph g(vert_original, vert_index);
		for (size_t i = 0; i < N(); ++i) {
			for (size_t j = 0; j < N(); ++j) {
				if (i == j)
					continue;
				if (a_mtx[i][j])
					g.add_edge(i, j, true);
			}
		}
		return g;
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
	vector<int> pred;
	vector<int> component;

	void comp_dfs(int x, int c) {
		if (component[x] != -1)
			return;
		component[x] = c;
		for (auto y : a_list[x]) {
			comp_dfs(y.b, c);
		}
	}

	void bfs(int x) {
		queue<int> q;
		q.push(x);
		while (!q.empty()) {
			auto v = q.front();
			q.pop();
			for (auto u : a_list[v]) {
				if (pred[u.b] == -1) {
					pred[u.b] = v;
					q.push(u.b);
				}
			}
		}
	}

	void dfs(int x) {
		for (auto y : a_list[x]) {
			if (pred[y.b] == -1) {
				pred[y.b] = x;
				dfs(y.b);
			}
		}
	}
	
public:

	vector<vector<int>> get_connectivity_components() {
		component.assign(N(), -1);

		int c = 0;
		for (size_t i = 0; i < N(); ++i) {
			if (component[i] == -1) {
				comp_dfs(i, c++);
			}
		}

		vector<vector<int>> res(c);
		for (size_t i = 0; i < N(); ++i) {
			res[component[i]].push_back(vert_original[i]);
		}
		return res;
	}

	Graph boruvkaMST() {
		vector<bool> edge_added(e_list.size());
		Graph T(vert_original, vert_index);
		while (T.e_list.size() != N() - 1) {
			vector<int> min_edge;
			T.component.assign(N(), -1);
			int c = 0;
			for (size_t i = 0; i < N(); ++i) {
				if (T.component[i] == -1) {
					T.comp_dfs(i, c++);
					min_edge.push_back(-1);
				}
			}

			for (size_t i = 0; i < e_list.size(); ++i) {
				Edge & e = e_list[i];
				auto ac = T.component[e.a];
				auto bc = T.component[e.b];
				if (ac != bc) {
					auto & mac = min_edge[ac];
					auto & mbc = min_edge[bc];
					if (mac == -1 || e_list[mac].w > e.w)
						mac = i;
					if (mbc == -1 || e_list[mbc].w > e.w)
						mbc = i;
				}
			}

			for (auto & e : min_edge) {
				if (!edge_added[e]) {
					T.add_edge(e_list[e]);
					edge_added[e] = 1;
				}
			}
		}

		return T;
	}

	Graph primMST() {
		Graph T(vert_original, vert_index);

		priority_queue<Edge> q;
		vector<bool> S(N());
		S[0] = 1;
		for (auto v : a_list[0])
			q.push({ 0, v.b, v.w });

		while (!q.empty()){
			auto e = q.top();
			q.pop();
			if (S[e.b])
				continue;
			T.add_edge(e);
			S[e.b] = 1;
			for (auto v : a_list[e.b])
				if(!S[v.b])
					q.push({ e.b, v.b, v.w });
		}

		return T;
	}

	Graph kruskalMST() {
		Graph T(vert_original, vert_index);
		DSU d(N());
		sort(e_list.begin(), e_list.end(), 
			[](Edge & a, Edge & b) {return a.w < b.w; });

		for (auto & e : e_list) {
			if (d.find_set(e.a) != d.find_set(e.b)) {
				T.add_edge(e);
				d.union_sets(e.a, e.b);
			}
		}

		return T;
	}

};

int main() {
	ofstream f("report.txt");
	Graph nog("input.dat");
	auto comps = nog.get_connectivity_components();
	auto it = max_element(comps.begin(), comps.end(),
		[](vector<int> & a, vector<int> & b) {return a.size() < b.size(); });
	Graph g("input.dat", *it);
	f << "The graph:\n";
	g.print_adjacency_list(f);
	//g.print_adjacency_matrix(f);


	f << "Boruvka MST:\n";
	auto mst = g.boruvkaMST();
	mst.print_adjacency_list(f);

	f << "Prim MST:\n";
	mst = g.primMST();
	mst.print_adjacency_list(f);

	f << "Kruskal MST:\n";
	mst = g.kruskalMST();
	mst.print_adjacency_list(f);

	f.close();
	return 0;
}