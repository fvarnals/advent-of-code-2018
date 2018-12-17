#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

const int GRID_SIZE = 150;

enum Facing {
    UP, RIGHT, DOWN, LEFT
};

Facing charToFacing(char cart) {
    if (cart == '<')    return LEFT;
    if (cart == '^')    return UP;
    if (cart == '>')    return RIGHT;
    if (cart == 'v')    return DOWN;
    exit(42);
}

enum Track {
    NONE, INTERSECTION, H, V, SLASH, BACKSLASH
};

Track charToTrack(char c) {
    switch (c) {
        case ' ':   return NONE;
        case '|': case 'v': case '^':   return V;
        case '-': case '<': case '>':   return H;
        case '\\':  return BACKSLASH;
        case '/':   return SLASH;
        case '+':   return INTERSECTION;
    }
    exit(37);
}

struct Cart {
    int x, y;
    Facing facing;
    Facing nextIntersection = LEFT;     // Pretend UP is 'go straight'

    // Order carts by x, y coords
    bool operator<(const Cart& b) const {
        return y < b.y || (y == b.y && x < b.x);
    }

    void move() {
        switch (facing) {
            case LEFT:  x--; break;
            case RIGHT: x++; break;
            case UP:    y--; break;
            case DOWN:  y++; break;
        }
    }

    void updateFacing(Track track) {
        switch (track) {
            case SLASH:
                switch (facing) {
                    case UP:    facing = RIGHT; break;
                    case RIGHT: facing = UP; break;
                    case DOWN:  facing = LEFT; break;
                    case LEFT:  facing = DOWN; break;
                }
                break;
            case BACKSLASH:
                switch (facing) {
                    case UP:    facing = LEFT; break;
                    case RIGHT: facing = DOWN; break;
                    case DOWN:  facing = RIGHT; break;
                    case LEFT:  facing = UP; break;
                }
                break;
            case INTERSECTION:
                switch (nextIntersection) {
                    case LEFT:
                        switch (facing) {
                            case UP:    facing = LEFT; break;
                            case RIGHT: facing = UP; break;
                            case DOWN:  facing = RIGHT; break;
                            case LEFT:  facing = DOWN; break;
                        }
                        nextIntersection = UP;
                        break;
                    case UP:
                        nextIntersection = RIGHT;
                        break;
                    case RIGHT:
                        switch (facing) {
                            case UP:    facing = RIGHT; break;
                            case RIGHT: facing = DOWN; break;
                            case DOWN:  facing = LEFT; break;
                            case LEFT:  facing = UP; break;
                        }
                        nextIntersection = LEFT;
                    default:
                        break;
                }
            default:
                break;
        }
    }
};

struct State {
    array<array<Track, GRID_SIZE>, GRID_SIZE> tracks;
    vector<Cart> carts;
};

State parse(const string& in) {
    State state;
    istringstream iss(in);
    int y = 0;
    for (string line; getline(iss, line); y++) {
        for (int x = 0; x < line.length(); x++) {
            char c = line[x];
            state.tracks[y][x] = charToTrack(c);
            if (c == 'v' || c == '^' || c == '<' || c == '>')
                state.carts.push_back({x, y, charToFacing(c)});
        }
    }
    return state;
}

string run(const string& s) {
    auto state = parse(s);

    while (state.carts.size() > 1) {
        for (auto& cart : state.carts) {
            // Do not update if we crashed
            if (crashed.find(cart) != crashed.end())
                continue;
            
            // create a copy
            auto tmp = cart;
            tmp.move();

            // Check for collisions
            for (auto& c2 : state.carts) {
                if (c2.x == tmp.x && c2.y == tmp.y) {
                    crashed.insert(tmp);
                    crashed.insert(c2);
                    continue;
                }
            }

            cart = tmp;
            Track tile = state.tracks[tmp.y][tmp.x];
            tmp.updateFacing(tile);
        }
    }

    auto cart = *state.carts.begin();
    return to_string(cart.x) + ',' + to_string(cart.y);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Missing one argument" << endl;
        exit(1);
    }

    clock_t start = clock();
    auto answer = run(string(argv[1]));
    
    cout << "_duration:" << float( clock () - start ) * 1000.0 /  CLOCKS_PER_SEC << "\n";
    cout << answer << "\n";
    return 0;
}
