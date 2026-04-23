

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <climits>
#include <iomanip>

// ================= EDGE =================
class Edge {
public:
    int src, dest, distance, cost;
    Edge(int s=0,int d=0,int dist=0,int c=0)
        :src(s),dest(d),distance(dist),cost(c){}
    bool operator<(const Edge& o) const {
        return distance < o.distance;
    }
};

// ================= VERTEX =================
template<typename T>
class Vertex {
public:
    Vertex(const T& d=T()):data(d),visited(false){}
    T data;
    bool visited;
};

// ================= QUEUE =================
template<typename T>
class Queue {
public:
    void push(T v){buf.push_back(v);}
    T front(){return buf[head];}
    void pop(){head++;}
    bool empty(){return head>=buf.size();}
private:
    std::vector<T> buf;
    int head=0;
};

// ================= MIN HEAP =================
template<typename T>
class MinHeap {
public:
    void insert(T v){
        data.push_back(v);
        int i=data.size()-1;
        while(i>0 && data[i]<data[(i-1)/2]){
            std::swap(data[i],data[(i-1)/2]);
            i=(i-1)/2;
        }
    }

    T extract(){
        T res=data[0];
        data[0]=data.back();
        data.pop_back();
        heapify(0);
        return res;
    }

    bool empty(){return data.empty();}

private:
    std::vector<T> data;

    void heapify(int i){
        int l=2*i+1,r=2*i+2,s=i;
        if(l<data.size() && data[l]<data[s]) s=l;
        if(r<data.size() && data[r]<data[s]) s=r;
        if(s!=i){
            std::swap(data[i],data[s]);
            heapify(s);
        }
    }
};

// ================= STATE =================
struct StateEntry {
    std::string airport, state;
};

// ================= GRAPH =================
template<typename T>
class Graph {
public:
    std::vector<Vertex<T>> V;
    std::vector<std::vector<Edge>> adj;
    std::vector<StateEntry> states;

    // ---------- BASIC ----------
    int index(T name){
        for(int i=0;i<V.size();i++)
            if(V[i].data==name) return i;
        return -1;
    }

    void add_vertex(T name){
        if(index(name)==-1){
            V.push_back(Vertex<T>(name));
            adj.push_back({});
        }
    }

    void add_edge(T a,T b,int d,int c){
        int i=index(a),j=index(b);
        adj[i].push_back(Edge(i,j,d,c));
    }

    // ---------- STATE ----------
    void setState(T a,std::string s){
        for(auto& e:states)
            if(e.airport==a){e.state=s;return;}
        states.push_back({a,s});
    }

    std::string getState(T a){
        for(auto& e:states)
            if(e.airport==a) return e.state;
        return "";
    }
    void shortest_path_k_stops(const T& start, const T& dest, int K){
        int s = index(start);
        int d = index(dest);

        if(s == -1 || d == -1){
            std::cout << "Shortest route from " << start << " to " << dest
                    << " with " << K << " stops: None\n";
            return;
        }

        struct State {
            int node;
            int dist;
            int cost;
            int stops;
            std::vector<int> path;
        };

        int n = V.size();

        // best[node][stops] = best distance seen
        std::vector<std::vector<int>> best(n, std::vector<int>(K+2, INT_MAX));

        Queue<State> q;

        best[s][0] = 0;
        q.push({s, 0, 0, 0, {s}});

        int bestDist = INT_MAX;
        int bestCost = 0;
        std::vector<int> bestPath;

        while(!q.empty()){
            State cur = q.front(); q.pop();

            int u = cur.node;

            if(u == d){
                if(cur.dist < bestDist){
                    bestDist = cur.dist;
                    bestCost = cur.cost;
                    bestPath = cur.path;
                }
                continue;
            }

            // stops exceeded
            if(cur.stops >= K+1) continue;

            for(const auto& e : adj[u]){
                int v = e.dest;
                int nd = cur.dist + e.distance;
                int nc = cur.cost + e.cost;
                int ns = cur.stops + 1;

                if(ns <= K+1 && nd < best[v][ns]){
                    best[v][ns] = nd;

                    std::vector<int> newPath = cur.path;
                    newPath.push_back(v);

                    q.push({v, nd, nc, ns, newPath});
                }
            }
        }

        if(bestDist == INT_MAX){
            std::cout << "Shortest route from " << start << " to " << dest
                    << " with " << K << " stops: None\n";
            return;
        }

        // ===== PRINT RESULT =====
        std::cout << "Shortest route from " << start << " to " << dest
                << " with " << K << " stops: ";

        for(int i = 0; i < (int)bestPath.size(); i++){
            if(i) std::cout << " -> ";
            std::cout << V[bestPath[i]].data;
        }

        std::cout << ". The length is " << bestDist
                << ". The cost is " << bestCost << ".\n";
    }

