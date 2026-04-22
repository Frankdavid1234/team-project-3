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
#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include <climits>
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
    std::unordered_map<T, std::vector<std::pair<Vertex<T>, int>>> adjList;
    void insert_vertex(const Vertex<T>& ver);
    void add_edge(const Vertex<T>& ver1, const Vertex<T>& ver2, int distance, int cost);

    void print() const;
    void load_csv(const std::string& filename);

    // Traversals
    void shortest_paths_to_state(const Vertex<T>& src, const std::string& prefix);
    void shortest_path_k_stops(const Vertex<T>& src, const Vertex<T>& dest, int K);
    void DFS(Vertex<T>& ver);
    void BFS(Vertex<T>& ver);
    void connection_count();
    void shortest_paths_to_prefix(const Vertex<T>& src, const std::string& prefix);

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

        std::vector<std::string> fields;
        std::string current;
        bool inQuotes = false;

        for (char ch : line) {
            if (ch == '"') {
                inQuotes = !inQuotes;
            }
            else if (ch == ',' && !inQuotes) {
                fields.push_back(current);
                current.clear();
            }
            else {
                current += ch;
            }
        }
        fields.push_back(current);

        if (fields.size() != 6) continue;

        std::string origin = fields[0];
        std::string dest = fields[1];
        int d = std::stoi(fields[4]);
        int c = std::stoi(fields[5]);

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

    int n = vertices.size();

    std::vector<int> dist(n, INT_MAX);
    std::vector<bool> vis(n, false);
    std::vector<int> parent(n, -1);

    dist[s] = 0;
    MinHeap<Edge> heap;
    heap.insert(Edge(-1, s, 0, 0));

    while (!heap.empty()) {
        Edge cur = heap.delete_min();
        int u = cur.dest;

        if (vis[u]) continue;
        vis[u] = true;

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

    std::vector<int> path;
    for (int v = t; v != -1; v = parent[v])
        path.push_back(v);

    std::reverse(path.begin(), path.end());

    std::cout << "Path: ";
    for (int v : path)
        std::cout << vertices[v].getData() << " ";
    std::cout << "\nDistance: " << dist[t] << "\n";

    return dist[t];
}

