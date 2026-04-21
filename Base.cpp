#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>
#include <unistd.h>
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
    MinHeap() {}
    void insert(const T& val);
    void print() const;
    T delete_min();
    bool empty() const;
private:
    std::vector<T> data;

    void swap(T& v1, T& v2) {
        T tmp = v1;
        v1 = v2;
        v2 = tmp;
    }

    void percolate_down(int i);
    int min_index(int i1, int i2) const;
};
template<typename T>
bool MinHeap<T>::empty() const {
    return data.empty();
}
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
void MinHeap<T>::print() const {
    int cur_level = 0, new_level = 1;

    for (const T& i : data) {
        std::cout << i << ' ';
        cur_level++;
        if (cur_level == new_level) {
            std::cout << "\n";
            new_level *= 2;
            cur_level = 0;
        }
    }
    std::cout << "\n----------------------------\n";
}

template<typename T>
T MinHeap<T>::delete_min() {
    if (data.empty()) {
        throw std::string("delete_min: Empty Heap");
    }

    T res = data[0];
    data[0] = data.back();
    data.pop_back();

    if (!data.empty())
        percolate_down(0);

    return res;
}

template<typename T>
void MinHeap<T>::percolate_down(int i) {
    if (i < 0 || i >= data.size()) return;

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

template<typename T>
int MinHeap<T>::min_index(int i1, int i2) const {
    if (i1 >= data.size() || i2 >= data.size())
        throw std::string("min_index: incorrect index");

    return (data[i1] < data[i2] ? i1 : i2);
}

// ================= GRAPH =================
template <typename T>
class Graph {
public:
    void insert_vertex(const Vertex<T>& ver);

    // FIXED: now includes distance + cost
    void add_edge(const Vertex<T>& ver1, const Vertex<T>& ver2,
                  int distance, int cost);

    void print() const;

    void DFS(Vertex<T>& ver);
    void BFS(Vertex<T>& ver);

    int dijkstra_shortest_path(const Vertex<T>& src, const Vertex<T>& dest);
    void prim_mst(const Vertex<T>& start);
    void kruskal_mst_cost();
    std::vector<std::vector<Edge>> build_undirected_cost_graph();
     // safety fallback for infinite loop in Dijkstra's (should never happen)

private:
    std::vector<Vertex<T>> vertices;
    std::vector<std::vector<Edge>> edges;

    void clean_visited();
    void DFS_helper(Vertex<T>& ver);
    int get_vertex_index(const Vertex<T>& ver);
};

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
void Graph<T>::add_edge(const Vertex<T>& ver1, const Vertex<T>& ver2, int distance, int cost) {

    int i1 = get_vertex_index(ver1);
    int i2 = get_vertex_index(ver2);

    if (i1 == -1 || i2 == -1)
        throw std::string("Add_edge: incorrect vertices");

    edges[i1].push_back(Edge(i1, i2, distance, cost));
}

template <typename T>
void Graph<T>::print() const {
    for (int i = 0; i < vertices.size(); i++) {
        std::cout << "{ " << vertices[i].getData() << ": ";
        for (auto& e : edges[i]) {
            std::cout << '{' << vertices[e.dest].getData()
                      << ", " << e.cost << "} ";
        }
        std::cout << "}\n";
    }
}

template <typename T>
void Graph<T>::clean_visited() {
    for (auto& v : vertices)
        v.setVisited(false);
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
    if (i == -1) throw std::string("DFS: Vertex not found");

    vertices[i].setVisited(true);
    std::cout << vertices[i].getData() << ' ';

    for (auto& e : edges[i]) {
        if (!vertices[e.dest].getVisited()) {
            DFS_helper(vertices[e.dest]);
        }
    }
}

template <typename T>
void Graph<T>::BFS(Vertex<T>& ver) {
    clean_visited();

    int i = get_vertex_index(ver);
    if (i == -1) throw std::string("BFS: Vertex not found");

    std::queue<int> q;
    q.push(i);
    vertices[i].setVisited(true);

    while (!q.empty()) {
        int cur = q.front();
        std::cout << vertices[cur].getData() << ' ';

        for (auto& e : edges[cur]) {
            if (!vertices[e.dest].getVisited()) {
                vertices[e.dest].setVisited(true);
                q.push(e.dest);
            }
        }
        q.pop();
    }

    clean_visited();
}

/*template<typename T>
int Graph<T>::dijkstra_shortest_path(const Vertex<T>& src, const Vertex<T>& dest) {

    int i_src = get_vertex_index(src);
    int i_dest = get_vertex_index(dest);

    if (i_src == -1 || i_dest == -1)
        throw std::string("Dijkstra: incorrect vertices");

    std::vector<int> dist(vertices.size(), INT_MAX);
    std::vector<bool> visited(vertices.size(), false);

    dist[i_src] = 0;

    MinHeap<Edge> heap;
    heap.insert(Edge(-1, i_src, 0, 0));

    while (true) {

        if (dist[i_dest] != INT_MAX) break;

        Edge current = heap.delete_min();
        int u = current.dest;

        if (visited[u]) continue;
        visited[u] = true;

        for (auto& e : edges[u]) {
            int v = e.dest;
            int newDist = dist[u] + e.distance;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                heap.insert(Edge(u, v, newDist, 0));
            }
        }

        if (heapEmptyCheckHack()) break; // safety fallback
    }

    return dist[i_dest];
}*/
template<typename T>
std::vector<std::vector<Edge>> Graph<T>::build_undirected_cost_graph() {

    int n = vertices.size();
    std::vector<std::vector<Edge>> undirected(n);

    // map to track best cost between pairs
    std::map<std::pair<int,int>, Edge> bestEdge;

    for (int u = 0; u < n; u++) {
        for (const Edge& e : edges[u]) {
            int v = e.dest;

            int a = std::min(u, v);
            int b = std::max(u, v);

            auto key = std::make_pair(a, b);

            if (bestEdge.find(key) == bestEdge.end() ||
                e.cost < bestEdge[key].cost) {
                bestEdge[key] = e;
            }
        }
    }

    // build undirected adjacency list
    for (auto& entry : bestEdge) {
        Edge e = entry.second;
        int u = entry.first.first;
        int v = entry.first.second;

        undirected[u].push_back(Edge(u, v, 0, e.cost));
        undirected[v].push_back(Edge(v, u, 0, e.cost));
    }

    return undirected;
}
template<typename T>    
void Graph<T>::prim_mst(const Vertex<T>& start) {

    auto gu = build_undirected_cost_graph();

    int startIndex = get_vertex_index(start);
    if (startIndex == -1)
        throw std::string("Prim: invalid start");

    std::vector<bool> visited(vertices.size(), false);
    MinHeap<Edge> heap;

    int totalCost = 0;
    int edgesUsed = 0;

    visited[startIndex] = true;

    for (const Edge& e : gu[startIndex]) {
        heap.insert(e);
    }

    std::cout << "MST (Prim - cost):\n";

    while (!heap.empty()) {
        Edge e = heap.delete_min();

        int u = e.src;
        int v = e.dest;

        if (visited[v]) continue;

        visited[v] = true;
        edgesUsed++;

        totalCost += e.cost;

        std::cout << vertices[u].getData()
                  << " - "
                  << vertices[v].getData()
                  << " (" << e.cost << ")\n";

        for (const Edge& next : gu[v]) {
            if (!visited[next.dest]) {
                heap.insert(next);
            }
        }
    }

    std::cout << "Total MST cost: " << totalCost << "\n";
}    
template<typename T>   
void Graph<T>::kruskal_mst_cost() {

    auto gu = build_undirected_cost_graph();

    std::vector<Edge> edgeList;

    for (int u = 0; u < gu.size(); u++) {
        for (const Edge& e : gu[u]) {
            if (u < e.dest) { // avoid duplicates
                edgeList.push_back(e);
            }
        }
    }

    std::sort(edgeList.begin(), edgeList.end(),
              [](const Edge& a, const Edge& b) {
                  return a.cost < b.cost;
              });

    std::vector<int> parent(vertices.size());
    for (int i = 0; i < parent.size(); i++) parent[i] = i;

    std::function<int(int)> find = [&](int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    };

    auto unite = [&](int a, int b) {
        parent[find(a)] = find(b);
    };

    int totalCost = 0;

    std::cout << "MST (Kruskal - cost):\n";

    for (const Edge& e : edgeList) {
        int u = e.src;
        int v = e.dest;

        if (find(u) != find(v)) {
            unite(u, v);

            totalCost += e.cost;

            std::cout << vertices[u].getData()
                      << " - "
                      << vertices[v].getData()
                      << " (" << e.cost << ")\n";
        }
    }

    std::cout << "Total MST cost: " << totalCost << "\n";

   /*
    // check connectivity
    if (edgesUsed != vertices.size() - 1) {
        std::cout << "Graph is disconnected — MST cannot be formed\n";
        return;
    }
        */


}
// ================= MAIN ================= example test
int main() {

   Vertex<std::string> A("A"), B("B"), C("C"), D("D");

Graph<std::string> g;

g.insert_vertex(A);
g.insert_vertex(B);
g.insert_vertex(C);
g.insert_vertex(D);

// Directed edges (distance, cost)

// A ↔ B (two edges → should keep min cost = 2)
g.add_edge(A, B, 5, 3);
g.add_edge(B, A, 6, 2);

// A → C (one edge → keep as-is, cost = 4)
g.add_edge(A, C, 7, 4);

// B ↔ C (two edges → min cost = 1)
g.add_edge(B, C, 3, 1);
g.add_edge(C, B, 8, 6);

// C → D (one edge → cost = 2)
g.add_edge(C, D, 2, 2);

// B → D (one edge → cost = 5)
g.add_edge(B, D, 4, 5);
    g.print();

    std::cout << "\n";
    std::cout << "Prim's MST:\n";
   
g.prim_mst(A);

std::cout << "\nKruskal MST:\n";
g.kruskal_mst_cost();
    // std::cout << g.dijkstra_shortest_path(v7, v) << "\n";

    return 0;
}
