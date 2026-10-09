#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>


const int kInf = 1'000'000;
const double kEps = 0.000001;


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

    std::pair<std::vector<int>, std::vector<int>> FindAllReachable(int source, int left, int right) {
        std::vector<bool> reachable(left + right + 2, false);
        std::queue<int> queue;
        queue.push(source);
        reachable[source] = true;

        while (!queue.empty()) {
            int vertex = queue.front();
            queue.pop();

            for (const auto& neighbour : graph_[vertex]) {
                int cap = capacity_[vertex][neighbour];
                if (cap == 0 || reachable[neighbour]) {
                    continue;
                }

                queue.push(neighbour);
                reachable[neighbour] = true;
            }
        }

        std::vector<int> left_reachable;
        std::vector<int> right_reachable;
        for (int i = 1; i <= left; ++i) {
            if (reachable[i]) {
                left_reachable.push_back(i);
            }
        }
        for (int i = 1; i <= right; ++i) {
            if (reachable[left + i]) {
                right_reachable.push_back(i);
            }
        }

        return {left_reachable, right_reachable};
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


std::vector<std::pair<int, int>> ConstructJ(const std::vector<std::vector<int>>& C,
                                            const std::vector<double>& alpha,
                                            const std::vector<double>& beta) {
    int n = C.size();
    std::vector<std::pair<int, int>> J;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (std::fabs(alpha[i] + beta[j] - (double)C[i][j]) < kEps) {
                J.push_back({i + 1, j + 1});
            }
        }
    }

    return J;
}


std::vector<std::pair<int, int>> Solve(const std::vector<std::vector<int>>& C) {
    // step 1
    int n = C.size();
    std::vector<double> alpha(n, 0.0);
    std::vector<double> beta(n, (double)kInf);

    for (int b = 0; b < n; ++b) {
        for (int i = 0; i < n; ++i) {
            if ((double)C[i][b] < beta[b]) {
                beta[b] = (double)C[i][b];
            }
        }
    }

    while (true) {
        // step 2
        std::vector<std::pair<int, int>> J = ConstructJ(C, alpha, beta);
    
        // step 3 && step 4
        std::vector<std::vector<std::pair<int, int>>> graph(2 * n + 2, std::vector<std::pair<int, int>>());
        for (int i = 1; i <= n; ++i) {
            graph[0].push_back({i, 1});
            graph[i].push_back({0, 0});
        }
        for (const auto& [v1, v2] : J) {
            graph[v1].push_back({n + v2, 1});
            graph[n + v2].push_back({v1, 0});
        }
        for (int i = n + 1; i <= 2 * n; ++i) {
            graph[i].push_back({2 * n + 1, 1});
            graph[2 * n + 1].push_back({i, 0});
        }
        FordFalkerson ff(graph);
        auto [num, matching] = ff.FindMaxMatching(0, 2 * n + 1, n, n);
    
        // step 5
        if (num == n) {
            return matching;
        }
    
        // step 6 && step 7
        auto [I_so_zviozdochkoi, J_so_zviozdochkoi] = ff.FindAllReachable(0, n, n);
    
        // step 8
        std::vector<int> alpha_s_kryshkoi(n, -1);
        std::vector<int> beta_s_kryshkoi(n, 1);
        for (int vertex : I_so_zviozdochkoi) {
            alpha_s_kryshkoi[vertex - 1] = 1;
        }
        for (int vertex: J_so_zviozdochkoi) {
            beta_s_kryshkoi[vertex - 1] = -1;
        }
    
        // step 9
        double min_tetha = (double)kInf;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                // i is from I_so_zviozdochkoi
                // j is not from J_so_zviozdochkoi
                if (alpha_s_kryshkoi[i] == 1 && beta_s_kryshkoi[j] == 1) {
                    double tetha = ((double)C[i][j] - alpha[i] - beta[j]) / 2.0;
                    min_tetha = std::min(min_tetha, tetha);
                }
            }
        }
    
        // step 10 && step 11
        for (int i = 0; i < n; ++i) {
            alpha[i] += min_tetha * alpha_s_kryshkoi[i];
            beta[i] += min_tetha * beta_s_kryshkoi[i];
        }
    }
}


int main() {
    std::vector<std::vector<int>> C = {
        {7, 2, 1, 9, 4},
        {9, 6, 9, 5, 5},
        {3, 8, 3, 1, 8},
        {7, 9, 4, 2, 2},
        {8, 4, 7, 4, 8}
    };

    std::vector<std::pair<int, int>> pos = Solve(C);
    for (const auto& [x, y] : pos) {
        std::cout << x << " " << y << '\n';
    }

    return 0;
}
