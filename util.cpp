enum eFiles {
    a,
    b,
    c,
    d,
    e,
    f,
    g,
    h,
    i,
    j,
    k
};

enum ePlayer {
    WhitePlayer,
    BlackPlayer
};

enum eType {
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

enum eColor {
    colorW,
    colorB,
    colorG,
    HighlightSelect,
    HighlightMove,
    HighlightCapture,
    HighlightCheck
};

struct sCoords{
    // absolute coordinates of a tile or piece
    int rank;
    eFiles file;
    sCoords(int _rank, eFiles _file) {
        rank = _rank;
        file = _file;
    }
};

struct sRelCoords {
    // defines the reletive movement of a piece
    int rel_rank; // reletive movement along a rank
    int rel_file; // relative movement along a file
    bool repeat; // if the relative movement can be repeated in one move, like in the case of a Rook
    sRelCoords(int _rel_rank, int _rel_file, bool _repeat=false) {
        rel_rank = _rel_rank;
        rel_file = _rel_file;
        repeat = _repeat;
    }
};
