#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>

using namespace std;

struct Edge {
	int a, b;
	friend ostream & operator<< (ostream & s, Edge & e) {
		return s << e.a << " " << e.b << " ";
	}
};

class Graph {
public:
	typedef vector<Edge> EdgesList;
	typedef vector<vector<int>> AdjList;
	typedef vector<vector<int>> AdjMatrix;

	int N; // verticies number, V = {1, 2, ..., N}
	EdgesList e_list;
	AdjList a_list;
	AdjMatrix a_mtx;

public:
	void add_edge(const Edge & e) {
		e_list.push_back(e);
		a_list[e.a].push_back(e.b);
		a_list[e.b].push_back(e.a);
		++a_mtx[e.a][e.b];
		if (e.a != e.b)
			++a_mtx[e.b][e.a];
	}

	void add_edge(int x, int y) {
		add_edge({ x, y });
	}

	void add_oriented_edge(const Edge & e) {
		e_list.push_back(e);
		a_list[e.a].push_back(e.b);
		++a_mtx[e.a][e.b];
	}

	void add_oriented_edge(int x, int y) {
		add_oriented_edge({ x, y });
	}

	Graph(int N) {
		this->N = N;
		a_list.resize(N);
		a_mtx.resize(N, vector<int>(N));
	}

	Graph(string filename) {
		//constructing edges list
		ifstream f(filename);
		f >> N;

		this->N = N;
		a_list.resize(N);
		a_mtx.resize(N, vector<int>(N));

		while (1) {
			Edge e;
			f >> e.a;
			if (!f)
				break;
			f >> e.b;
			--e.a; --e.b;
			add_edge(e);
		}
		f.close();
	}
	
	Graph get_regular_graph() {
		Graph g(N);
		for (int i = 0; i < N; ++i) {
			for (int j = i + 1; j < N; ++j) {
				if (a_mtx[i][j])
					g.add_edge(i, j);
			}
		}
		return g;
	}

	auto degree(int x) {
		return a_list[x].size();
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
		for (int i = 0; i < N; ++i) {
			if (skip_isolated && a_list[i].empty())
				continue;
			s << i + 1 << ": ";
			for (auto & el : a_list[i])
				s << (el+1) << " ";
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
			comp_dfs(y, c);
		}
	}

	Graph get_pred_tree() {
		Graph tree(N);
		for (int i = 0; i < N; ++i) {
			if (pred[i] >= 0) {
				tree.add_oriented_edge(pred[i], i);
			}
		}
		return tree;
	}

	void bfs(int x) {
		queue<int> q;
		q.push(x);
		while (!q.empty()) {
			auto v = q.front();
			q.pop();
			for (auto u : a_list[v]) {
				if (pred[u] == -1) {
					pred[u] = v;
					q.push(u);
				}
			}
		}
	}

	void dfs(int x) {
		for (auto y : a_list[x]) {
			if (pred[y] == -1) {
				pred[y] = x;
				dfs(y);
			}
		}
	}
public:

	vector<vector<int>> get_connectivity_components() {
		component.assign(N, -1);

		int c = 0;
		for (int i = 0; i < N; ++i) {
			if (component[i] == -1) {
				comp_dfs(i, c++);
			}
		}

		vector<vector<int>> res(c);
		for (int i = 0; i < N; ++i) {
			res[component[i]].push_back(i);
		}
		return res;
	}

	Graph get_bfs_tree(int x) {
		pred.assign(N, -1);
		pred[x] = -2;
		bfs(x);
		return get_pred_tree();
	}

	Graph get_dfs_tree(int x) {
		pred.assign(N, -1);
		pred[x] = -2;
		dfs(x);
		return get_pred_tree();
	}
};

int main() {
	Graph g("input.dat");

	ofstream f("report.txt");
	f << "TASK 1\n";
	g.print_adjacency_list(f);
	g.print_adjacency_matrix(f);
	f << "TASK 2\n";

	f << "Verticies degrees:\n";
	for (int i = 1; i <= g.N; ++i) {
		f << i << ": " << g.degree(i - 1) << "\n";
	}
	f << "\nIsolated verticies:\n";
	for (int i = 1; i <= g.N; ++i)
		if (g.degree(i-1) == 0) 
			f << i << " ";
	f << "\n\nLeaves:\n";
	for (int i = 1; i <= g.N; ++i) {
		if (g.degree(i-1) == 1) 
			f << i << " ";
	}
	f << "\n\nHanging edges:\n";
	for (auto & e : g.e_list)
		if (g.degree(e.a) == 1 || g.degree(e.b) == 1)
			f << e << "\n";
	f << "\nLoops (with degrees):\n";
	for (int i = 0; i < g.N; ++i) {
		if (g.a_mtx[i][i])
			f << i+1 << ": " << g.a_mtx[i][i] << "\n";
	}
	f << "\nMultiple edges (with degrees):\n";
	for (int i = 0; i < g.N; ++i) {
		for (int j = i + 1; j < g.N; ++j) {
			if (g.a_mtx[i][j] > 1)
				f << i+1 << " " << j+1 << ": " << g.a_mtx[i][j] << "\n";
		}
	}

	f << "\nTASK 3 - regular graph\n";
	auto rg = g.get_regular_graph();
	rg.print_adjacency_list(f);

	f << "TASK 4\n";
	auto comps = rg.get_connectivity_components();
	f << "Number of components: " << comps.size() << "\n";
	for (size_t i = 0; i < comps.size(); ++i) {
		f << i + 1 << ": ";
		for (auto el : comps[i])
			f << el + 1 << " ";
		f << "\n";
	}

	f << "\nTASK 5\n";
	for (size_t i = 0; i < comps.size(); ++i) {
		if (comps[i].size() > 1) {
			f << "Component #" << i + 1 << "\n";
			f << "BFS tree:\n";
			auto tree = rg.get_bfs_tree(comps[i][0]);
			tree.print_adjacency_list(f, true);
			f << "DFS tree:\n";
			tree = rg.get_dfs_tree(comps[i][0]);
			tree.print_adjacency_list(f, true);
		}
	}
	return 0;
}