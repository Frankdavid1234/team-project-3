#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>
#include <unistd.h>

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
void Graph<T>::add_edge(const Vertex<T>& ver1, const Vertex<T>& ver2,
                        int distance, int cost) {

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
                      << ", " << e.weight << "} ";
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

template<typename T>
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
}
// ================= MAIN ================= example test
int main() {
    Vertex<std::string> v("SFO"), v2("LAX"), v3("DFW"),
                        v4("ORD"), v5("JFK"), v6("BOS"), v7("MIA");

    Graph<std::string> airports;

    airports.insert_vertex(v);
    airports.insert_vertex(v2);
    airports.insert_vertex(v3);
    airports.insert_vertex(v4);
    airports.insert_vertex(v5);
    airports.insert_vertex(v6);
    airports.insert_vertex(v7);

    airports.add_edge(v, v2, 337);
    airports.add_edge(v, v3, 1464);
    airports.add_edge(v, v4, 1846);
    airports.add_edge(v, v6, 2704);
    airports.add_edge(v3, v4, 802);
    airports.add_edge(v6, v4, 867);
    airports.add_edge(v5, v6, 187);
    airports.add_edge(v5, v7, 1090);
    airports.add_edge(v6, v7, 1258);
    airports.add_edge(v2, v7, 2342);
    airports.add_edge(v2, v3, 1235);
    airports.add_edge(v7, v3, 1121);
    airports.add_edge(v4, v5, 740);

    airports.print();

    std::cout << "\n";
    std::cout << airports.shortest_path(v7, v) << "\n";
    std::cout << airports.dijkstra_shortest_path(v7, v) << "\n";

    return 0;
}