#include <bits/stdc++.h>
#include <fstream>

#define file_in "curatare.in"
#define file_out "curatare.out"

std::vector<std::vector<char>> matrix;
std::vector<std::pair<int, int>> robots;
std::vector<std::pair<int, int>> spaces;
std::vector<std::vector<bool>> visited;

std::vector<std::vector<int>> robots_distance;
std::vector<std::vector<int>> spaces_distance;
int N, M, R, S;

// function that reads the input data, count the number of robots and
// cleaning spaces and initializes matrix of available to go cells
void read() {
    std::ifstream in;
    in.open(file_in);

    in >> N >> M;

    for (int i = 0; i < N; i++) {
        std::vector<char> line;
        std::vector<bool> visitable;
        for (int j = 0; j < M; j++) {
            char r;
            in >> r;
            line.push_back(r);

            if (r == 'R') {
                R++;
                robots.push_back({i, j});
            }
            if (r == 'S') {
                S++;
                spaces.push_back({i, j});
            }
            if (r == 'X') {
                visitable.push_back(true);
            } else {
                visitable.push_back(false);
            }
        }
        matrix.push_back(line);
        visited.push_back(visitable);
    }

    in.close();
}

// function that checks if a location is inside the borders
// true -> inside borders
// false -> outside borders
bool check_borders(int x, int y) {
    if (x < 0 || x >= N) {
        return false;
    }

    if (y < 0 || y >= M) {
        return false;
    }

    return true;
}

// get the index of a robot based on its location
int robot_index(int x, int y) {
    for (int i = 0; i < R; i++) {
        if (robots[i].first == x && robots[i].second == y) {
            return i;
        }
    }

    return -1;
}

// gets the index of a dirty space based on its location
int space_index(int x, int y) {
    for (int i = 0; i < S; i++) {
        if (spaces[i].first == x && spaces[i].second == y) {
            return i;
        }
    }

    return -1;
}

void distance_between(int space_ind, std::pair<int, int> start,
                    std::vector<std::vector<bool>> visited_copy) {
    // queue that stores pairs of locations and distance from target in BFS
    std::queue<std::pair<std::pair<int, int>, int>> q;
    q.push({start, 0});

    // flag that stops the loop when all elements have been discovered
    int stop = 0;

    while (!q.empty() && stop != R + S) {
        // extract current element and its fields
        std::pair<std::pair<int, int>, int> curr_elem = q.front();
        q.pop();
        int x = curr_elem.first.first;
        int y = curr_elem.first.second;
        int distance = curr_elem.second;

        // mark cell as visited
        visited_copy[x][y] = true;

        // found a robot
        if (matrix[x][y] == 'R') {
            int index = robot_index(x, y);
            robots_distance[index][space_ind] = distance;
            stop++;
        }

        // found a space
        if (matrix[x][y] == 'S') {
            int index = space_index(x, y);
            spaces_distance[space_ind][index] = distance;
            stop++;
        }

        // add neighbours to queue with increased distance
        // North
        if (check_borders(x - 1, y) && !visited_copy[x - 1][y]) {
            q.push({{x - 1, y}, distance + 1});
            visited_copy[x - 1][y] = true;
        }
        // South
        if (check_borders(x + 1, y) && !visited_copy[x + 1][y]) {
            q.push({{x + 1, y}, distance + 1});
            visited_copy[x + 1][y] = true;
        }
        // West
        if (check_borders(x, y - 1) && !visited_copy[x][y - 1]) {
            q.push({{x, y - 1}, distance + 1});
            visited_copy[x][y - 1] = true;
        }
        // East
        if (check_borders(x, y + 1) && !visited_copy[x][y + 1]) {
            q.push({{x, y + 1}, distance + 1});
            visited_copy[x][y + 1] = true;
        }
    }
}

// function that computes important distances in matrix
void compute_distances() {
    // initialise distance vectors for robots and spaces
    std::vector<int> spaces_ind(S);
    for (int i = 0; i < R; i++) {
        robots_distance.push_back(spaces_ind);
    }
    for (int i = 0; i < S; i++) {
        spaces_distance.push_back(spaces_ind);
    }

    for (int i = 0; i < S; i++) {
        distance_between(i, spaces[i], visited);
    }
}

