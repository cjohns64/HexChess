#ifndef BOARD_H
#define BOARD_H
#include "tile.h"
#include <vector>
using namespace std;


class Board {
private:
    // data structure representing the board
    // tile sequence is White, Black, Grey
    std::vector<std::vector<Tile>> chessboard;

    /**
     * Initializes the chessboard with default tiles
     */
    void init_board();

    /**
     * Checks if the given location is on the board
     */
    bool IsOnBoard(sCoords location);

public:
    Board();

    /**
     * Retrieves a reference to the tile at the given coordinates. Checks if the tile is on the board first.
     * Returns a pointer to the tile or a null pointer if the tile is not on the board.
     */
    Tile* GetTile(sCoords location);

    /**
     * Set the board to the state given by a list of pieces for both players, each piece will have its current location.
     */
    void SetBoard(std::vector<ChessPiece>& white_pieces, KingPiece& white_king, std::vector<ChessPiece>& black_pieces, KingPiece& black_king);
};
#endif
