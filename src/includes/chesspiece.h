#ifndef CHESSPICE_H
#define CHESSPICE_H
#include "util.h"
#include <vector>


class ChessPiece {
public:
    // full span of the board does not exceed 11
    static const int MAX_BOARD_LENGTH = 11;
    eType type; // the piece type: King, Queen, Rook, Bishop, Knight, Pawn
    ePlayer player; // the player for the piece: White, Black
    bool is_promotable = false; // if the piece can be promoted to a higher value piece
    int value;
    bool is_unmoved = true;
    bool is_alive = true;
    // relative locations
    std::vector<sRelCoords> moves;
    // references to exact locations
    std::vector<sCoords> valid_moves_this_turn;

private:
    sCoords* location;

public:
    ChessPiece(eType _type, ePlayer _player, sCoords* _location);

    /**
     * Sets the location of the piece to the new value and updates is_unmoved to false.
     */
    void SetLocation(sCoords new_location);

    /**
     * Returns the current location of the piece
     */
    sCoords GetLocation();
};

class KingPiece: public ChessPiece {
    public:
        int value = 0;

        KingPiece(ePlayer _player, sCoords* _location);
};

class QueenPiece: public ChessPiece {
    public:
        int value = 9;

        QueenPiece(ePlayer _player, sCoords* _location);
};

class RookPiece: public ChessPiece {
    public:
        int value = 5;

        RookPiece(ePlayer _player, sCoords* _location);
};

class BishopPiece: public ChessPiece {
    public:
        int value = 3;

        BishopPiece(ePlayer _player, sCoords* _location);
};

class KnightPiece: public ChessPiece {
    public:
        int value = 3;

        KnightPiece(ePlayer _player, sCoords* _location);
};

class PawnPiece: public ChessPiece {
    public:
        bool is_promotable = true;
        int value = 1;

        PawnPiece(ePlayer _player, sCoords* _location);
};

inline bool operator == (ChessPiece first, ChessPiece other) {
    return first.type == other.type && first.GetLocation() == other.GetLocation() && first.player == other.player;
}
#endif
