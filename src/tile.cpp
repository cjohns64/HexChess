#include "../includes/tile.h"

// Contains information and functions associated with a single tile.
Tile::Tile(eColor tile_color, sCoords location): position(location.rank, location.file)  {
    tile_color = tile_color;
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
ChessPiece* Tile::GetPiece(bool testing) {
    if (testing && test_piece != NULL) {
        // prefer a testing piece if testing mode is on
        return test_piece;
    }
    return current_piece;
}

/**
 * Clear the piece on tile, use SetPiece to replace the piece if possible.
 */
void Tile::_RemovePiece_() {
    current_piece = NULL;
}

void Tile::TestPieceSet(ChessPiece* piece) {
    test_piece = piece;
}

void Tile::TestPieceRemove() {
    test_piece = NULL;
}

void Tile::TestPieceStash() {
    test_stashed = current_piece;
}

void Tile::TestPieceRestore() {
    current_piece = test_stashed;
}