// function that sorts the locations a robot has to clean in order to
// achieve the smallest travelled distance
std::vector<int> sort_locations(int robot, std::vector<int> spaces) {
    std::vector<int> sorted_spaces;
    int min_distance = N * M, min_index = -1, value = -1;
    for (int i = 0; i < spaces.size(); i++) {
        if (robots_distance[robot][spaces[i]] < min_distance) {
            min_index = i;
            value = spaces[i];
            min_distance = robots_distance[robot][spaces[i]];
        }
    }
    sorted_spaces.push_back(spaces[min_index]);
    spaces.erase(spaces.begin() + min_index);

    while (spaces.size() > 0) {
        min_distance = N * M, min_index = -1;
        for (int i = 0; i < spaces.size(); i++) {
            if (spaces_distance[sorted_spaces[
                    sorted_spaces.size() - 1]][spaces[i]] < min_distance) {
                min_index = i;
                min_distance = spaces_distance[sorted_spaces[
                                    sorted_spaces.size() - 1]][spaces[i]];
            }
        }
        sorted_spaces.push_back(spaces[min_index]);
        spaces.erase(spaces.begin() + min_index);
    }

    return sorted_spaces;
}

// function that computes the time it takes a robot to clean a set of spaces
int robot_cleans(int robot, std::vector<int> spaces) {
    if (spaces.size() == 0) {
        return 0;
    }

    std::vector<int> sorted_spaces = sort_locations(robot, spaces);

    int time = 0;
    time += robots_distance[robot][sorted_spaces[0]];
    for (int i = 1; i < sorted_spaces.size(); i++) {
        time += spaces_distance[sorted_spaces[i - 1]][sorted_spaces[i]];
    }

    return time;
}

// function that adds a new element to an existent partition
std::vector<std::vector<std::vector<int>>> partition(int elem,
            std::vector<std::vector<std::vector<int>>> &parts) {
    std::vector<int> fst(1, elem);
    std::vector<std::vector<std::vector<int>>> all_parts;
    if (parts.empty()) {
        std::vector<std::vector<int>> part1;
        part1.push_back(fst);
        all_parts.push_back(part1);
    } else {
        for (int i = 0; i < parts.size(); i++) {
            std::vector<std::vector<int>> curr_part = parts[i];
            curr_part.insert(curr_part.begin(), fst);
            all_parts.push_back(curr_part);
        }

        for (int i = 0; i < parts.size(); i++) {
            for (int j = 0; j < parts[i].size(); j++) {
                std::vector<std::vector<int>> curr_part = parts[i];
                curr_part[j].insert(curr_part[j].begin(), elem);
                all_parts.push_back(curr_part);
            }
        }
    }

    return all_parts;
}

// function that keeps the partitions with number of elements less or equal
// than the number of robots
std::vector<std::vector<std::vector<int>>> trim_partitions(
        std::vector<std::vector<std::vector<int>>> &parts) {
    std::vector<std::vector<std::vector<int>>> all_parts;
    for (int i = 0; i < parts.size(); i++) {
        if (parts[i].size() <= R) {
            all_parts.push_back(parts[i]);
        }
    }
    return all_parts;
}

// function that recursively generates partitions of cleaning spaces
std::vector<std::vector<std::vector<int>>> generate_partitions() {
    std::vector<std::vector<std::vector<int>>> parts;
    for (int i = 0; i < S; i++) {
        parts = partition(i, parts);
    }
    parts = trim_partitions(parts);
    return parts;
}

// driver function that finds the minimum time it takes all robots
// to clean a set of spaces
int share_tasks() {
    int min_time = M * N;
    std::vector<int> empty;
    std::vector<int> robots(R);

    // generate all possible ways the robots can clean the house
    std::vector<std::vector<std::vector<int>>> parts = generate_partitions();
    for (int i = 0; i < parts.size(); i++) {
        // for each particular possibility, swap places of the robots
        std::vector<std::vector<int>> curr_part = parts[i];
        std::iota(robots.begin(), robots.end(), 0);

        // if there are more robots than partitions of cleaning spaces
        // add empty lists
        while (curr_part.size() < R) {
            curr_part.push_back(empty);
        }

        // permute ways of assigning robots to spaces
        do {
            int lap_time = 0;
            for (int r = 0; r < robots.size(); r++) {
                int time = robot_cleans(robots[r], curr_part[r]);
                if (time > lap_time) {
                    lap_time = time;
                }
            }
            if (min_time > lap_time) {
                min_time = lap_time;
            }
        } while (std::next_permutation(robots.begin(), robots.end()));
    }

    return min_time;
}

int main() {
    std::ofstream out;
    out.open(file_out);

    read();
    compute_distances();
    out << share_tasks();

    out.close();
    return 0;
}
