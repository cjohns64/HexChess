#include "./tile.cpp"
#include <vector>
using namespace std;

vector<vector<Tile>> BOARD_INIT(11,vector<Tile>());

/**
 * Initializes BOARD_INIT with default tiles
 */
void init_board() {
    int x = 6; // x is the total number of files in each rank. It changes by one on each adjacent rank.
    int z = 0; // z counts the total number of tiles visited, used for tile color
    // i is the rank number
    for (int i=0; i<11; i++) {
        // j is the file number
        for (int j=0; j<x; j++) {
            // the tile color repeats in a pattern of W > B > G > ...
            switch (z % 3) {
                case 0:
                    BOARD_INIT[i].push_back(Tile(colorW, sCoords(i, static_cast<eFiles>(j))));
                    break;
                case 1:
                    BOARD_INIT[i].push_back(Tile(colorB, sCoords(i, static_cast<eFiles>(j))));
                    break;
                case 2:
                    BOARD_INIT[i].push_back(Tile(colorG, sCoords(i, static_cast<eFiles>(j))));
                    break;
            }
            // tile compleat, increase z
            z += 1;
        }
        // rank compleat, addjust x.
        // x  increases by one per rank up to the center, then decreases by one per rank. 
        if (i >= 5) {
            x -= 1;
        }
        else {
            x += 1;
        }
    }
}

// manually defined board, placed here since it is too big (91 tiles).
// vector<vector<Tile>> BOARD_INIT = 
// {{
//     Tile(colorW, sCoords(0, a)), Tile(colorB, sCoords(0, b)), 
//     Tile(colorG, sCoords(0, c)), Tile(colorW, sCoords(0, d)), 
//     Tile(colorB, sCoords(0, e)), Tile(colorG, sCoords(0, f)) 
//     },
//     {
//     Tile(colorW, sCoords(1, a)), Tile(colorB, sCoords(1, b)), 
//     Tile(colorG, sCoords(1, c)), Tile(colorW, sCoords(1, d)), 
//     Tile(colorB, sCoords(1, e)), Tile(colorG, sCoords(1, f)), 
//     Tile(colorW, sCoords(1, g))
//     },
//     {
//     Tile(colorB, sCoords(2, a)), 
//     Tile(colorG, sCoords(2, b)), Tile(colorW, sCoords(2, c)), 
//     Tile(colorB, sCoords(2, d)), Tile(colorG, sCoords(2, e)), 
//     Tile(colorW, sCoords(2, f)), Tile(colorB, sCoords(2, g)), 
//     Tile(colorG, sCoords(2, h))
//     },
//     {
//     Tile(colorW, sCoords(3, a)), 
//     Tile(colorB, sCoords(3, b)), Tile(colorG, sCoords(3, c)), 
//     Tile(colorW, sCoords(3, d)), Tile(colorB, sCoords(3, e)), 
//     Tile(colorG, sCoords(3, f)), Tile(colorW, sCoords(3, g)), 
//     Tile(colorB, sCoords(3, h)), Tile(colorG, sCoords(3, i))
//     },
//     {
//     Tile(colorW, sCoords(4, a)), Tile(colorB, sCoords(4, b)), 
//     Tile(colorG, sCoords(4, c)), Tile(colorW, sCoords(4, d)), 
//     Tile(colorB, sCoords(4, e)), Tile(colorG, sCoords(4, f)), 
//     Tile(colorW, sCoords(4, g)), Tile(colorB, sCoords(4, h)), 
//     Tile(colorG, sCoords(4, i)), Tile(colorW, sCoords(4, j)) 
//     },
//     {
//     Tile(colorB, sCoords(5, a)), Tile(colorG, sCoords(5, b)), 
//     Tile(colorW, sCoords(5, c)), Tile(colorB, sCoords(5, d)), 
//     Tile(colorG, sCoords(5, e)), Tile(colorW, sCoords(5, f)), 
//     Tile(colorB, sCoords(5, g)), Tile(colorG, sCoords(5, h)), 
//     Tile(colorW, sCoords(5, i)), Tile(colorB, sCoords(5, j)), 
//     Tile(colorG, sCoords(5, k))
//     },
//     {
//     Tile(colorW, sCoords(6, a)), 
//     Tile(colorB, sCoords(6, b)), Tile(colorG, sCoords(6, c)), 
//     Tile(colorW, sCoords(6, d)), Tile(colorB, sCoords(6, e)), 
//     Tile(colorG, sCoords(6, f)), Tile(colorW, sCoords(6, g)), 
//     Tile(colorB, sCoords(6, h)), Tile(colorG, sCoords(6, i)), 
//     Tile(colorW, sCoords(6, j))
//     },
//     {
//     Tile(colorB, sCoords(7, a)), 
//     Tile(colorG, sCoords(7, b)), Tile(colorW, sCoords(7, c)), 
//     Tile(colorB, sCoords(7, d)), Tile(colorG, sCoords(7, e)), 
//     Tile(colorW, sCoords(7, f)), Tile(colorB, sCoords(7, g)), 
//     Tile(colorG, sCoords(7, h)), Tile(colorW, sCoords(7, i)) 
//     },
//     {
//     Tile(colorB, sCoords(8, a)), Tile(colorG, sCoords(8, b)), 
//     Tile(colorW, sCoords(8, c)), Tile(colorB, sCoords(8, d)), 
//     Tile(colorG, sCoords(8, e)), Tile(colorW, sCoords(8, f)), 
//     Tile(colorB, sCoords(8, g)), Tile(colorG, sCoords(8, h))
//     },
//     {
//     Tile(colorW, sCoords(9, a)), Tile(colorB, sCoords(9, b)), 
//     Tile(colorG, sCoords(9, c)), Tile(colorW, sCoords(9, d)), 
//     Tile(colorB, sCoords(9, e)), Tile(colorG, sCoords(9, f)), 
//     Tile(colorW, sCoords(9, g))
//     },
//     {
//     Tile(colorB, sCoords(10, a)), 
//     Tile(colorG, sCoords(10, b)), Tile(colorW, sCoords(10, c)), 
//     Tile(colorB, sCoords(10, d)), Tile(colorG, sCoords(10, e)), 
//     Tile(colorW, sCoords(10, f))
//     }};
