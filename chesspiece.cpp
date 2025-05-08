#include "./util.cpp"
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
        std::vector<sRelCoords> inital_moves;
        std::vector<sRelCoords> captures;
        // references to exact locations
        std::vector<sCoords*> valid_moves_this_turn;

    private:
        sCoords* location;

    public:
        ChessPiece(eType _type, ePlayer _player, sCoords* _location) {
            type = _type;
            player = _player;
            location = _location;
        }

        /**
         * Sets the location of the piece to the new value and updates is_unmoved to false.
         */
        void SetLocation(sCoords new_location) {
            location->rank = new_location.rank;
            location->file = new_location.file;
            is_unmoved = false;
        }

        /**
         * Returns the current location of the piece
         */
        sCoords GetLocation() {
            return sCoords(location->rank, location->file);
        }
};

class KingPiece: public ChessPiece {
    public:
        static const int value = 0;

        KingPiece(ePlayer _player, sCoords* _location) : ChessPiece(King, _player, _location) {
            // King can move to all adjacent tiles
            moves = {
                sRelCoords(1, 0), sRelCoords(-1, 0),
                sRelCoords(0, 1), sRelCoords(0, -1),
                sRelCoords(1, 1), sRelCoords(-1, -1)
            };
            // King can capture at movement tiles
            captures = moves;
        }
};

class QueenPiece: public ChessPiece {
    public:
        static const int value = 9;

        QueenPiece(ePlayer _player, sCoords* _location) : ChessPiece(Queen, _player, _location) {
            // Queen can move to all tiles allong a rank
            // and to all diagonal tiles
            moves = {
                // Rook moves
                sRelCoords(1, 0, true), sRelCoords(-1, 0, true),
                sRelCoords(0, 1, true), sRelCoords(0, -1, true),
                sRelCoords(1, 1, true), sRelCoords(-1, -1, true),
                // Bishop moves
                sRelCoords(1, -1, true), sRelCoords(-1, 1, true),
                sRelCoords(-2, -1, true), sRelCoords(2, 1, true),
                sRelCoords(1, 2, true), sRelCoords(-1, -2, true)
            };
            // Queen can capture at movement tiles
            captures = moves;
        }
};

class RookPiece: public ChessPiece {
    public:
        static const int value = 5;

        RookPiece(ePlayer _player, sCoords* _location) : ChessPiece(Rook, _player, _location) {
            // Rook can move to all tiles allong a rank
            moves = {
                sRelCoords(1, 0, true), sRelCoords(-1, 0, true),
                sRelCoords(0, 1, true), sRelCoords(0, -1, true),
                sRelCoords(1, 1, true), sRelCoords(-1, -1, true)
            };
            // Rook can capture at movement tiles
            captures = moves;
        }
};

class BishopPiece: public ChessPiece {
    public:
        static const int value = 3;

        BishopPiece(ePlayer _player, sCoords* _location) : ChessPiece(Bishop, _player, _location) {
            // Bishop can move to all diagonal tiles
            moves = {
                sRelCoords(1, -1, true), sRelCoords(-1, 1, true),
                sRelCoords(-2, -1, true), sRelCoords(2, 1, true),
                sRelCoords(1, 2, true), sRelCoords(-1, -2, true)
            };
            // Bishop can capture at movement tiles
            captures = moves;
        }
};

class KnightPiece: public ChessPiece {
    public:
        static const int value = 3;

        KnightPiece(ePlayer _player, sCoords* _location) : ChessPiece(Knight, _player, _location) {
            // Knight can move to the first tiles not on a rank or diagonal
            moves = {
                sRelCoords(3, 1), sRelCoords(-3, -1),
                sRelCoords(3, 2), sRelCoords(-3, -2),
                sRelCoords(2, -1), sRelCoords(-2, 1),
                sRelCoords(-2, -3), sRelCoords(2, 3),
                sRelCoords(-1, -3), sRelCoords(1, -2),
                sRelCoords(-1, 2), sRelCoords(1, 3)
            };
            // Knight can capture at movement tiles
            captures = moves;
        }
};

class PawnPiece: public ChessPiece {
    public:
        static const bool is_promotable = true;
        static const int value = 1;
        static const bool captures_with_moves = false;

        PawnPiece(ePlayer _player, sCoords* _location) : ChessPiece(Pawn, _player, _location) {
            // Pawn can move one tile in direction of opponent
            // Pawn can move two tile in direction of opponent first time it moves
            // Pawn can capture at first diagonals in direction of opponent
            if (player == WhitePlayer) {
                moves = {sRelCoords(1, 0)};
                inital_moves = {sRelCoords(2, 0)};
                captures = {sRelCoords(1, -1), sRelCoords(2, 1)};
            }
            else {
                moves = {sRelCoords(-1, 0)};
                inital_moves = {sRelCoords(-2, 0)};
                captures = {sRelCoords(-1, 1), sRelCoords(-2, -1)};
            }
        }
};
