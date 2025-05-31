#ifndef CHESSPICE_H
#define CHESSPICE_H
#include "./util.h"
#include <vector>
// full span of the board does not exceed 11
static const int MAX_BOARD_LENGTH = 11;

class ChessPiece {
    public:
        eType type; // the piece type: King, Queen, Rook, Bishop, Knight, Pawn
        ePlayer player; // the player for the piece: White, Black
        static const bool is_promotable = false; // if the piece can be promoted to a higher value piece
        static const int value;
        static const bool captures_with_moves = true;
        bool is_unmoved = true;
        // relative locations
        std::vector<sRelCoords> moves;
        int initial_move_repeat_count = 0;
        eType initial_move_target_requirement = NoPiece;
        std::vector<sRelCoords> captures;
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
        static const int value = 0;

        KingPiece(ePlayer _player, sCoords* _location);
};

class QueenPiece: public ChessPiece {
    public:
        static const int value = 9;

        QueenPiece(ePlayer _player, sCoords* _location);
};

class RookPiece: public ChessPiece {
    public:
        static const int value = 5;

        RookPiece(ePlayer _player, sCoords* _location);
};

class BishopPiece: public ChessPiece {
    public:
        static const int value = 3;

        BishopPiece(ePlayer _player, sCoords* _location);
};

class KnightPiece: public ChessPiece {
    public:
        static const int value = 3;

        KnightPiece(ePlayer _player, sCoords* _location);
};

class PawnPiece: public ChessPiece {
    public:
        static const bool is_promotable = true;
        static const int value = 1;
        static const bool captures_with_moves = false;

        PawnPiece(ePlayer _player, sCoords* _location);
};

inline bool operator == (ChessPiece first, ChessPiece other) {
    return first.type == other.type && first.GetLocation() == other.GetLocation() && first.player == other.player;
}
#endif
