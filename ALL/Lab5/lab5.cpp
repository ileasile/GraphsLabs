#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <stack>
#include <limits>

using namespace std;
const int INF = numeric_limits<int>::max();

struct Edge {
	int a, b, w;
	bool operator< (const Edge & e) const {
		return w > e.w;
	}
	friend ostream & operator<< (ostream & s, Edge & e) {
		return s << e.a << " " << e.b << " " << e.w;
	}
};

struct ToEdge {
	int b, w;
};

struct Pr {
	int x, w;
	bool operator< (const Pr & p) const {
		return w > p.w;
	}
};

class Graph {
public:
	typedef vector<Edge> EdgesList;
	typedef vector<vector<ToEdge>> AdjList;
	typedef vector<vector<int>> AdjMatrix;

	EdgesList e_list;
	AdjList a_list;
	AdjMatrix a_mtx;
	int N;
public:
	void add_edge(const Edge & e) {
		e_list.push_back(e);
		a_list[e.a].push_back({ e.b, e.w });
		a_mtx[e.a][e.b] = e.w;
	}

	void add_edge(int x, int y, int w) {
		add_edge({ x, y, w });
	}

	void init(int N) {
		this->N = N;
		a_list.resize(N);
		a_mtx.resize(N, vector<int>(N, INF));
	}

	Graph() {

	}

	Graph(int N) {
		init(N);
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
			add_edge(e);
		}
		f.close();
	}

	void print_adjacency_matrix(ostream & s) {
		s << "Adjacency matrix:\n";
		for (auto & v : a_mtx) {
			for (auto & el : v)
				s << (el == INF ? 0 : el) << " ";
			s << "\n";
		}
		s << "\n";
	}
	void print_adjacency_list(ostream & s, bool skip_isolated = false) {
		s << "Adjacency list:\n";
		for (int i = 0; i < N; ++i) {
			if (skip_isolated && a_list[i].empty())
				continue;
			s << i << ": ";
			for (auto & el : a_list[i])
				s << el.b << "(" << el.w << ") ";
			s << "\n";
		}
		s << "\n";
	}

private:
	vector<int> d;
	vector<int> pred;

	Graph get_pred_tree(const vector<int> & pred) {
		Graph tree(N);
		for (int i = 0; i < N; ++i) {
			if (pred[i] >= 0) {
				tree.add_edge(pred[i], i, a_mtx[pred[i]][i]);
			}
		}
		return tree;
	}

public:
	
	void print_paths(ostream & s) {
		s << "Shortest paths:\n";
		for (int x = 0; x < N; x++) {
			s << d[x] << " : " << x;
			for (int y = x; pred[y] >= 0; y = pred[y])
				s << " <- " << pred[y];
			s << "\n";

		}
	}
	Graph shortest_paths_tree_stack() {
		d.assign(N, INF);
		pred.assign(N, -1);

		stack<Pr> bag;
		bag.push({ 0, 0 });
		d[0] = 0;

		while (!bag.empty()) {
			auto x = bag.top().x;
			bag.pop();
			for (auto & e : a_list[x]) {
				if (d[x] + e.w < d[e.b]) {
					d[e.b] = d[x] + e.w;
					pred[e.b] = x;
					bag.push({ e.b, d[e.b] });
				}
			}
		}

		return get_pred_tree(pred);
	}

	Graph shortest_paths_tree_queue() {
		d.assign(N, INF);
		pred.assign(N, -1);

		queue<Pr> bag;
		bag.push({ 0, 0 });
		d[0] = 0;

		while (!bag.empty()) {
			auto x = bag.front().x;
			bag.pop();
			for (auto & e : a_list[x]) {
				if (d[x] + e.w < d[e.b]) {
					d[e.b] = d[x] + e.w;
					pred[e.b] = x;
					bag.push({ e.b, d[e.b] });
				}
			}
		}

		return get_pred_tree(pred);
	}

	Graph shortest_paths_tree_priority_queue() {
		d.assign(N, INF);
		pred.assign(N, -1);

		priority_queue<Pr> bag;
		bag.push({ 0, 0 });
		d[0] = 0;

		while (!bag.empty()) {
			auto x = bag.top().x;
			bag.pop();
			for (auto & e : a_list[x]) {
				if (d[x] + e.w < d[e.b]) {
					d[e.b] = d[x] + e.w;
					pred[e.b] = x;
					bag.push({ e.b, d[e.b] });
				}
			}
		}

		return get_pred_tree(pred);
	}

};

int main() {
	ofstream f("report.txt");
	Graph g("input.dat");
	f << "The graph:\n";
	g.print_adjacency_list(f);
	//g.print_adjacency_matrix(f);

	auto tree = g.shortest_paths_tree_stack();
	f << "\nShortest paths tree, based on stack:\n";
	tree.print_adjacency_list(f);
	g.print_paths(f);

	tree = g.shortest_paths_tree_queue();
	f << "\nShortest paths tree, based on queue:\n";
	tree.print_adjacency_list(f);
	g.print_paths(f);

	tree = g.shortest_paths_tree_priority_queue();
	f << "\nShortest paths tree, based on priority queue:\n";
	tree.print_adjacency_list(f);
	g.print_paths(f);

	f.close();
	return 0;
}