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

	vector<int> vert_original;
	vector<int> vert_index;
	EdgesList e_list;
	AdjList a_list;
	AdjMatrix a_mtx;
	vector<int> deg_plus;
	vector<int> deg_minus;

public:
	size_t N() {
		return vert_original.size();
	}

	void add_edge(const Edge & e, bool is_oriented = true) {
		e_list.push_back(e);
		a_list[e.a].push_back(e.b);
		if(!is_oriented)
			a_list[e.b].push_back(e.a);

		++a_mtx[e.a][e.b];
		if (!is_oriented && e.a != e.b)
			++a_mtx[e.b][e.a];
		if (is_oriented) {
			++deg_plus[e.a];
			++deg_minus[e.b];
		}
	}

	void add_edge(int x, int y, bool is_oriented = true) {
		add_edge({ x, y }, is_oriented);
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
		a_mtx.resize(N, vector<int>(N));
		deg_plus.resize(N, 0);
		deg_minus.resize(N, 0);
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

	Graph(string filename, const vector<int> & vert, bool is_oriented = true) {
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
			f >> e.b;
			e.a = vert_index[e.a];
			e.b = vert_index[e.b];
			if(e.a != -1 && e.b != -1)
				add_edge(e, is_oriented);
		}
		f.close();
	}

	Graph(string filename, bool is_oriented = true) {
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
			f >> e.b;
			e.a = vert_index[e.a];
			e.b = vert_index[e.b];
			add_edge(e, is_oriented);
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
				s << vert_original[el] << " ";
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
		vector<bool> vf(vert_index.size());
		
		for (size_t i = 0; i < N(); ++i) {
			if (pred[i] >= 0) {
				vf[vert_original[i]] = vf[vert_original[pred[i]]] = 1;
			}
		}
		vector<int> vo, vi;
		vi.resize(vert_index.size());
		for (size_t i = 0; i < vf.size(); ++i) {
			if (vf[i]) {
				vi[i] = vo.size();
				vo.push_back(i);
			}
		}

		Graph tree(vo, vi);
		for (size_t i = 0; i < N(); ++i) {
			if (pred[i] >= 0) {
				tree.add_edge(vi[vert_original[pred[i]]], vi[vert_original[i]]);
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

	vector<bool> used;
	vector<int> num;

	void topo_dfs(int v) {
		used[v] = true;
		for (auto to : a_list[v]) {
			if (!used[to])
				topo_dfs(to);
		}
		num.push_back(vert_original[v]);
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

	pair<vector<vector<int>>, vector<vector<int>>> get_accessability_and_strong_conn() {
		vector<vector<int>> access(N());
		vector<vector<bool>> accessable(N(), vector<bool>(N()));
		for (size_t x = 0; x < N(); ++x) {
			pred.assign(N(), -1);
			pred[x] = -2;
			bfs(x);
			for (size_t y = 0; y < N(); ++y) {
				if (pred[y] >= 0) {
					access[x].push_back(y);
					accessable[x][y] = 1;
				}
			}
		}

		component.assign(N(), -1);
		int c = 0;
		for (size_t i = 0; i < N(); ++i) {
			if (component[i] != -1)
				continue;
			component[i] = c++;
			for (size_t j = i + 1; j < N(); ++j) {
				if (accessable[i][j] && accessable[j][i]) {
					component[j] = component[i];
				}
			}
			
		}

		vector<vector<int>> res(c);
		for (size_t i = 0; i < N(); ++i) {
			res[component[i]].push_back(vert_original[i]);
		}
		return make_pair(move(access), move(res));

	}

	Graph get_bfs_tree(int x) {
		pred.assign(N(), -1);
		pred[x] = -2;
		bfs(x);
		return get_pred_tree();
	}

	Graph get_dfs_tree(int x) {
		pred.assign(N(), -1);
		pred[x] = -2;
		dfs(x);
		return get_pred_tree();
	}

	auto topological_sort() {
		used.assign(N(), false);
		num.clear();
		for (size_t i = 0; i<N(); ++i)
			if (!used[i])
				topo_dfs(i);
		reverse(num.begin(), num.end());
		return num;
	}
};

int main() {
	Graph nog("input.dat", false);
	auto comps = nog.get_connectivity_components();
	auto it = max_element(comps.begin(), comps.end(), 
		[](vector<int> & a, vector<int> & b) {return a.size() < b.size(); });
	Graph g("input.dat", *it);

	ofstream f("report.txt");
	f << "TASK 1\n";
	g.print_adjacency_list(f);
	g.print_adjacency_matrix(f);
	f << "TASK 2\n";

	f << "Verticies degrees:\n";
	for (auto i : g.vert_original) {
		f << i << ": deg+ = " << g.deg_plus[g.vert_index[i]] << ", deg- = " << g.deg_minus[g.vert_index[i]] << "\n";
	}
	f << "\nSources:\n";
	for (auto i : g.vert_original) {
		if (g.deg_minus[g.vert_index[i]] == 0) {
			f << i << " ";
		}
	}
	f << "\nSinks:\n";
	for (auto i : g.vert_original) {
		if (g.deg_plus[g.vert_index[i]] == 0) {
			f << i << " ";
		}
	}

	auto r = g.get_accessability_and_strong_conn();
	f << "\n\nAccess lists:\n";
	auto & access_lists = r.first;
	for (auto i : g.vert_original) {
		auto ind = g.vert_index[i];
		f << i << ": ";
		for (auto j : access_lists[ind]) {
			f << g.vert_original[j] << " ";
		}
		f << "\n";
	}

	auto & comp = r.second;
	bool is_acyclic = true;
	f << "\nNumber of strong connectivity components (including trivial): " << comps.size() << "\n";
	for (size_t i = 0; i < comp.size(); ++i) {
		f << i + 1 << ": ";
		is_acyclic &= comp[i].size() == 1;
		for (auto el : comp[i])
			f << el << " ";
		f << "\n";
	}
	if (is_acyclic)
		f << "\nGraph is acyclic\n";
	else
		f << "\nGraph has cycles\n";
	
	f << "\nTASK 3\n";
	Graph ag("input2.dat");
	//ag.print_adjacency_matrix(f);
	auto vert_num = ag.topological_sort();
	f << "Topological order of verticies:\n";
	for (auto v : vert_num) {
		f << v << " ";
	}
	return 0;
}