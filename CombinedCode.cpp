#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <functional>

// ================= EDGE =================
class Edge {
public:
    Edge(int src = 0, int dest = 0, int distance = 0, int cost = 0)
        : src(src), dest(dest), distance(distance), cost(cost) {}

    int src;
    int dest;
    int distance;
    int cost;

    bool operator<(const Edge& other) const {
        return distance < other.distance;
    }
};

// ================= VERTEX =================
template <typename T>
class Vertex {
public:
    Vertex(const T& d = T()) : data(d), visited(false) {}

    const T& getData() const { return data; }
    bool getVisited() const { return visited; }
    void setVisited(bool v) { visited = v; }

private:
    T data;
    bool visited;
};

// ================= MIN HEAP =================
template <typename T>
class MinHeap {
public:
    void insert(const T& val);
    T delete_min();
    bool empty() const { return data.empty(); }

private:
    std::vector<T> data;

    void swap(T& v1, T& v2) {
        T tmp = v1;
        v1 = v2;
        v2 = tmp;
    }

    void percolate_down(int i);
};

template<typename T>
void MinHeap<T>::insert(const T& val) {
    data.push_back(val);
    int n = data.size() - 1;

    while (n > 0 && data[n] < data[(n - 1) / 2]) {
        swap(data[n], data[(n - 1) / 2]);
        n = (n - 1) / 2;
    }
}

template<typename T>
T MinHeap<T>::delete_min() {
    if (data.empty()) throw std::string("Empty Heap");

    T res = data[0];
    data[0] = data.back();
    data.pop_back();

    if (!data.empty()) percolate_down(0);
    return res;
}

template<typename T>
void MinHeap<T>::percolate_down(int i) {
    int parent = i;

    while (true) {
        int left = 2 * parent + 1;
        int right = 2 * parent + 2;
        int smallest = parent;

        if (left < data.size() && data[left] < data[smallest])
            smallest = left;
        if (right < data.size() && data[right] < data[smallest])
            smallest = right;

        if (smallest != parent) {
            swap(data[parent], data[smallest]);
            parent = smallest;
        } else break;
    }
}

// ================= GRAPH =================
template <typename T>
class Graph {
public:
    void insert_vertex(const Vertex<T>& ver);
    void add_edge(const Vertex<T>& ver1, const Vertex<T>& ver2,
                  int distance, int cost);

    void print() const;
    void load_csv(const std::string& filename);

    // Traversals
    void DFS(Vertex<T>& ver);
    void BFS(Vertex<T>& ver);

    // Algorithms
    int dijkstra_shortest_path(const Vertex<T>& src, const Vertex<T>& dest);
    void prim_mst(const Vertex<T>& start);
    void kruskal_mst_cost();

private:
    std::vector<Vertex<T>> vertices;
    std::vector<std::vector<Edge>> edges;

    int get_vertex_index(const Vertex<T>& ver);
    void clean_visited();
    void DFS_helper(Vertex<T>& ver);

    std::vector<std::vector<Edge>> build_undirected_cost_graph();
};

// ================= GRAPH METHODS =================
template <typename T>
void Graph<T>::insert_vertex(const Vertex<T>& ver) {
    if (get_vertex_index(ver) == -1) {
        vertices.push_back(ver);
        edges.push_back({});
    }
}

template <typename T>
int Graph<T>::get_vertex_index(const Vertex<T>& ver) {
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].getData() == ver.getData())
            return i;
    }
    return -1;
}

template <typename T>
void Graph<T>::add_edge(const Vertex<T>& ver1, const Vertex<T>& ver2,
                        int distance, int cost) {

    int i1 = get_vertex_index(ver1);
    int i2 = get_vertex_index(ver2);

    if (i1 == -1 || i2 == -1)
        throw std::string("Invalid vertices");

    edges[i1].push_back(Edge(i1, i2, distance, cost));
}

template <typename T>
void Graph<T>::print() const {
    for (int i = 0; i < vertices.size(); i++) {
        std::cout << "{ " << vertices[i].getData() << ": ";
        for (auto& e : edges[i]) {
            std::cout << vertices[e.dest].getData()
                      << "(d=" << e.distance << ", c=" << e.cost << ") ";
        }
        std::cout << "}\n";
    }
}

// ================= CSV LOADER =================
template <typename T>
void Graph<T>::load_csv(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::string("CSV open failed");

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string origin, dest, _, __, dStr, cStr;

        std::getline(ss, origin, ',');
        std::getline(ss, dest, ',');
        std::getline(ss, _, ',');
        std::getline(ss, __, ',');
        std::getline(ss, dStr, ',');
        std::getline(ss, cStr, ',');

        if (dStr.empty() || cStr.empty()) continue;
        /* Debug command to see what was wrong with CSV loader
        std::cout << "DEBUG dStr='" << dStr << "' cStr='" << cStr << "'\n"; */
        int d = std::stoi(dStr);
        int c = std::stoi(cStr);

        Vertex<T> v1(origin), v2(dest);
        insert_vertex(v1);
        insert_vertex(v2);
        add_edge(v1, v2, d, c);
    }
}

// ================= DFS =================
template <typename T>
void Graph<T>::clean_visited() {
    for (auto& v : vertices) v.setVisited(false);
}

template <typename T>
void Graph<T>::DFS(Vertex<T>& ver) {
    clean_visited();
    DFS_helper(ver);
    clean_visited();
}

template <typename T>
void Graph<T>::DFS_helper(Vertex<T>& ver) {
    int i = get_vertex_index(ver);
    if (i == -1) throw std::string("DFS error");

    vertices[i].setVisited(true);
    std::cout << vertices[i].getData() << " ";

    for (auto& e : edges[i]) {
        if (!vertices[e.dest].getVisited())
            DFS_helper(vertices[e.dest]);
    }
}