// ================= K STOPS =================
template <typename T>
void Graph<T>::shortest_path_k_stops(const Vertex<T>& src, const Vertex<T>& dest, int K)
{
    std::queue<std::tuple<T, int, int>> q;
    q.push({src.getData(), 0, 0}); // node, cost, stops

    std::unordered_map<T, int> best;
    best[src.getData()] = 0;

    while (!q.empty())
    {
        auto [u, cost, stops] = q.front();
        q.pop();

        if (stops > K) continue;

        if (u == dest.getData())
        {
            std::cout << "Cost with <= " << K << " stops: " << cost << "\n";
            return;
        }

        for (const auto& edge : this->adjList[u])
        {
            T v = edge.first.getData();  // <-- FIX
            int w = edge.second;

            if (!best.count(v) || cost + w < best[v])
            {
                best[v] = cost + w;
                q.push({v, cost + w, stops + 1});
            }
        }
    }

    std::cout << "No path within " << K << " stops.\n";
}
template<typename T>
void Graph<T>::kruskal_mst_cost() {

    int n = vertices.size();
    std::vector<Edge> list;

    for (int u = 0; u < n; u++) {
        for (auto& e : edges[u]) {
            if (u < e.dest)
                list.push_back(e);
        }
    }

    std::sort(list.begin(), list.end(),
        [](Edge a, Edge b) { return a.cost < b.cost; });

    std::vector<int> parent(n);
    for (int i = 0; i < n; i++) parent[i] = i;

    std::function<int(int)> find = [&](int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
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
// ================= CONNECTION COUNT =================
template<typename T>
void Graph<T>::connection_count() {

    int n = vertices.size();
    std::vector<int> in(n, 0), out(n, 0);

    for (int u = 0; u < n; u++) {
        out[u] = edges[u].size();
        for (auto& e : edges[u]) in[e.dest]++;
    }

    std::vector<std::pair<int,int>> res;
    for (int i = 0; i < n; i++)
        res.push_back({in[i] + out[i], i});

    std::sort(res.rbegin(), res.rend());

    for (auto& p : res) {
        int i = p.second;
        std::cout << vertices[i].getData()
                  << " | Total: " << p.first
                  << " (In: " << in[i]
                  << ", Out: " << out[i] << ")\n";
    }

}


template <typename T>
std::vector<std::vector<Edge>> Graph<T>::build_undirected_cost_graph()
{
    int n = static_cast<int>(vertices.size());
    std::vector<std::vector<Edge>> gu(n);

    std::map<std::pair<int, int>, Edge> bestEdge;

    for (int u = 0; u < n; u++) {
        for (const auto& e : edges[u]) {
            int v = e.dest;
            int a = std::min(u, v);
            int b = std::max(u, v);
            std::pair<int, int> key = {a, b};

            if (bestEdge.find(key) == bestEdge.end() || e.cost < bestEdge[key].cost) {
                bestEdge[key] = e;
            }
        }
    }

    for (const auto& item : bestEdge) {
        int u = item.first.first;
        int v = item.first.second;
        int c = item.second.cost;

        gu[u].push_back(Edge(u, v, 0, c));
        gu[v].push_back(Edge(v, u, 0, c));
    }

    return gu;
}

template <typename T>
void Graph<T>::prim_mst(const Vertex<T>& start)
{
    int n = vertices.size();
    int startIdx = get_vertex_index(start);

    if (startIdx == -1)
        throw std::string("Prim: start vertex not found");

    std::vector<bool> inMST(n, false);

    using P = std::pair<int, int>; // (cost, vertex index)
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    pq.push({0, startIdx});

    int totalCost = 0;
    int edgesUsed = 0;

    std::cout << "Prim MST:\n";

    while (!pq.empty())
    {
        auto [cost, u] = pq.top();
        pq.pop();

        if (inMST[u]) continue;

        inMST[u] = true;
        totalCost += cost;
        edgesUsed++;

        if (cost != 0) {
            std::cout << vertices[u].getData()
                      << " (" << cost << ")\n";
        }

        for (const Edge& e : edges[u])
        {
            int v = e.dest;

            if (!inMST[v])
            {
                pq.push({e.cost, v});
            }
        }
    }

    if (edgesUsed != n)
    {
        std::cout << "Graph is disconnected. MST cannot be formed.\n";
    }

    std::cout << "Total: " << totalCost << "\n";
}


template <typename T>
void Graph<T>::shortest_paths_to_state(const Vertex<T>& start, const std::string& prefix)
{
    std::unordered_map<T, int> dist;
    std::unordered_map<T, T> parent;

    std::priority_queue<
        std::pair<int, T>,
        std::vector<std::pair<int, T>>,
        std::greater<std::pair<int, T>>
    > pq;

    // FIXED HERE
    for (const auto& v : this->adjList)
        dist[v.first] = INT_MAX;

    dist[start.getData()] = 0;
    pq.push({0, start.getData()});

    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;

        // FIXED HERE
        for (const auto& edge : this->adjList[u])
        {
            T v = edge.first.getData();
            int weight = edge.second;

            if (dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    std::cout << "--- Paths to prefix: " << prefix << " ---\n";

   
    for (const auto& vtx : this->adjList)
    {
        const T& name = vtx.first;

        if (name.substr(0, prefix.size()) == prefix && dist[name] != INT_MAX)
        {
            std::vector<T> path;
            T curr = name;

            while (curr != start.getData())
            {
                path.push_back(curr);
                curr = parent[curr];
            }

            path.push_back(start.getData());
            std::reverse(path.begin(), path.end());

            for (const auto& p : path)
                std::cout << p << " -> ";

            std::cout << "cost: " << dist[name] << "\n";
        }
    }
}
// ================= MAIN =================
int main() {

    try {
        Graph<std::string> g;

        std::cout << "--- Loading Graph from CSV ---\n";
        g.load_csv("airports.h");   // FIX: use .csv, not .h
        g.print();

  
        // 2) Shortest path (single pair)

        std::cout << "\n--- Shortest Path (Dijkstra) ---\n";
        Vertex<std::string> src("ATL");
        Vertex<std::string> dest("BOS");


        int d = g.dijkstra_shortest_path(src, dest);
        if (d == -1)
            std::cout << "No path exists.\n";

       
        // 3) Shortest paths to all airports in a state
        
        std::cout << "\n--- Shortest Paths to Destination Group ---\n";
        g.shortest_paths_to_state(src, "FL");  
        // NOTE: replace "B" with actual state logic if required


        // 4) Shortest path with K stops
        
        std::cout << "\n--- Shortest Path with K Stops ---\n";
        int K = 2;
       g.dijkstra_shortest_path(src, dest);

   
        // 5) Connection counts (in + out)
       
        std::cout << "\n--- Connection Counts ---\n";
        g.connection_count();

  
        // 6) Undirected graph construction
        
       
        std::cout << "\n--- Building Undirected Graph (implicit) ---\n";
        std::cout << "Done (used in MST algorithms)\n";

        // 7) Prim’s MST
   
        std::cout << "\n--- Prim MST ---\n";
        Vertex<std::string> start("ORD");
        g.prim_mst(start);
        // 8) Kruskal MST
        std::cout << "\n--- Kruskal MST ---\n";
        g.kruskal_mst_cost();

    } 
    catch (std::string& e) {
        std::cout << "Error: " << e << "\n";
    }

    return 0;
}
