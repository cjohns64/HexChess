#ifndef TILE_H
#define TILE_H
#include "util.h"
#include "chesspiece.h"

// Contains information and functions associated with a single tile.
class Tile {
private:
    ChessPiece* current_piece;
    sCoords position;

public:
    eColor tile_color;
    Tile(eColor tile_color, sCoords location);
    sCoords GetLocation();

    /**
     * Places the given piece on this tile, updating current_piece with the reference to the piece.
     */
    void SetPiece(ChessPiece* piece);

    /**
     * Retrieves the piece reference located on the tile, if any.
     */
    ChessPiece* GetPiece();

    /**
     * Clear the piece on tile, use SetPiece to replace the piece if possible.
     */
    void RemovePiece();
};
#endif
