#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>

#define file_in "fortificatii.in"
#define file_out "fortificatii.out"


int N, M, K, B;
std::set<int> barbarian;
std::vector<std::vector<std::pair<int, int>>> locations;
std::vector<int> distances;
std::vector<std::vector<int>> barbarian_distances;
std::vector<int> all_barbarian_distances;
// function that reads the input data
void read() {
    std::ifstream in;
    in.open(file_in);

    in >> N >> M >> K >> B;

    // initialise locations vector
    locations = std::vector<std::vector<std::pair<int, int>>> (N + 1);
    distances = std::vector<int> (N + 1, INT32_MAX >> 3);

    // read barbarian location
    for (int i = 0; i < B; i++) {
        int x;
        in >> x;
        barbarian.insert(x);
    }

    // read connections
    for (int i = 0; i < M; i++) {
        int x, y, t;
        in >> x >> y >> t;
        locations[x].push_back({y, t});
        locations[y].push_back({x, t});
    }
    in.close();
}

void min_distances() {
    std::vector<int> visited(N + 1, 0);
    distances[1] = 0;
    // initialise a priority_queue that will keep pairs of locations
    // costs sorted in increasing order by cost
    // {Loc, Cost}
    auto cmp = [](std::pair<int, int> left, std::pair<int, int> right) {
        return left.second > right.second;
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                                                        decltype(cmp)> pq(cmp);

    pq.push({1, 0});
    while (!pq.empty()) {
        std::pair<int, int> curr = pq.top();
        pq.pop();
        int curr_loc = curr.first;
        int curr_cost = curr.second;

        // skip if already visited
        if (visited[curr_loc]) {
            continue;
        }

        visited[curr_loc] = 1;

        for (int i = 0; i < locations[curr_loc].size(); i++) {
            // if current neighbour is not a barbarian location
            if (barbarian.find(locations[curr_loc][i].first) == barbarian.end()
                && distances[locations[curr_loc][i].first] >
                        distances[curr_loc] + locations[curr_loc][i].second) {
                distances[locations[curr_loc][i].first] = distances[curr_loc] +
                                                locations[curr_loc][i].second;
                pq.push({locations[curr_loc][i].first,
                                    distances[locations[curr_loc][i].first]});
            }
        }
    }
}

void compute_distances() {
    // compute distances from all free locations to capital
    min_distances();

    // initialise and compute barbarian distances
    std::vector<int> empty;
    for (int i = 0; i <= N; i++) {
        barbarian_distances.push_back(empty);
    }

    // compute all distances of barbarian locations to capital
    for (int i = 1; i <= N; i++) {
        if (barbarian.find(i) != barbarian.end()) {
            for (int j = 0; j < locations[i].size(); j++) {
                if (barbarian.find(locations[i][j].first) == barbarian.end()) {
                    // if current neighbour of a barbarian
                    // location is a free location
                    barbarian_distances[i].push_back(
                        distances[locations[i][j].first] +
                                    locations[i][j].second);
                    all_barbarian_distances.push_back(
                        distances[locations[i][j].first] +
                                    locations[i][j].second);
                }
            }
        }
    }

    // sort the barbarian distances in increasing order
    std::sort(all_barbarian_distances.begin(), all_barbarian_distances.end());
}

int find_minimum_time() {
    int ind = 0, max_efficient_fortified_value = all_barbarian_distances[0] + K;
    unsigned long long sum = 0;
    while (ind < all_barbarian_distances.size() - 1 &&
                all_barbarian_distances[ind] < max_efficient_fortified_value) {
        sum += all_barbarian_distances[ind];
        ind++;
    }

    if (all_barbarian_distances[ind] < max_efficient_fortified_value) {
        sum += all_barbarian_distances[ind];
    }

    if (ind == 0) {
        return max_efficient_fortified_value;
    }

    if (ind == all_barbarian_distances.size() - 1 && sum < K) {
        return (K + sum) / all_barbarian_distances.size();
    }

    std::vector<int> interest_distances;

    for (int i = 0; i <= ind; i++) {
        interest_distances.push_back(all_barbarian_distances[i]);
    }

    while (K) {
        int begin = 0;
        while (begin < ind) {
            if (interest_distances[begin] <= interest_distances[begin + 1]) {
                K--;
                interest_distances[begin]++;
                break;
            }
        }
        std::sort(interest_distances.begin(), interest_distances.end());
    }

    int min_val = interest_distances[0];

    return min_val;
}

int main() {
    std::ofstream out;
    out.open(file_out);

    read();
    compute_distances();
    out << find_minimum_time();

    out.close();
    return 0;
}
