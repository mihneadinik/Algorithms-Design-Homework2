#include <fstream>
#include <iostream>
#include <vector>
#include <queue>

#define file_in "beamdrone.in"
#define file_out "beamdrone.out"

#define NORTH 'n'
#define SOUTH 's'
#define WEST 'w'
#define EAST 'e'
#define ANY 'a'  // used for starting position

std::vector<std::vector<char>> matrix;
std::vector<std::vector<int>> costs;
std::vector<std::vector<int>> visitedN;
std::vector<std::vector<int>> visitedS;
std::vector<std::vector<int>> visitedW;
std::vector<std::vector<int>> visitedE;
int N, M, Xi, Yi, Xf, Yf;

// function that reads the input data
void read() {
    std::ifstream in;
    in.open(file_in);

    in >> N >> M >> Xi >> Yi >> Xf >> Yf;

    for (int i = 0; i < N; i++) {
        std::vector<char> line;
        for (int j = 0; j < M; j++) {
            char r;
            in >> r;
            line.push_back(r);
        }
        matrix.push_back(line);
    }

    in.close();
}

void initialise_visited() {
    for (int i = 0; i < N; i++) {
        std::vector<int> line(M, INT32_MAX);
        costs.push_back(line);
    }
    for (int i = 0; i < N; i++) {
        std::vector<int> line(M, 0);
        visitedN.push_back(line);
        visitedS.push_back(line);
        visitedW.push_back(line);
        visitedE.push_back(line);
    }
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

// function that checks if a location is in a wall position
// true -> it is a wall
// false -> it is not a wall
bool check_wall(int x, int y) {
    if (matrix[x][y] == 'W') {
        return true;
    }

    return false;
}

// function that checks if a location is the destination position
// true -> it is
// false -> it is not
bool check_final_destination(int x, int y) {
    if (x == Xf && y == Yf) {
        return true;
    }

    return false;
}

// function that computes the minimum distance to target
int compute_min() {
    // initialise a priority_queue that will keep pairs of locations
    // and directions with costs sorted in increasing order by cost
    // {{{posX, posY}, Dir}, Cost}
    auto cmp = [](std::pair<std::pair<std::pair<int, int>, char>, int> left,
                    std::pair<std::pair<std::pair<int, int>, char>, int> right)
                    { return left.second > right.second; };

    std::priority_queue<std::pair<std::pair<std::pair<int, int>, char>, int>,
            std::vector<std::pair<std::pair<std::pair<int, int>, char>, int>>,
                decltype(cmp)> q(cmp);

    // add starting position
    q.push({{{Xi, Yi}, ANY}, 0});
    costs[Xi][Yi] = 0;
    while (!q.empty()) {
        char no_go = ANY;
        // extract the smallest cost in the queue
        std::pair<std::pair<std::pair<int, int>, char>, int> curr = q.top();
        q.pop();
        int curr_x = curr.first.first.first;
        int curr_y = curr.first.first.second;
        char prev_dir = curr.first.second;
        int curr_cost = curr.second;

        // visited before but with a bigger cost now => skip
        if (curr_x != Xi && curr_y != Yi && costs[curr_x][curr_y] < curr_cost) {
            continue;
        }

        // update location cost
        costs[curr_x][curr_y] = std::min(curr_cost, costs[curr_x][curr_y]);

        // check if it is the final destination
        if (check_final_destination(curr_x, curr_y)) {
            return costs[curr_x][curr_y];
        }

        // don't turn 180 degrees
        if (prev_dir == NORTH) {
            no_go = SOUTH;
        }
        if (prev_dir == SOUTH) {
            no_go = NORTH;
        }
        if (prev_dir == EAST) {
            no_go = WEST;
        }
        if (prev_dir == WEST) {
            no_go = EAST;
        }

        // go north all the column
        if (no_go != NORTH) {
            int updated_cost;
            int next_x = curr_x - 1, next_y = curr_y;
            if (prev_dir == ANY || prev_dir == NORTH) {
                updated_cost = curr_cost;
            } else {
                updated_cost = curr_cost + 1;
            }

            while (check_borders(next_x, next_y) && !check_wall(next_x, next_y)
                        && visitedN[next_x][next_y] == 0
                        && costs[next_x][next_y] >= updated_cost) {
                q.push({{{next_x, next_y}, NORTH}, updated_cost});
                costs[next_x][next_y] = updated_cost;
                visitedN[next_x][next_y] = 1;
                next_x = next_x - 1;
            }
        }

        // go south all the column
        if (no_go != SOUTH) {
            int updated_cost;
            int next_x = curr_x + 1, next_y = curr_y;
            if (prev_dir == ANY || prev_dir == SOUTH) {
                updated_cost = curr_cost;
            } else {
                updated_cost = curr_cost + 1;
            }

            while (check_borders(next_x, next_y) && !check_wall(next_x, next_y)
                        && visitedS[next_x][next_y] == 0
                        && costs[next_x][next_y] >= updated_cost) {
                q.push({{{next_x, next_y}, SOUTH}, updated_cost});
                costs[next_x][next_y] = updated_cost;
                visitedS[next_x][next_y] = 1;
                next_x = next_x + 1;
            }
        }

        // go west all the row
        if (no_go != WEST) {
            int updated_cost;
            int next_x = curr_x, next_y = curr_y - 1;
            if (prev_dir == ANY || prev_dir == WEST) {
                updated_cost = curr_cost;
            } else {
                updated_cost = curr_cost + 1;
            }

            while (check_borders(next_x, next_y) && !check_wall(next_x, next_y)
                        && visitedW[next_x][next_y] == 0
                        && costs[next_x][next_y] >= updated_cost) {
                q.push({{{next_x, next_y}, WEST}, updated_cost});
                costs[next_x][next_y] = updated_cost;
                visitedW[next_x][next_y] = 1;
                next_y = next_y - 1;
            }
        }

        // go east all the row
        if (no_go != EAST) {
            int updated_cost;
            int next_x = curr_x, next_y = curr_y + 1;
            if (prev_dir == ANY || prev_dir == EAST) {
                updated_cost = curr_cost;
            } else {
                updated_cost = curr_cost + 1;
            }

            while (check_borders(next_x, next_y) && !check_wall(next_x, next_y)
                        && visitedE[next_x][next_y] == 0
                        && costs[next_x][next_y] >= updated_cost) {
                q.push({{{next_x, next_y}, EAST}, updated_cost});
                costs[next_x][next_y] = updated_cost;
                visitedE[next_x][next_y] = 1;
                next_y = next_y + 1;
            }
        }
    }

    return -1;
}

int main() {
    std::ofstream out;
    out.open(file_out);

    read();
    initialise_visited();
    int cost = compute_min();
    out << cost;

    out.close();
    return 0;
}
