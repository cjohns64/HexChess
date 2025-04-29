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

struct {
    int rank;
    eFiles file;
} sCoords;
