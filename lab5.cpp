#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

class FordFalkerson {
public:
    FordFalkerson(const std::vector<std::vector<std::pair<int, int>>>& graph) {
        size_ = graph.size();
        graph_.resize(size_);
        capacity_.assign(size_, std::vector<int>(size_, 0));
        for (size_t i = 0; i < size_; ++i) {
            for (const auto& [vert, cap] : graph[i]) {
                graph_[i].push_back(vert);
                capacity_[i][vert] = cap;
            }
        }
    }

    std::pair<int, std::vector<std::pair<int, int>>> FindMaxMatching(int source, int target, int left, int right) {
        int num = 0;

        while (true) {
            auto [path, exists] = FindOnesPath(source, target);
            if (!exists) {
                break;
            }

            num += 1;
            for (int i = 1; i < static_cast<int>(path.size()); ++i) {
                int prev = path[i - 1];
                int curr = path[i];
                capacity_[prev][curr] = 0;
                capacity_[curr][prev] = 1;
            }
        }

        std::vector<std::pair<int, int>> matching;
        for (int i = 1; i <= left; ++i) {
            for (int j = left + 1; j <= left + right; ++j) {
                if (capacity_[j][i] == 1) {
                    matching.push_back({i, j - left});
                }
            }
        }
        return {num, matching};
    }

private:
    std::vector<std::vector<int>> graph_;
    std::vector<std::vector<int>> capacity_;
    size_t size_;

    std::pair<std::vector<int>, bool> FindOnesPath(int source,
                                                      int target) const {
        std::vector<int> prev(graph_.size(), -1);
        std::queue<int> queue;
        queue.push(source);

        while (!queue.empty()) {
            int vertex = queue.front();
            queue.pop();

            for (const auto& neighbour : graph_[vertex]) {
                int cap = capacity_[vertex][neighbour];
                if (prev[neighbour] != -1 || cap == 0) {
                    continue;
                }

                prev[neighbour] = vertex;
                queue.push(neighbour);

                if (neighbour == target) {
                    break;
                }
            }
        }

        if (prev[target]== -1) {
            return {{}, false};
        }

        std::vector<int> path;
        path.push_back(target);
        int cur = target;
        while (cur != source) {
            int previous = prev[cur];
            path.push_back(previous);
            cur = previous;
        }
        std::reverse(path.begin(), path.end());

        return {path, true};
    }
};


int main() {
    int left = 3;
    int right = 3;
    std::vector<std::pair<int, int>> edges = {
        {1, 1},
        {2, 2},
        {3, 3},
        {2, 1},
        {3, 1},
        {3, 2}
    };

    std::vector<std::vector<std::pair<int, int>>> graph(left + right + 2);
    for (int i = 1; i <= left; ++i) {
        graph[0].push_back({i, 1});
        graph[i].push_back({0, 0});
    }
    for (const auto& [v1, v2] : edges) {
        graph[v1].push_back({left + v2, 1});
        graph[left + v2].push_back({v1, 0});
    }
    for (int i = left + 1; i <= left + right; ++i) {
        graph[i].push_back({left + right + 1, 1});
        graph[left + right + 1].push_back({i, 0});
    }

    FordFalkerson ff(graph);
    auto [num, max_matching] = ff.FindMaxMatching(0, left + right + 1, left, right);
    std::cout << num << '\n';
    for (const auto& [v1, v2] : max_matching) {
        std::cout << v1 << ' ' << v2 << '\n';
    }

    return 0;
}
