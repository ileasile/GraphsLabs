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
		if (!is_oriented)
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
			if (e.a != -1 && e.b != -1)
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

	vector<vector<int>> get_layers() {
		topological_sort();
		vector<int> layer(N(), 0);
		int c = 0;
		for (size_t j = 1; j < N(); ++j) {
			auto ind = vert_index[num[j]];
			for (auto v : a_list[ind]) {
				layer[v] = max(layer[v], layer[ind] + 1);
				if (c < layer[v])
					c = layer[v];
			}
		}

		vector<vector<int>> res(c + 1);
		for (size_t i = 0; i < N(); ++i) {
			res[layer[i]].push_back(vert_original[i]);
		}

		return res;
	}

private:
	vector<int> * strong_comp;
	void dfs2(int v, const Graph & gt) {
		used[v] = true;
		strong_comp->push_back(vert_original[v]);
		for (auto u: gt.a_list[v])
			if (!used[u])
				dfs2(u, gt);
	}

	void dfs2_mod(int v, const Graph & gt, int c) {
		used[v] = true;
		component[v] = c;
		for (auto u : gt.a_list[v])
			if (!used[u])
				dfs2_mod(u, gt, c);
	}
public:


	Graph transpose() {
		Graph gt(vert_original, vert_index);
		for (auto & e : e_list) {
			gt.add_edge(e.b, e.a);
		}
		return gt;
	}

	 auto get_strong_connected_components() {
		vector<vector<int>> comp;
		auto gt = transpose();
		topological_sort();
		used.assign(N(), false);
		for (auto i : num) {
			int v = vert_index[i];
			if (!used[v]) {
				comp.push_back({});
				strong_comp = &comp.back();
				dfs2(v, gt);
			}
		}

		return comp;
	}

	 auto get_condensation() {
		 component.resize(N());
		 auto gt = transpose();
		 topological_sort();
		 used.assign(N(), false);
		 int c = 0;
		 for (auto i : num) {
			 int v = vert_index[i];
			 if (!used[v]) {
				 dfs2_mod(v, gt, c++);
			 }
		 }

		 Graph condensation(c);
		 for (auto & e : e_list) {
			 auto ca = component[e.a], cb = component[e.b];
			 if (ca != cb && !condensation.a_mtx[ca][cb]) {
				 condensation.add_edge(ca, cb);
			 }
		 }

		 return condensation;
	 }
};

int main() {
	ofstream f("report.txt");
	
	f << "TASK 1\n";
	Graph ag("input2.dat");
	//ag.print_adjacency_matrix(f);
	auto vert_num = ag.get_layers();
	f << "Layer form of topological sort:\n";
	for (auto & v : vert_num) {
		for (auto & x : v) {
			f << x << " ";
		}
		f << "\n";
	}

	f << "\nTASK 2\n";
	Graph g("input3.dat");
	//g.print_adjacency_matrix(f);
	auto sconn = g.get_strong_connected_components();
	f << "Strong connected components:\n";
	for (int i = 0; i < sconn.size(); ++i) {
		f << i + 1 << ": ";
		for (auto & x : sconn[i]) {
			f << x << " ";
		}
		f << "\n";
	}

	f << "\nTASK 3\n";
	auto cond = g.get_condensation();
	f << "Condensation:\n";
	cond.print_adjacency_list(f);

	f.close();
	return 0;
}