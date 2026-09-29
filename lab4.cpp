#include <bits/stdc++.h>
using namespace std;

const int inf = 1000000000;

vector<vector<int>> ConstructGraph(int n, vector<vector<int>>& edges) {
    vector<vector<int>> graph(n, vector<int>());
    for (int i = 0; i < edges.size(); ++i) {
        graph[edges[i][0]].push_back(edges[i][1]);
    }
    return graph;
}

vector<vector<pair<int, int>>> ConstructReversedGraph(int n, vector<vector<int>>& edges) {
    vector<vector<pair<int, int>>> graph(n, vector<pair<int, int>>());
    for (int i = 0; i < edges.size(); ++i) {
        graph[edges[i][1]].push_back({edges[i][0], edges[i][2]});
    }
    return graph;
}

void DFS(int vertex, vector<vector<int>>& graph, vector<bool>& used, vector<int>& result) {
    for (auto& neighbour : graph[vertex]) {
        if (!used[neighbour]) {
            used[neighbour] = true;
            DFS(neighbour, graph, used, result);
        }
    }
    result.push_back(vertex);
}

vector<int> topsort(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<bool> used(n, false);
    vector<int> result;
    for (int i = 0; i < n; ++i) {
        if (!used[i]) {
            used[i] = true;
            DFS(i, graph, used, result);
        }
    }
    reverse(result.begin(), result.end());
    return result;
}

pair<int, vector<int>> Solve(int n, vector<vector<int>>& edges, int s, int t) {
    auto graph = ConstructGraph(n, edges);
    vector<int> topsorted = topsort(graph);

    int ind_s = find(topsorted.begin(), topsorted.end(), s) - topsorted.begin();
    int ind_t = find(topsorted.begin(), topsorted.end(), t) - topsorted.begin();

    if (ind_t < ind_s) {
        return {-inf, {}};
    }

    auto reversed_graph = ConstructReversedGraph(n, edges);

    vector<int> opt(n, -inf);
    vector<int> prev(n, -1);
    opt[s] = 0;

    for (int ind = ind_s + 1; ind <= ind_t; ++ind) {
        int best_v = -1;
        int best_dist = -inf;
        for (auto& [neighbour, length] : reversed_graph[topsorted[ind]]) {
            if (opt[neighbour] != -inf) {
                int dist = opt[neighbour] + length;
                if (dist > best_dist) {
                    best_dist = dist;
                    best_v = neighbour;
                }
            }
        }
        opt[topsorted[ind]] = best_dist;
        prev[topsorted[ind]] = best_v;
    }

    if (opt[t] == -inf) {
        return {-inf, {}};
    }

    vector<int> path;
    int curr = t;
    while (curr != -1) {
        path.push_back(curr);
        curr = prev[curr];
    }
    reverse(path.begin(), path.end());
    return {opt[t], path};
}

int main() {
    int n = 7;
    vector<vector<int>> edges = {
        {1, 0, 1},
        {6, 0, 4},
        {2, 6, 2},
        {5, 0, 1},
        {4, 0, 3},
        {5, 1, 2},
        {2, 5, 2},
        {4, 5, 1},
        {3, 4, 1}
    };
    int s = 2;
    int t = 0;

    auto [length, path] = Solve(n, edges, s, t);
    cout << length << '\n';
    for (auto& el : path) {
        cout << el << " ";
    }

    return 0;
}
