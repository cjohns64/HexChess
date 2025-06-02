#ifndef HEXCHESS_UTIL_H
#define HEXCHESS_UTIL_H
#include <string>

// game end state
enum eGameEnd {
    Running=-1,
    Checkmate = 0,
    Stalemate = 1,
    DeadPosistion = 2,
    ThreefoldRepitition = 3,
    FiftyMoveRule = 4
};

enum eFiles {
    a = 0,
    b = 1,
    c = 2,
    d = 3,
    e = 4,
    f = 5,
    g = 6,
    h = 7,
    i = 8,
    j = 9,
    k = 10
};

enum ePlayer {
    WhitePlayer = 0,
    BlackPlayer = 1
};

enum eType {
    King = 0,
    Queen = 1,
    Rook = 2,
    Bishop = 3,
    Knight = 4,
    Pawn = 5,
    NoPiece = 9,
    AnyPiece = 10,
};

enum eMoveReq {
    TileEmpty = 0,
    TileEnemy = 1,
    TileEmptyOrEnemy = 2,
    TileAlly = 3,
};

enum eColor {
    colorW = 0,
    colorB = 1,
    colorG = 2,
    HighlightSelect = 3,
    HighlightMove = 4,
    HighlightCapture = 5,
    HighlightCheck = 6
};

struct sCoords
{
    // absolute coordinates of a tile or piece
    int rank;
    eFiles file;
    sCoords(int _rank, eFiles _file);

    // overload == for comparing coordinates
    bool operator == (const sCoords& obj);
};

struct sRelCoords
{
    // defines the relative movement of a piece
    int rel_rank; // relative movement along a rank
    int rel_file; // relative movement along a file
    int repeat_max; // if the relative movement can be repeated in one move, like in the case of a Rook
    bool initial_only; // this move can only be used if the piece hasn't moved
    eType target_requirement; // target square must be of this type for this move to be valid
    eMoveReq tile_requirement; // target square must be of this type for this move to be valid
    sRelCoords(int _rel_rank, int _rel_file, int _repeat_max=99,
            eType _target_requirement=AnyPiece, eMoveReq _tile_requirement=TileEmptyOrEnemy, bool _initial_only=false);

    /**
     * Returns the relative movement in the opposite direction
     */
    sRelCoords invert();

    // overload == for comparing rel coordinates
    bool operator == (const sRelCoords& obj);
};

// overload +- operators for sCoords and sRelCoords combinations
inline sCoords operator + (const sRelCoords& opt1, const sCoords& opt2) {
        int new_rank = opt2.rank + opt1.rel_rank;
        int new_file = opt2.file + opt1.rel_file;
        return sCoords(new_rank, static_cast<eFiles>(new_file));
}

inline sCoords operator - (const sRelCoords& opt1, const sCoords& opt2) {
        int new_rank = opt2.rank - opt1.rel_rank;
        int new_file = opt2.file - opt1.rel_file;
        return sCoords(new_rank, static_cast<eFiles>(new_file));
}

inline sCoords operator + (const sCoords& opt1, const sRelCoords& opt2) {
        int new_rank = opt1.rank + opt2.rel_rank;
        int new_file = opt1.file + opt2.rel_file;
        return sCoords(new_rank, static_cast<eFiles>(new_file));
}

inline sCoords operator - (const sCoords& opt1, const sRelCoords& opt2) {
        int new_rank = opt1.rank - opt2.rel_rank;
        int new_file = opt1.file - opt2.rel_file;
        return sCoords(new_rank, static_cast<eFiles>(new_file));
}

inline sCoords operator + (const sCoords& opt1, const sCoords& opt2) {
        int new_rank = opt1.rank + opt2.rank;
        int new_file = opt1.file + opt2.file;
        return sCoords(new_rank, static_cast<eFiles>(new_file));
}

inline sCoords operator - (const sCoords& opt1, const sCoords& opt2) {
        int new_rank = opt1.rank - opt2.rank;
        int new_file = opt1.file - opt2.file;
        return sCoords(new_rank, static_cast<eFiles>(new_file));
}

// overload multiplication for sRelCoords
inline sRelCoords operator * (const int& x, const sRelCoords& obj) {
        int new_rank = obj.rel_rank * x;
        int new_file = obj.rel_file * x;
        return sRelCoords(new_rank, new_file, obj.repeat_max);
}

inline sRelCoords operator * (const sRelCoords& obj, const int& x) {
        int new_rank = obj.rel_rank * x;
        int new_file = obj.rel_file * x;
        return sRelCoords(new_rank, new_file, obj.repeat_max);
}

inline const char* ToString(eType type) {
    switch (type) {
        case King:      return "King";
        case Queen:     return "Queen";
        case Rook:      return "Rook";
        case Bishop:    return "Bishop";
        case Knight:    return "Knight";
        case Pawn:      return "Pawn";
        default:        return "Unknown";
    }
}

inline const char ToString(eFiles file) {
    return (char) (static_cast<int>(file) + 'a');
}

inline const std::string ToString(sCoords coords) {
    return std::to_string(coords.rank) + ToString(coords.file);
}

#endif