// ================= BFS =================
template <typename T>
void Graph<T>::BFS(Vertex<T>& ver) {
    clean_visited();

    int i = get_vertex_index(ver);
    if (i == -1) throw std::string("BFS error");

    std::queue<int> q;
    q.push(i);
    vertices[i].setVisited(true);

    while (!q.empty()) {
        int cur = q.front(); q.pop();
        std::cout << vertices[cur].getData() << " ";

        for (auto& e : edges[cur]) {
            if (!vertices[e.dest].getVisited()) {
                vertices[e.dest].setVisited(true);
                q.push(e.dest);
            }
        }
    }

    clean_visited();
}

// ================= DIJKSTRA =================
template<typename T>
int Graph<T>::dijkstra_shortest_path(const Vertex<T>& src, const Vertex<T>& dest) {

    int s = get_vertex_index(src);
    int t = get_vertex_index(dest);

    if (s == -1 || t == -1)
        throw std::string("Invalid vertices");

    std::vector<int> dist(vertices.size(), INT_MAX);
    std::vector<bool> visited(vertices.size(), false);
    std::vector<int> parent(vertices.size(), -1);

    dist[s] = 0;
    MinHeap<Edge> heap;
    heap.insert(Edge(-1, s, 0, 0));

    while (!heap.empty()) {
        Edge cur = heap.delete_min();
        int u = cur.dest;

        if (visited[u]) continue;
        visited[u] = true;

        for (auto& e : edges[u]) {
            int v = e.dest;
            int nd = dist[u] + e.distance;

            if (nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                heap.insert(Edge(u, v, nd, 0));
            }
        }
    }

    if (dist[t] == INT_MAX) return -1;

    std::cout << "Distance: " << dist[t] << "\n";
    return dist[t];
}

// ================= MST =================
template<typename T>
std::vector<std::vector<Edge>> Graph<T>::build_undirected_cost_graph() {

    int n = vertices.size();
    std::vector<std::vector<Edge>> undirected(n);
    std::map<std::pair<int,int>, Edge> best;

    for (int u = 0; u < n; u++) {
        for (auto& e : edges[u]) {
            int a = std::min(u, e.dest);
            int b = std::max(u, e.dest);

            if (!best.count({a,b}) || e.cost < best[{a,b}].cost)
                best[{a,b}] = e;
        }
    }

    for (auto& p : best) {
        int u = p.first.first, v = p.first.second;
        int c = p.second.cost;

        undirected[u].push_back(Edge(u,v,0,c));
        undirected[v].push_back(Edge(v,u,0,c));
    }

    return undirected;
}

template<typename T>
void Graph<T>::prim_mst(const Vertex<T>& start) {

    auto g = build_undirected_cost_graph();
    int s = get_vertex_index(start);

    std::vector<bool> vis(vertices.size(), false);
    MinHeap<Edge> heap;

    vis[s] = true;
    for (auto& e : g[s]) heap.insert(e);

    int total = 0;

    while (!heap.empty()) {
        Edge e = heap.delete_min();
        if (vis[e.dest]) continue;

        vis[e.dest] = true;
        total += e.cost;

        std::cout << vertices[e.src].getData()
                  << "-" << vertices[e.dest].getData()
                  << " (" << e.cost << ")\n";

        for (auto& ne : g[e.dest])
            if (!vis[ne.dest]) heap.insert(ne);
    }

    std::cout << "Total: " << total << "\n";
}

template<typename T>
void Graph<T>::kruskal_mst_cost() {

    auto g = build_undirected_cost_graph();
    std::vector<Edge> list;

    for (int u = 0; u < g.size(); u++)
        for (auto& e : g[u])
            if (u < e.dest) list.push_back(e);

    std::sort(list.begin(), list.end(),
              [](Edge a, Edge b){ return a.cost < b.cost; });

    std::vector<int> parent(vertices.size());
    for (int i = 0; i < parent.size(); i++) parent[i] = i;

    std::function<int(int)> find = [&](int x){
        return parent[x]==x?x:parent[x]=find(parent[x]);
    };

    int total = 0;

    for (auto& e : list) {
        if (find(e.src) != find(e.dest)) {
            parent[find(e.src)] = find(e.dest);
            total += e.cost;

            std::cout << vertices[e.src].getData()
                      << "-" << vertices[e.dest].getData()
                      << " (" << e.cost << ")\n";
        }
    }

    std::cout << "Total: " << total << "\n";
}

// ================= MAIN =================
int main() {
    
    Graph<std::string> g;
    // CSV Testing
    g.load_csv("airports.csv");
    g.print();
    /* Vertex<std::string> src("JFK");   
    Vertex<std::string> dest("LAX");  

    std::cout << "\nDijkstra:\n";
    g.dijkstra_shortest_path(src, dest); */

    /* Manual testing
    Vertex<std::string> A("A"), B("B"), C("C"), D("D");

    g.insert_vertex(A);
    g.insert_vertex(B);
    g.insert_vertex(C);
    g.insert_vertex(D);

    g.add_edge(A,B,5,3);
    g.add_edge(B,A,6,2);
    g.add_edge(A,C,7,4);
    g.add_edge(B,C,3,1);
    g.add_edge(C,D,2,2);
    g.add_edge(B,D,4,5);

    g.print();

    std::cout << "\nDFS: ";
    g.DFS(A);

    std::cout << "\nBFS: ";
    g.BFS(A);

    std::cout << "\nDijkstra A->D:\n";
    g.dijkstra_shortest_path(A,D);

    std::cout << "\nPrim MST:\n";
    g.prim_mst(A);

    std::cout << "\nKruskal MST:\n";
    g.kruskal_mst_cost(); */

    return 0;
}