    // ---------- CSV ----------
    struct PrimEntry {
    int src, dest, cost;
    bool operator<(const PrimEntry& o) const {
        return cost < o.cost;
    }
};
std::string extractState(const std::string& city){
    size_t pos = city.find(',');
    if(pos == std::string::npos) return "";
    size_t start = pos + 1;
    while(start < city.size() && (city[start] == ' ' || city[start] == '\"')) start++;

    std::string st;
    while(start < city.size() && city[start] != '\"' && city[start] != '\r')
        st += city[start++];

    return st;
}
    void load_csv(const std::string& file){
    std::ifstream f(file);
    if(!f.is_open()){
        std::cout << "Could not open CSV file.\n";
        return;
    }

    std::string line;
    std::getline(f, line); // header

    while(std::getline(f, line)){
        if(line.empty()) continue;

        std::vector<std::string> x;
        std::string cur;
        bool inQuotes = false;

        for(char c : line){
            if(c == '"'){
                inQuotes = !inQuotes;
            } else if(c == ',' && !inQuotes){
                x.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }
        x.push_back(cur);

        if(x.size() != 6) continue;

        std::string origin = x[0];
        std::string dest   = x[1];
        std::string oCity  = x[2];
        std::string dCity  = x[3];
        int dist = std::stoi(x[4]);
        int cost = std::stoi(x[5]);

        add_vertex(origin);
        add_vertex(dest);
        add_edge(origin, dest, dist, cost);

        setState(origin, extractState(oCity));
        setState(dest, extractState(dCity));
    }
}
    // ================= TASK 2 =================
    void dijkstra(T s,T t){
        int src=index(s),dst=index(t);
        if(src==-1||dst==-1){
            std::cout<<"None\n"; return;
        }

        int n=V.size();
        std::vector<int> dist(n,INT_MAX),cost(n,0),par(n,-1);
        std::vector<bool> vis(n,false);

        dist[src]=0;
        MinHeap<Edge> pq;
        pq.insert(Edge(-1,src,0,0));

        while(!pq.empty()){
            int u=pq.extract().dest;
            if(vis[u]) continue;
            vis[u]=true;

            for(auto& e:adj[u]){
                int v=e.dest;
                if(dist[u]+e.distance<dist[v]){
                    dist[v]=dist[u]+e.distance;
                    cost[v]+=e.cost;
                    par[v]=u;
                    pq.insert(Edge(u,v,dist[v],0));
                }
            }
        }

        if(dist[dst]==INT_MAX){
            std::cout<<"Shortest route from "<<s<<" to "<<t<<": None\n";
            return;
        }

        std::vector<int> path;
        for(int v=dst;v!=-1;v=par[v]) path.push_back(v);

        std::cout<<"Shortest route from "<<s<<" to "<<t<<": ";
        for(int i=path.size()-1;i>=0;i--){
            std::cout<<V[path[i]].data;
            if(i) std::cout<<" -> ";
        }

        std::cout<<". The length is "<<dist[dst]
                 <<". The cost is "<<cost[dst]<<".\n";
    }

    // ================= TASK 5 =================
    void connection_count(){
        int n=V.size();
        std::vector<int> in(n,0),out(n,0);

        for(int i=0;i<n;i++){
            out[i]=adj[i].size();
            for(auto& e:adj[i]) in[e.dest]++;
        }

        for(int i=0;i<n;i++){
            std::cout<<V[i].data<<" "
                     <<(in[i]+out[i])<<"\n";
        }
    }

    // ================= TASK 6 =================
    Graph<T> build_undirected(){
        Graph<T> g2;
        g2.V = V;
        g2.adj.resize(V.size());

        int n=V.size();

        for(int u=0;u<n;u++){
            for(auto& e:adj[u]){
                int v=e.dest;

                bool found=false;
                int bestCost=e.cost;

                for(auto& rev:adj[v]){
                    if(rev.dest==u){
                        found=true;
                        if(rev.cost<bestCost)
                            bestCost=rev.cost;
                    }
                }

                if(u<v){
                    g2.adj[u].push_back(Edge(u,v,0,bestCost));
                    g2.adj[v].push_back(Edge(v,u,0,bestCost));
                }
            }
        }

        return g2;
    }

    // ================= TASK 7 =================
    void prim(){
        int n=V.size();
        std::vector<bool> vis(n,false);
        MinHeap<Edge> pq;

        pq.insert(Edge(-1,0,0,0));

        int total=0, count=0;

        while(!pq.empty()){
            Edge e=pq.extract();
            int u=e.dest;

            if(vis[u]) continue;
            vis[u]=true;
            total+=e.cost;
            count++;

            if(e.src!=-1)
                std::cout<<V[e.src].data<<"-"<<V[u].data<<" "<<e.cost<<"\n";

            for(auto& ne:adj[u])
                if(!vis[ne.dest])
                    pq.insert(ne);
        }

        if(count!=n)
            std::cout<<"Graph disconnected\n";

        std::cout<<"Total Cost "<<total<<"\n";
    }

    // ================= TASK 8 =================
    void kruskal(){
        int n=V.size();
        std::vector<Edge> edges;

        for(int u=0;u<n;u++)
            for(auto& e:adj[u])
                if(u<e.dest)
                    edges.push_back(e);

        // insertion sort
        for(int i=1;i<edges.size();i++){
            Edge k=edges[i];
            int j=i-1;
            while(j>=0 && edges[j].cost>k.cost){
                edges[j+1]=edges[j];
                j--;
            }
            edges[j+1]=k;
        }

        std::vector<int> p(n);
        for(int i=0;i<n;i++) p[i]=i;

        auto find=[&](int x){
            while(p[x]!=x) x=p[x];
            return x;
        };

        int total=0;

        for(auto& e:edges){
            int a=find(e.src),b=find(e.dest);
            if(a!=b){
                p[a]=b;
                total+=e.cost;
                std::cout<<V[e.src].data<<"-"<<V[e.dest].data<<" "<<e.cost<<"\n";
            }
        }

        std::cout<<"Total Cost "<<total<<"\n";
    }
    void print_graph() const {
    std::cout << "Graph Connections:\n";

        for(int i = 0; i < V.size(); i++){
            std::cout << V[i].data << " -> ";

            if(adj[i].empty()){
                std::cout << "None";
            } else {
                for(int j = 0; j < adj[i].size(); j++){
                    int v = adj[i][j].dest;
                    std::cout << V[v].data
                            << "(d=" << adj[i][j].distance
                            << ", c=" << adj[i][j].cost << ")";

                    if(j != adj[i].size()-1) std::cout << ", ";
                }
            }
        std::cout << "\n";
    }
    }
    void shortest_paths_to_state(const T& start, const std::string& state){
    int s = index(start);

    if(s == -1){
        std::cout << "Source not found.\n";
        return;
    }

    int n = V.size();

    std::vector<int> dist(n, INT_MAX);
    std::vector<int> cost(n, 0);
    std::vector<int> parent(n, -1);
    std::vector<bool> vis(n, false);

    dist[s] = 0;

    MinHeap<Edge> pq;
    pq.insert(Edge(-1, s, 0, 0));

    // ===== DIJKSTRA =====
    while(!pq.empty()){
        Edge cur = pq.extract();
        int u = cur.dest;

        if(vis[u]) continue;
        vis[u] = true;

        for(const auto& e : adj[u]){
            int v = e.dest;
            int nd = dist[u] + e.distance;

            if(nd < dist[v]){
                dist[v] = nd;
                cost[v] = cost[u] + e.cost;
                parent[v] = u;
                pq.insert(Edge(u, v, nd, 0));
            }
        }
    }
    

    // ===== OUTPUT =====
    std::cout << "Shortest paths from " << start
              << " to " << state << " state airports are:\n";

    std::cout << std::left << std::setw(40) << "Path"
              << std::setw(10) << "Length"
              << "Cost\n";

    bool found = false;

        for(int i = 0; i < n; i++){
            if(i == s) continue;

            if(getState(V[i].data) != state) continue;
            if(dist[i] == INT_MAX) continue;

            // reconstruct path
            std::vector<int> path;
            for(int v = i; v != -1; v = parent[v])
                path.push_back(v);

            // reverse manually
            for(int l=0,r=path.size()-1; l<r; l++,r--){
                int tmp = path[l];
                path[l] = path[r];
                path[r] = tmp;
            }

            // build string
            std::string pathStr;
            for(int j = 0; j < path.size(); j++){
                if(j) pathStr += "->";
                pathStr += V[path[j]].data;
            }

            std::cout << std::left << std::setw(40) << pathStr
                    << std::setw(10) << dist[i]
                    << cost[i] << "\n";

            found = true;
        }

        if(!found){
            std::cout << "No reachable airports found in state: "
                    << state << "\n";
        }
    }
};


// ================= MAIN =================
int main(){
    Graph<std::string> g;
     std::cout << "\n--- Task 1 ---\n";
    g.load_csv("airports.csv");
     std::cout << "\n--- Task 2 ---\n";
    g.print_graph();

    std::cout << "\n--- Task 3 ---\n";
    g.shortest_paths_to_state("ATL", "GA");
     std::cout << "\n--- Task 4 ---\n";
    g.shortest_path_k_stops("ATL", "MIA", 3);
     std::cout << "\n--- Task 5 ---\n";
    std::cout<<"\nConnections:\n";
    g.connection_count();

    std::cout<<"\nPrim MST:\n";
    Graph<std::string> gu=g.build_undirected();
    gu.prim();

    std::cout<<"\nKruskal MST:\n";
    gu.kruskal();

    return 0;
}

