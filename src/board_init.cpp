#include "../includes/board_init.h"

/**
 * Initializes BOARD_INIT with default tiles
 */
void init_board(vector<vector<Tile>>& board) {
    int x = 6; // x is the total number of files in each rank. It changes by one on each adjacent rank.
    int z = 0; // z counts the total number of tiles visited, used for tile color
    // i is the rank number
    for (int i=0; i<11; i++) {
        // j is the file number
        for (int j=0; j<x; j++) {
            // the tile color repeats in a pattern of W > B > G > ...
            switch (z % 3) {
                case 0:
                    board[i].push_back(Tile(colorW, sCoords(i, static_cast<eFiles>(j))));
                    break;
                case 1:
                    board[i].push_back(Tile(colorB, sCoords(i, static_cast<eFiles>(j))));
                    break;
                case 2:
                    board[i].push_back(Tile(colorG, sCoords(i, static_cast<eFiles>(j))));
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
