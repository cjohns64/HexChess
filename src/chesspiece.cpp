#include "includes/chesspiece.h"

ChessPiece::ChessPiece(eType _type, ePlayer _player, sCoords* _location) {
    type = _type;
    player = _player;
    location = _location;
};

/**
 * Sets the location of the piece to the new value and updates is_unmoved to false.
 */
void ChessPiece::SetLocation(sCoords new_location) {
    location->rank = new_location.rank;
    location->file = new_location.file;
    is_unmoved = false;
}

/**
 * Returns the current location of the piece
 */
sCoords ChessPiece::GetLocation() {
    return sCoords(location->rank, location->file);
}

KingPiece::KingPiece(ePlayer _player, sCoords* _location) : ChessPiece(King, _player, _location) {
    // TODO must move to a same-player Rook that has not moved. And trigger a castling event
    // King can move to all adjacent tiles
    moves = {
        // normal moves
        sRelCoords(1, 0, 0), sRelCoords(-1, 0, 0),
        sRelCoords(0, 1, 0), sRelCoords(0, -1, 0),
        sRelCoords(1, 1, 0), sRelCoords(-1, -1, 0),
        // castling moves
        sRelCoords(1, 0, 2, Rook, TileAlly, true),
        sRelCoords(-1, 0, 2, Rook, TileAlly, true),
        sRelCoords(0, 1, 2, Rook, TileAlly, true),
        sRelCoords(0, -1, 2, Rook, TileAlly, true),
        sRelCoords(1, 1, 2, Rook, TileAlly, true),
        sRelCoords(-1, -1, 2, Rook, TileAlly, true)
    };
    // King can capture at movement tiles
};

QueenPiece::QueenPiece(ePlayer _player, sCoords* _location) : ChessPiece(Queen, _player, _location) {
    // Queen can move to all tiles allong a rank
    // and to all diagonal tiles
    moves = {
        // Rook moves
        sRelCoords(1, 0), sRelCoords(-1, 0),
        sRelCoords(0, 1), sRelCoords(0, -1),
        sRelCoords(1, 1), sRelCoords(-1, -1),
        // Bishop moves
        sRelCoords(1, -1), sRelCoords(-1, 1),
        sRelCoords(-2, -1), sRelCoords(2, 1),
        sRelCoords(1, 2), sRelCoords(-1, -2)
    };
};

RookPiece::RookPiece(ePlayer _player, sCoords* _location) : ChessPiece(Rook, _player, _location) {
    // Rook can move to all tiles allong a rank
    moves = {
        sRelCoords(1, 0), sRelCoords(-1, 0),
        sRelCoords(0, 1), sRelCoords(0, -1),
        sRelCoords(1, 1), sRelCoords(-1, -1)
    };
};

BishopPiece::BishopPiece(ePlayer _player, sCoords* _location) : ChessPiece(Bishop, _player, _location) {
    // Bishop can move to all diagonal tiles
    moves = {
        sRelCoords(1, -1), sRelCoords(-1, 1),
        sRelCoords(-2, -1), sRelCoords(2, 1),
        sRelCoords(1, 2), sRelCoords(-1, -2)
    };
};

KnightPiece::KnightPiece(ePlayer _player, sCoords* _location) : ChessPiece(Knight, _player, _location) {
    // Knight can move to the first tiles not on a rank or diagonal
    moves = {
        sRelCoords(3, 1, 0), sRelCoords(-3, -1, 0),
        sRelCoords(3, 2, 0), sRelCoords(-3, -2, 0),
        sRelCoords(2, -1, 0), sRelCoords(-2, 1, 0),
        sRelCoords(-2, -3, 0), sRelCoords(2, 3, 0),
        sRelCoords(-1, -3, 0), sRelCoords(1, -2, 0),
        sRelCoords(-1, 2, 0), sRelCoords(1, 3, 0),
    };
};

PawnPiece::PawnPiece(ePlayer _player, sCoords* _location) : ChessPiece(Pawn, _player, _location) {
    // Pawn can move two tile in direction of opponent first time it moves
    // Pawn can move one tile in direction of opponent
    // Pawn can capture at first diagonals in direction of opponent
    if (player == WhitePlayer) {
        moves = {sRelCoords(1, 0, 0, NoPiece, TileEmpty),
            // initial move can repeat
            sRelCoords(1, 0, 1, NoPiece, TileEmpty, true),
            // capture moves
            sRelCoords(1, -1, 0, AnyPiece, TileEnemy),
            sRelCoords(2, 1, 0, AnyPiece, TileEnemy)};
    }
    else {
        moves = {sRelCoords(-1, 0, 0, NoPiece, TileEmpty),
            // initial move can repeat
            sRelCoords(-1, 0, 1, NoPiece, TileEmpty, true),
            // capture moves
            sRelCoords(-1, 1, 0, AnyPiece, TileEnemy),
            sRelCoords(-2, -1, 0, AnyPiece, TileEnemy)};
    }
};
