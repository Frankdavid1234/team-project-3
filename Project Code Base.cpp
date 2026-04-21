#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>
#include <unistd.h>
#include <fstream>
#include <sstream>

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
    bool empty() const { return data.empty(); }

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
    for (const T& i : data) {
        std::cout << i << ' ';
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
    return (data[i1] < data[i2] ? i1 : i2);
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

    int dijkstra_shortest_path(const Vertex<T>& src, const Vertex<T>& dest);

private:
    std::vector<Vertex<T>> vertices;
    std::vector<std::vector<Edge>> edges;

    int get_vertex_index(const Vertex<T>& ver);
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
        throw std::string("Add_edge: incorrect vertices");

    edges[i1].push_back(Edge(i1, i2, distance, cost));
}

// ================= CSV LOADER (FIXED) =================

template <typename T>
void Graph<T>::load_csv(const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::string("Could not open CSV file");
    }

    std::string line;

    std::getline(file, line); // skip header

    while (std::getline(file, line)) {

        if (line.empty()) continue;

        std::stringstream ss(line);

        std::string origin, dest;
        std::string origin_city, dest_city;
        std::string distanceStr, costStr;

        std::getline(ss, origin, ',');
        std::getline(ss, dest, ',');
        std::getline(ss, origin_city, ',');
        std::getline(ss, dest_city, ',');
        std::getline(ss, distanceStr, ',');
        std::getline(ss, costStr, ',');

        if (distanceStr.empty() || costStr.empty()) continue;

        int distance = std::stoi(distanceStr);
        int cost = std::stoi(costStr);

        Vertex<T> v1(origin);
        Vertex<T> v2(dest);

        insert_vertex(v1);
        insert_vertex(v2);

        add_edge(v1, v2, distance, cost);
    }

    file.close();
}

// ================= DIJKSTRA =================

template<typename T>
int Graph<T>::dijkstra_shortest_path(const Vertex<T>& src,
                                      const Vertex<T>& dest) {

    int i_src = get_vertex_index(src);
    int i_dest = get_vertex_index(dest);

    if (i_src == -1 || i_dest == -1)
        throw std::string("Dijkstra: incorrect vertices");

    std::vector<int> dist(vertices.size(), INT_MAX);
    std::vector<bool> visited(vertices.size(), false);

    dist[i_src] = 0;

    MinHeap<Edge> heap;
    heap.insert(Edge(-1, i_src, 0, 0));

    while (!heap.empty()) {

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
    }

    return dist[i_dest];
}

// ================= MAIN =================

int main() {
    Graph<std::string> airports;
    airports.load_csv("airports.csv");

    return 0;
}
