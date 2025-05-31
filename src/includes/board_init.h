#ifndef BOARD_INIT_H
#define BOARD_INIT_H
#include "./tile.h"
#include <vector>
using namespace std;

/**
 * Initializes BOARD_INIT with default tiles
 */
void init_board(vector<vector<Tile>>& board);
#endif

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
