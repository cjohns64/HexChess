#include "includes/tile.h"

// Contains information and functions associated with a single tile.
Tile::Tile(eColor tile_color, sCoords location): position(location.rank, location.file)  {
    tile_color = tile_color;
    current_piece = nullptr;
}

sCoords Tile::GetLocation() {
    return position;
}

/**
 * Places the given piece on this tile, updating current_piece with the reference to the piece.
 */
void Tile::SetPiece(ChessPiece* piece) {
    current_piece = piece;
}

/**
 * Retrieves the piece reference located on the tile, if any.
 */
ChessPiece* Tile::GetPiece() {
    return current_piece;
}

/**
 * Clear the piece on tile, use SetPiece to replace the piece if possible.
 */
void Tile::RemovePiece() {
    current_piece = nullptr;
}
