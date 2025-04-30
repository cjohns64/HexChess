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
    int rank;
    eFiles file;
    sCoords(int _rank, eFiles _file) {
        rank = _rank;
        file = _file;
    }
};

struct sRelCoords {
    int rel_rank;
    int rel_file;
    bool repeat;
    sRelCoords(int _rel_rank, int _rel_file, bool _repeat=false) {
        rel_rank = _rel_rank;
        rel_file = _rel_file;
        repeat = _repeat;
    }
};
