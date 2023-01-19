#include <bits/stdc++.h>
#include <fstream>

#define file_in "curse.in"
#define file_out "curse.out"

int N, M, A;
std::vector<std::vector<int>> trainings;
std::vector<std::vector<int>> neighs;
std::vector<int> result;

// function that reads the input data
void read() {
    std::ifstream in;
    in.open(file_in);

    in >> N >> M >> A;
    std::vector<int> empty(N, 0);
    trainings.push_back(empty);

    for (int i = 0; i < A; i++) {
        std::vector<int> lap;
        for (int j = 0; j < N; j++) {
            int x;
            in >> x;
            lap.push_back(x);
        }
        trainings.push_back(lap);
    }

    in.close();
}

void initialise_neighbours() {
    std::vector<int> empty;

    for (int i = 0; i <= M; i++) {
        neighs.push_back(empty);
    }
}

void generate_dependencies() {
    // initialise a vector of adjacencies
    initialise_neighbours();

    for (int i = 1; i < A; i++) {
        // look at 2 neighbouring results every time
        std::vector<int> first_train = trainings[i];
        std::vector<int> second_train = trainings[i + 1];

        // check each track's result
        int track = 0;
        while (track < N && first_train[track] == second_train[track]) {
            track++;
        }

        // if we can make a conclusion about 2 cars based on previous results
        if (track != N) {
            neighs[second_train[track]].push_back(first_train[track]);
        }
    }
}

// function taken from the implementation of 7th laboratory in Pa course
void dfs(int node, std::vector<int> &visited) {
    visited[node] = 1;

    for (int i = 0; i < neighs[node].size(); i++) {
        if (visited[neighs[node][i]] != 1) {
            dfs(neighs[node][i], visited);
        }
    }

    result.push_back(node);
}

// driver function of the programme
void solve() {
    // initialise a vector of already rated cars
    std::vector<int> visited(M + 1, 0);

    // create links between cars
    generate_dependencies();

    // start a new DFS search from each unseen car
    for (int car = 1; car <= M; car++) {
        if (visited[car] == 0) {
            dfs(car, visited);
        }
    }
}

// function that prints the outcome
void print() {
    std::ofstream out;
    out.open(file_out);

    for (int i = 0; i < result.size(); i++) {
        out << result[i] << " ";
    }

    out.close();
}

int main() {
    read();
    solve();
    print();

    return 0;
}
