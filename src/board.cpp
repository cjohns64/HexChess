#include "includes/board.h"

vector<vector<Tile>> BOARD_INIT(11,vector<Tile>());

Board::Board() {
    // setup the inital chessboard
    init_board(BOARD_INIT);
    chessboard = BOARD_INIT;
};
/**
 * Checks if the given location is on the board
 */
bool Board::IsOnBoard(sCoords location) {
    // check rank is within range
    if (location.rank < 10 and location.rank >= 0) {
        // check file is within range
        switch (location.rank) {
            case 0:
                return location.file <= 5 and location.file >= 0;
            case 1:
                return location.file <= 6 and location.file >= 0;
            case 2:
                return location.file <= 7 and location.file >= 0;
            case 3:
                return location.file <= 8 and location.file >= 0;
            case 4:
                return location.file <= 9 and location.file >= 0;
            case 5:
                return location.file <= 10 and location.file >= 0;
            case 6:
                return location.file <= 9 and location.file >= 0;
            case 7:
                return location.file <= 8 and location.file >= 0;
            case 8:
                return location.file <= 7 and location.file >= 0;
            case 9:
                return location.file <= 6 and location.file >= 0;
            case 10:
                return location.file <= 5 and location.file >= 0;
            default:
                return false;
        }
    }
    else {
        return false;
    }
}

/**
 * Retrieves a reference to the tile at the given coordinates. Checks if the tile is on the board first.
 * Returns a pointer to the tile or a null pointer if the tile is not on the board.
 */
Tile* Board::GetTile(sCoords location) {
    if (IsOnBoard(location)) {
        return &chessboard[location.rank][location.file];
    }
    else {
        return nullptr;
    }
}

/**
 * Set the board to the state given by a list of pieces for both players, each piece will have its current location.
 */
void Board::SetBoard(std::vector<ChessPiece>& white_pieces, KingPiece& white_king, std::vector<ChessPiece>& black_pieces, KingPiece& black_king) {
    // set the white and black Kings
    GetTile(white_king.GetLocation())->SetPiece(&white_king);
    GetTile(black_king.GetLocation())->SetPiece(&black_king);
    // set the white pieces
    for (int i=0; i<white_pieces.size(); i++) {
        // get the location of the piece and the corresponding tile
        Tile* tile = GetTile(white_pieces[i].GetLocation());
        // set the piece on the tile, ignore if the tile is already full
        tile->SetPiece(&white_pieces[i]);
    }
    // set the black pieces
    for (int i=0; i<black_pieces.size(); i++) {
        // get the location of the piece and the corresponding tile
        Tile* tile = GetTile(black_pieces[i].GetLocation());
        // set the piece on the tile, ignore if the tile is already full
        tile->SetPiece(&black_pieces[i]);
    }
}
