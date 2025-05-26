#include "../includes/tile.h"

// Contains information and functions associated with a single tile.
Tile::Tile(eColor tile_color, sCoords location): position(location.rank, location.file)  {
    tile_color = tile_color;
    current_piece = nullptr;
    en_passant_piece = nullptr;
    test_piece = nullptr;
    test_stashed = nullptr;
}

sCoords Tile::GetLocation() {
    return position;
}

/**
 * Places the given piece on this tile, updating current_piece with the reference to the piece.
 */
void Tile::SetPiece(ChessPiece* piece, bool en_passant) {
    if (en_passant) {
        en_passant_piece = piece;
    }
    else {
        current_piece = piece;
    }
}

/**
 * Retrieves the piece reference located on the tile, if any.
 */
ChessPiece* Tile::GetPiece(bool testing, bool en_passant) {
    if (testing && test_piece != nullptr) {
        // prefer a testing piece if testing mode is on
        return test_piece;
    }
    else if (en_passant && en_passant_piece != nullptr) {
        // second priority is the en_passant_piece
        return en_passant_piece;
    }
    // default to current_piece
    return current_piece;
}

/**
 * Clear the piece on tile, use SetPiece to replace the piece if possible.
 */
void Tile::RemovePiece() {
    current_piece = nullptr;
}

void Tile::TestPieceSet(ChessPiece* piece) {
    test_piece = piece;
}

void Tile::TestPieceRemove() {
    test_piece = nullptr;
}

void Tile::TestPieceStash() {
    test_stashed = current_piece;
}

void Tile::TestPieceRestore() {
    current_piece = test_stashed;
}

